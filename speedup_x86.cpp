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

#include<speedup_x86.hpp>
#include<array>
#include<algorithm>
#include<vector>
#include<cassert>
#include<immintrin.h>

uint64_t simple_loop(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    if(buf[i] >= 128) {
      result += buf[i];
    }
  }
  return result;
}

uint64_t avx2(const uint8_t *buf, size_t bufsize) {
    const __m256i z   = _mm256_setzero_si256();
    const __m256i l8  = _mm256_set1_epi16(0xFF);
    const __m256i l16 = _mm256_set1_epi32(0xFFFF);
    const __m256i l32 = _mm256_set1_epi64x(0xFFFFFFFF);
    __m256i r = z;
    for(size_t i=0; i<bufsize;) {
        __m256i pr = z;
        for(size_t j=0; j<8; j++,i+=32) {
            __m256i m = _mm256_min_epi8(z, _mm256_loadu_si256((const __m256i*)(buf+i)));
            pr = _mm256_add_epi16(_mm256_and_si256(m, l8), pr);
            pr = _mm256_add_epi16(_mm256_srli_epi16(m, 8), pr);
        }
        r = _mm256_add_epi32(_mm256_and_si256(pr, l16), r);
        r = _mm256_add_epi32(_mm256_srli_epi32(pr, 16), r);
    }
    r = _mm256_add_epi64(_mm256_and_si256(r, l32), _mm256_srli_epi64(r, 32));
    __m128i r2 = _mm_add_epi64(_mm256_extractf128_si256(r, 1), _mm256_castsi256_si128(r));
    r2 = _mm_add_epi64(r2, _mm_bsrli_si128(r2, 8));
    
    return *(uint64_t*)&r2;
}
