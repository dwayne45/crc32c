// Copyright (c) 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "./crc32c_sse42.h"

#include <cstddef>
#include <cstdint>

#include "crc32c/crc32c_config.h"

#if defined(HAVE_SSE42)

#include "./crc32c_read_le.h"

#if defined(_MSC_VER)
#include <intrin.h>
#else  // !defined(_MSC_VER)
#include <nmmintrin.h>
#endif  // defined(_MSC_VER)

// For further improvements see Intel publication at:
// http://download.intel.com/design/intarch/papers/323405.pdf
std::uint32_t CRC32C_Extend_SSE42(
    std::uint32_t crc, const std::uint8_t* buf, std::size_t size) {
  const std::uint8_t *p = reinterpret_cast<const std::uint8_t *>(buf);
  const std::uint8_t *e = p + size;
  std::uint32_t l = crc ^ 0xffffffffu;

#define STEP1 do {                              \
    l = _mm_crc32_u8(l, *p++);                  \
} while (0)
#define STEP4 do {                              \
    l = _mm_crc32_u32(l, ReadUint32LE(p));      \
    p += 4;                                     \
} while (0)
#define STEP8 do {                              \
    l = _mm_crc32_u64(l, ReadUint64LE(p));      \
    p += 8;                                     \
} while (0)

  if (size > 16) {
    // Process unaligned bytes.
    for (unsigned int i = reinterpret_cast<std::uintptr_t>(p) % 8; i; --i) {
      STEP1;
    }
    // Process 8 bytes at a time.
    while (e - p >= 8) {
      STEP8;
    }
    // Process 4 bytes at a time.
    if (e - p >= 4) {
      STEP4;
    }
  }
  // Process the last few bytes.
  while (p != e) {
    STEP1;
  }
#undef STEP8
#undef STEP4
#undef STEP1
  return l ^ 0xffffffffu;
}

#endif  // defined(HAVE_SSE42)
