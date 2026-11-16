// SPDX-License-Identifier: BSD-3-Clause
#ifndef SHENG_MEMORY_MAP_H
#define SHENG_MEMORY_MAP_H

// SEC calls this before constructing the MMU tables and resource HOBs.
VOID ShengPrepareMemoryMap (IN CONST VOID *Fdt);

#endif
