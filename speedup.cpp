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
  return 10038597640;
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
    result += (buf[i] >= 128) * buf[i];
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
