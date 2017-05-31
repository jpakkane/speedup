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
#include<array>

constexpr const int BUFSIZE=1024*1024;

std::array<uint8_t, BUFSIZE> create_random_array() {
  std::array<uint8_t, BUFSIZE> buf;
  return buf;
}

int main(int, char**) {
  auto buf = create_random_array();
  auto answer = simple_loop(buf.data(), buf.size());
  printf("The answer is %ld.\n", answer);
  return 0;
}
