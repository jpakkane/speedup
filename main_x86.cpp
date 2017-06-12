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
#include<cstdio>
#include<vector>
#include<random>
#include<chrono>
#include<algorithm>
#include<cstring>

constexpr const int BUFSIZE=10*1024*1024;

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

int main(int argc, char **argv) {
  int failed = 0;
  auto buf = create_random_array();
  if(argc > 1 && strcmp(argv[1], "--sort") == 0) {
    std::sort(buf.begin(), buf.end());
  }
  auto mutbuf = buf;
  auto mutbuf2 = buf;
  auto t0 = std::chrono::high_resolution_clock::now();
  auto simple_answer = simple_loop(buf.data(), buf.size());
  auto t1 = std::chrono::high_resolution_clock::now();
  auto avx2_answer = avx2(buf.data(), buf.size());
  auto t2 = std::chrono::high_resolution_clock::now();

  const uint64_t correct_answer = simple_answer;
  if(simple_answer != correct_answer) {
    printf("Simple loop produced wrong answer: %ld\n", (long)simple_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
    printf("simple %ld μs\n", (long)count);
  }

  if(avx2_answer != correct_answer) {
    printf("avx2 produced wrong answer: %ld\n", (long)avx2_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    printf("avx2 %ld μs\n", (long)count);
  }


  return failed;
}
