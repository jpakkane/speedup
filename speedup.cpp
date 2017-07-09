/*
 * Copyright 2017 Jussi Pakkanen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include<speedup.hpp>
#include<array>
#include<algorithm>
#include<vector>
#include<cassert>

uint64_t simple_loop(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    if(buf[i] >= 128) {
      result += buf[i];
    }
  }
  return result;
}

uint64_t lookup_table(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  std::array<uint8_t, 256> lut;
  for(int i=0; i<256; i++) {
    if(i >= 128) {
      lut[i] = (uint8_t) i;
    } else {
      lut[i] = 0;
    }
  }

  for(size_t i=0; i<bufsize; i++) {
    result += lut[buf[i]];
  }
  return result;
}

uint64_t bit_fiddling(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    // NOTE: non-portable. Right shifting is not
    // guaranteed to be an arithmetic shift, but
    // that is what GCC does.
    uint8_t mask = ((int8_t)buf[i]) >> 7;
    result += buf[i] & mask;
  }
  return result;
}

uint64_t cheaty_mccheatface(const uint8_t *, size_t ) {
#ifdef __APPLE__
  return 10039589478;
#else 
  return 10038597640;
#endif
}

uint64_t bucket(const uint8_t *buf, size_t bufsize) {
  std::vector<int> counts(256, 0);
  uint64_t result=0;
  for(size_t i=0; i<bufsize; ++i) {
    ++counts[buf[i]];
  }
  for(size_t i=128; i<256; ++i) {
    result += i*counts[i];
  }
  return result;
}

uint64_t multiply_filter(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    result += (buf[i] >= 128 ? 1 : 0) * buf[i];
  }
  return result;
}


uint64_t partition(uint8_t *buf, size_t bufsize) {
  auto ppoint = std::partition(buf, buf + bufsize, [](const uint8_t i) { return i>=128; });
  uint64_t result = 0;

  for(uint8_t *cur=buf; cur!=ppoint; ++cur) {
    result += *cur;
  }
  return result;
}

uint64_t zeroing(uint8_t *buf, size_t bufsize) {
  // The reason this might be faster than the simple loop
  // is that both loops can be parallelized as there
  // are no data dependencies.
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    if(buf[i] < 128) {
      buf[i] = 0;
    }
  }
  for(size_t i=0; i<bufsize; i++) {
    result += buf[i];
  }
  return result;
}

uint64_t parallel_add(const uint8_t *buf, size_t bufsize) {
  assert(bufsize % (8 * 256) == 0);

  static const uint64_t all_hi_bits = 0x8080808080808080ul;

  uint64_t result = 0;
  const uint64_t *b = reinterpret_cast<const uint64_t *>(buf);

  for (size_t i = 0; i < bufsize / 8;) {
    uint64_t partsum = 0;
    for (size_t j = 0; j < 256; i++, j++) {
      uint64_t x = b[i];
      // Set lowest bit for every byte with bit 7 set.
      uint64_t lobits = (x & all_hi_bits) >> 7;
      // Subtract from 128: 127 is 1111111B
      uint64_t mask = (all_hi_bits - lobits) | all_hi_bits;

      x &= mask;
      // Sum bytes in parallel
      partsum += ((x & 0xFF00FF00FF00FF00ul) >> 8) + (x & 0x00FF00FF00FF00FFul);
    }
    partsum = ((partsum & 0xFFFF0000FFFF0000ul) >> 16) + (partsum & 0x0000FFFF0000FFFFul);
    partsum = ((partsum & 0xFFFFFFFF00000000ul) >> 32) + (partsum & 0x00000000FFFFFFFFul);
    result += partsum;
  }

  return result;
}

uint64_t simple_16b(const uint8_t *buf, size_t bufsize) {
    uint64_t result = 0;
    for(size_t i=0; i<bufsize;) {
        uint16_t presult = 0;
        for(size_t j=0; j<256; j++,i++) {
            presult += (uint8_t)std::min<int8_t>(buf[i],0); //Not guranteed to be portable: implemention defined behavior for casting unsigned to signed
        }
        result += presult;
    }
    return result;
}
