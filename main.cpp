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
#include<cstdio>
#include<vector>
#include<random>
#include<chrono>

constexpr const int BUFSIZE=100*1024*1024;

std::vector<uint8_t> create_random_array() {
  std::vector<uint8_t> buf;
  std::mt19937 gen(42); // For reproducibility.
  std::uniform_int_distribution<> dis(0, 255);

  buf.reserve(BUFSIZE);
  for(int i=0; i<BUFSIZE; i++) {
    buf.push_back(dis(gen));
  }
  return buf;
}

int main(int, char**) {
  int failed = 0;
  const uint64_t correct_answer = 10038597640;
  auto buf = create_random_array();
  auto mutbuf = buf;
  auto t0 = std::chrono::high_resolution_clock::now();
  auto simple_answer = simple_loop(buf.data(), buf.size());
  auto t1 = std::chrono::high_resolution_clock::now();
  auto cheaty_answer = cheaty_mccheatface(buf.data(), buf.size());
  auto t2 = std::chrono::high_resolution_clock::now();
  auto lut_answer = lookup_table(buf.data(), buf.size());
  auto t3 = std::chrono::high_resolution_clock::now();
  auto bit_answer = bit_fiddling(buf.data(), buf.size());
  auto t4 = std::chrono::high_resolution_clock::now();
  auto partition_answer = partition(mutbuf.data(), mutbuf.size());
  auto t5 = std::chrono::high_resolution_clock::now();

  if(simple_answer != correct_answer) {
    printf("Simple loop produced wrong answer: %ld\n", simple_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
    printf("Simple loop took %ld ms\n", count);
  }

  if(cheaty_answer != correct_answer) {
    printf("Cheaty produced wrong answer: %ld\n", cheaty_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    printf("Cheaty McCheatface took %ld ms\n", count);
  }

  if(lut_answer != correct_answer) {
    printf("Lookup table produced wrong answer: %ld\n", lut_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count();
    printf("Lookup table took %ld ms\n", count);
  }

  if(bit_answer != correct_answer) {
    printf("Bit fiddling produced wrong answer: %ld\n", bit_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count();
    printf("Bit fiddling took %ld ms\n", count);
  }

  if(partition_answer != correct_answer) {
    printf("Partitioning produced wrong answer: %ld\n", partition_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count();
    printf("Partitioning took %ld ms\n", count);
  }


  return failed;
}
