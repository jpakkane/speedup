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

#include<speedup_compute.hpp>
#include<cstdio>
#include<vector>
#include<random>
#include<chrono>
#include<algorithm>
#include<cstring>
#include<boost/compute/core.hpp>

namespace compute = boost::compute;

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

  compute::device device = compute::system::default_device();
  compute::context context(device);
  compute::command_queue queue(context, device);
  printf("boost::compute with %s\n", device.name().c_str());

  // Kernels are compiled when first used and cached to
  // filesystem. This forces compilation / load before timing.
  {
    uint8_t tmpbuf[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    compute_transfer_only(tmpbuf, 8, context, queue);
    compute_simple_loop(tmpbuf, 8, context, queue);
    compute_partition(tmpbuf, 8, context, queue);
    compute_parallel_add(tmpbuf, 8, context, queue);
  }

  auto mutbuf = buf;
  auto mutbuf2 = buf;
  auto t0 = std::chrono::high_resolution_clock::now();
  auto simple_answer = simple_loop(buf.data(), buf.size());
  auto t1 = std::chrono::high_resolution_clock::now();

  auto compute_transfer_only_answer =
    compute_transfer_only(buf.data(), buf.size(), context, queue);
  auto t2 = std::chrono::high_resolution_clock::now();
  auto compute_simple_loop_answer =
    compute_simple_loop(buf.data(), buf.size(), context, queue);
  auto t3 = std::chrono::high_resolution_clock::now();
  auto compute_partition_answer =
    compute_partition(buf.data(), buf.size(), context, queue);
  auto t4 = std::chrono::high_resolution_clock::now();
  auto compute_parallel_add_answer =
    compute_parallel_add(buf.data(), buf.size(), context, queue);
  auto t5 = std::chrono::high_resolution_clock::now();

  const uint64_t correct_answer = simple_answer;
  if(simple_answer != correct_answer) {
    printf("Simple loop produced wrong answer: %ld\n", (long)simple_answer);
    failed++;
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
    printf("simple %ld μs\n", (long)count);
  }

  if(compute_transfer_only_answer != correct_answer) {
    printf("Compute_transfer_only produced wrong answer: %ld\n", (long)compute_transfer_only_answer);
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    printf("compute_transfer_only %ld μs\n", (long)count);
  }

  if(compute_simple_loop_answer != correct_answer) {
    printf("Compute_simple_loop produced wrong answer: %ld\n", (long)compute_simple_loop_answer);
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count();
    printf("compute_simple_loop %ld μs\n", (long)count);
  }

  if(compute_partition_answer != correct_answer) {
    printf("Compute_partition produced wrong answer: %ld\n", (long)compute_partition_answer);
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count();
    printf("compute_partition %ld μs\n", (long)count);
  }

  if(compute_parallel_add_answer != correct_answer) {
    printf("Compute_parallel_add produced wrong answer: %ld\n", (long)compute_parallel_add_answer);
  } else {
    int64_t count = std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count();
    printf("compute_parallel_add %ld μs\n", (long)count);
  }

  return failed;
}
