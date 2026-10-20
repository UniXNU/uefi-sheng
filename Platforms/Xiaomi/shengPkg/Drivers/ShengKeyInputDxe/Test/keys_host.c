// Host test of ShengSpmiKeys.h against a software model of the arbiter's observer channels.
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../ShengSpmiKeys.h"

static volatile uint32_t core[0x3000 / 4];
static volatile uint32_t obs[0x8000 / 4];
static volatile int stop, fail;
static uint8_t pon_sts, gpio_sts;
static int pon_apid = 9, gpio_apid = 21;

static void *device (void *arg)
{
  (void) arg;
  uint32_t seen[64] = {0};
  while (!stop) {
    for (int apid = 0; apid < 64; ++apid) {
      volatile uint32_t *ch = &obs[(SK_OBS_APID_STRIDE * apid) / 4];
      uint32_t cmd = ch[SK_CMD / 4];
      if (!cmd) { seen[apid] = 0; continue; }
      if (cmd == seen[apid]) continue;
      seen[apid] = cmd;
      assert ((cmd >> 27) == SK_OP_EXT_READL && ((cmd >> 4) & 0xff) == SK_REG_RT_STS && (cmd & 7) == 0);
      if (fail) { ch[SK_STATUS / 4] = SK_STATUS_DONE | 2; }
      else {
        ch[SK_RDATA0 / 4] = apid == pon_apid ? pon_sts : apid == gpio_apid ? gpio_sts : 0xee;
        ch[SK_STATUS / 4] = SK_STATUS_DONE;
      }
    }
    usleep (20);
  }
  return NULL;
}
// A new command for the same register must re-arm the model: clear the channel between reads.
static void rearm (void) { for (int a = 0; a < 64; ++a) { obs[(SK_OBS_APID_STRIDE * a) / 4 + 2] = 0; obs[(SK_OBS_APID_STRIDE * a) / 4] = 0; } usleep (2000); }   /* the model sees the idle channel before the next command */

int main (void)
{
  pthread_t t;
  struct sk_arbiter arb = { core, obs, 0 };
  struct sk_keys k;
  uint8_t pressed;
  int apid;

  core[0] = 0x70000000u; core[SK_FEATURES / 4] = 40;
  core[(SK_APID_MAP + 4 * 3) / 4] = (0x070u << 8) | 1;
  core[(SK_APID_MAP + 4 * pon_apid) / 4] = (SK_PPID_PON << 8) | 1;
  core[(SK_APID_MAP + 4 * gpio_apid) / 4] = ((uint32_t) SK_PPID_GPIO6 << 8) | 1;
  assert (sk_find_apid (&arb, SK_PPID_PON, &apid) == 0 && apid == pon_apid);
  assert (sk_find_apid (&arb, 0x555, &apid) == 3 && apid == -1);
  core[0] = 0x60000000u; assert (sk_find_apid (&arb, SK_PPID_PON, &apid) == 1);
  core[0] = 0x80000000u; assert (sk_find_apid (&arb, SK_PPID_PON, &apid) == 1);
  core[0] = 0x70000000u; core[SK_FEATURES / 4] = 0; assert (sk_find_apid (&arb, SK_PPID_PON, &apid) == 2);
  core[SK_FEATURES / 4] = 40;
  assert (sk_open (&arb, &k) == 0 && k.pon == pon_apid && k.gpio == gpio_apid);

  assert (pthread_create (&t, NULL, device, NULL) == 0);
  struct { uint8_t pon, gpio, want; } cases[] = {
    {0x00, 0x01, 0},                                   // nothing pressed (gpio high = released)
    {0x80, 0x01, SK_KEY_POWER}, {0x40, 0x01, SK_KEY_VOLDOWN}, {0xc0, 0x01, SK_KEY_POWER | SK_KEY_VOLDOWN},
    {0x00, 0x00, SK_KEY_VOLUP},                        // gpio low = pressed
    {0x3f, 0x01, 0},                                   // other PON status bits are not keys
    {0x80, 0x00, SK_KEY_POWER | SK_KEY_VOLUP},
  };
  for (unsigned i = 0; i < sizeof cases / sizeof *cases; ++i) {
    rearm (); pon_sts = cases[i].pon; gpio_sts = cases[i].gpio; fail = 0;
    int rc = sk_sample (&arb, &k, &pressed);
    if (rc || pressed != cases[i].want) fprintf (stderr, "case %u rc=%d got=%x want=%x\n", i, rc, pressed, cases[i].want);
    assert (rc == 0 && pressed == cases[i].want);
  }
  rearm (); fail = 1; assert (sk_sample (&arb, &k, &pressed) != 0 && pressed == 0);
  fail = 0;
  struct sk_keys nogpio = { pon_apid, -1 };
  rearm (); pon_sts = 0x80; gpio_sts = 0; assert (sk_sample (&arb, &nogpio, &pressed) == 0 && pressed == SK_KEY_POWER);
  stop = 1; pthread_join (t, NULL);

  // Debounce and repeat: two identical samples make a press; volume keys repeat after `delay` polls, then every `period`; power never.
  struct sk_filter f = {0};
  assert (sk_step (&f, SK_KEY_VOLUP, 4, 2) == 0);                       // first sample alone: not yet
  assert (sk_step (&f, SK_KEY_VOLUP, 4, 2) == SK_KEY_VOLUP);            // confirmed
  unsigned repeats = 0, polls = 0;
  for (; polls < 12; ++polls) repeats += sk_step (&f, SK_KEY_VOLUP, 4, 2) == SK_KEY_VOLUP;
  assert (repeats == 5);                                               // at held = 4, 6, 8, 10, 12
  assert (sk_step (&f, 0, 4, 2) == 0 && sk_step (&f, 0, 4, 2) == 0);   // release
  assert (sk_step (&f, SK_KEY_VOLDOWN, 4, 2) == 0 && sk_step (&f, SK_KEY_VOLDOWN, 4, 2) == SK_KEY_VOLDOWN);
  assert (sk_step (&f, SK_KEY_VOLDOWN, 4, 2) == 0);                    // a glitch of one sample is ignored below
  f = (struct sk_filter){0};
  assert (sk_step (&f, SK_KEY_POWER, 4, 2) == 0 && sk_step (&f, SK_KEY_POWER, 4, 2) == SK_KEY_POWER);
  for (unsigned i = 0; i < 40; ++i) assert (sk_step (&f, SK_KEY_POWER, 4, 2) == 0);   // power does not repeat
  f = (struct sk_filter){0};
  assert (sk_step (&f, SK_KEY_VOLUP, 4, 2) == 0 && sk_step (&f, 0, 4, 2) == 0 && sk_step (&f, SK_KEY_VOLUP, 4, 2) == 0);   // bounce: no press
  puts ("PASS: arbiter lookup, key decoding, read errors, debounce, repeat");
  return 0;
}
