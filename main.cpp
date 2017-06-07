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
#include<algorithm>
#include<cstring>

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
  auto cheaty_answer = cheaty_mccheatface(buf.data(), buf.size());
  auto t2 = std::chrono::high_resolution_clock::now();
  auto lut_answer = lookup_table(buf.data(), buf.size());
  auto t3 = std::chrono::high_resolution_clock::now();
  auto bit_answer = bit_fiddling(buf.data(), buf.size());
  auto t4 = std::chrono::high_resolution_clock::now();
  auto partition_answer = partition(mutbuf.data(), mutbuf.size());
  auto t5 = std::chrono::high_resolution_clock::now();
  auto zeroing_answer = zeroing(mutbuf2.data(), mutbuf.size());
  auto t6 = std::chrono::high_resolution_clock::now();
  auto bucket_answer = bucket(buf.data(), buf.size());
  auto t7 = std::chrono::high_resolution_clock::now();
  auto multiply_filter_answer = multiply_filter(buf.data(), buf.size());
  auto t8 = std::chrono::high_resolution_clock::now();
  auto parallel_add_lookup_answer = parallel_add_lookup(buf.data(), buf.size());
  auto t9 = std::chrono::high_resolution_clock::now();

  const uint64_t correct_answer = simple_answer;
  if(simple_answer != correct_answer) {
    printf("Simple loop produced wrong answer: %ld\n", (long)simple_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
    printf("simple %ld μs\n", (long)count);
  }
  /*
  if(cheaty_answer != correct_answer) {
    printf("Cheaty produced wrong answer: %ld\n", (long)cheaty_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    printf("cheaty %ld μs\n", (long)count);
  }
  */
  if(lut_answer != correct_answer) {
    printf("Lookup table produced wrong answer: %ld\n", (long)lut_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count();
    printf("lut %ld μs\n", (long)count);
  }

  if(bit_answer != correct_answer) {
    printf("Bit fiddling produced wrong answer: %ld\n", (long)bit_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count();
    printf("bitfiddle %ld μs\n", (long)count);
  }

  if(partition_answer != correct_answer) {
    printf("Partitioning produced wrong answer: %ld\n", (long)partition_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count();
    printf("partition %ld μs\n", (long)count);
  }


  if(zeroing_answer != correct_answer) {
    printf("Zeroing produced wrong answer: %ld\n", (long)zeroing_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count();
    printf("zeroing %ld μs\n", (long)count);
  }

  if(bucket_answer != correct_answer) {
    printf("Bucket produced wrong answer: %ld\n", (long)bucket_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t7-t6).count();
    printf("bucket %ld μs\n", (long)count);
  }

  if(multiply_filter_answer != correct_answer) {
    printf("Multi produced wrong answer: %ld\n", (long)multiply_filter_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t8-t7).count();
    printf("multiply %ld μs\n", (long)count);
  }

  if(parallel_add_lookup_answer != correct_answer) {
    printf("Parallel_add_lookup produced wrong answer: %ld\n", (long)parallel_add_lookup_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t9-t8).count();
    printf("paradd %ld μs\n", (long)count);
  }

  return failed;
}
