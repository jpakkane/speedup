/*
 * Copyright 2017 Juhani Simola
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

#include"speedup.hpp"
#include<boost/compute/core.hpp>
#include<boost/compute/context.hpp>
#include<boost/compute/lambda.hpp>
#include<boost/compute/container/vector.hpp>
#include<boost/compute/algorithm/transform_reduce.hpp>
#include<boost/compute/algorithm/partition.hpp>

namespace compute = boost::compute;
using boost::compute::lambda::_1;

uint64_t simple_loop(const uint8_t *buf, size_t bufsize) {
  uint64_t result = 0;
  for(size_t i=0; i<bufsize; i++) {
    if(buf[i] >= 128) {
      result += buf[i];
    }
  }
  return result;
}

uint64_t compute_transfer_only(const uint8_t *buf, size_t bufsize,
                               boost::compute::context& context,
                               boost::compute::command_queue& queue) {
  compute::vector<uint8_t> device_buf(bufsize, context);
  compute::copy_n(buf, bufsize, device_buf.begin(), queue);
#ifdef __APPLE__
  return 10039589478;
#else
  return 10038597640;
#endif
}

BOOST_COMPUTE_FUNCTION(uint64_t, gt128_or_zero, (uint8_t x),
                       {
                         return x >= 128 ? x : 0;
                       });

uint64_t compute_simple_loop(const uint8_t *buf, size_t bufsize,
                             boost::compute::context& context,
                             boost::compute::command_queue& queue) {
  compute::vector<uint8_t> device_buf(bufsize, context);
  compute::copy_n(buf, bufsize, device_buf.begin(), queue);
  uint64_t result;
  compute::transform_reduce(device_buf.begin(), device_buf.end(), &result,
                            gt128_or_zero, compute::plus<uint64_t>(), queue);

  return result;
}

BOOST_COMPUTE_FUNCTION(uint64_t, u8_to_u64, (uint8_t x),
                       {
                         return x;
                       });

uint64_t compute_partition(const uint8_t *buf, size_t bufsize,
                           boost::compute::context& context,
                           boost::compute::command_queue& queue) {
  compute::vector<uint8_t> device_buf(bufsize, context);
  compute::copy_n(buf, bufsize, device_buf.begin(), queue);
  auto ppoint = compute::partition(device_buf.begin(), device_buf.end(),
                                   _1 >= 128, queue);
  uint64_t result;
  compute::transform_reduce(device_buf.begin(), ppoint, &result,
                            u8_to_u64, compute::plus<uint64_t>(), queue);
  return result;
}

BOOST_COMPUTE_FUNCTION(uint64_t, paradd_8, (uint64_t x),
                       {
                         const ulong all_hi_bits = 0x8080808080808080;
                         // Set lowest bit for every byte with high bit set.
                         ulong lobits = (x & all_hi_bits) >> 7;
                         // Subtract from 128: 127 is 1111111B
                         ulong mask = (all_hi_bits - lobits) | all_hi_bits;
                         x &= mask;
                         // Sum bytes in parallel
                         x = ((x & 0xFF00FF00FF00FF00ul) >> 8) + (x & 0x00FF00FF00FF00FFul);
                         x = ((x & 0xFFFF0000FFFF0000ul) >> 16) + (x & 0x0000FFFF0000FFFFul);
                         x = ((x & 0xFFFFFFFF00000000ul) >> 32) + (x & 0x00000000FFFFFFFFul);
                         return x;
                       });

uint64_t compute_parallel_add(const uint8_t *buf, size_t bufsize,
                              boost::compute::context& context,
                              boost::compute::command_queue& queue) {
  assert(bufsize % 8 == 0);
  const uint64_t *buf64 = (uint64_t*)buf;

  compute::vector<uint64_t> device_buf(bufsize / 8, context);
  compute::copy_n(buf64, bufsize / 8, device_buf.begin(), queue);
  uint64_t result;
  compute::transform_reduce(device_buf.begin(), device_buf.end(), &result,
                            paradd_8, compute::plus<uint64_t>(), queue);
  return result;
}
