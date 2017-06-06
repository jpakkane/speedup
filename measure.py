#!/usr/bin/env python3

# Copyright 2017 Jussi Pakkanen
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import subprocess, sys, os, shutil, platform, json

meson_commands = ['meson', 'meson.py', '/home/jpakkane/workspace/meson/meson.py']

meson_bin = None

for c in meson_commands:
    if shutil.which(c):
        meson_bin = c
        break

if not meson_bin:
    sys.exit('Could not find Meson executable.')

def measure_one(builddir, compiler, extra_flag, sort, buildtype):
    if os.path.exists(builddir):
        shutil.rmtree(builddir)
    env = os.environ.copy()
    env['CXX'] = compiler
    env['CXXFLAGS'] = extra_flag
    if sort:
        sort_arg = ['--sort']
    else:
        sort_arg = []
    subprocess.check_call([meson_bin, builddir, '--buildtype=' + buildtype] , stdout=subprocess.DEVNULL, env=env)
    subprocess.check_call(['ninja', '-C', builddir], stdout=subprocess.DEVNULL)
    out = subprocess.check_output([os.path.join(builddir, 'speedup')] + sort_arg)
    out = out.decode('utf-8')
    result = {}
    for line in out.split('\n'):
        line = line.strip()
        if line == '':
            continue
        typename, duration, _ = line.split()
        result[typename] = duration
    shutil.rmtree(builddir)
    return result

def do_measurements():
    measurements = []
    if platform.processor() != 'x86_64':
        sys.exit('Unsupported CPU.')
    builddir = 'buildmeasurement'
    compilers = []
    for c in ['g++', 'clang++', 'cl']:
        if shutil.which(c):
            compilers.append(c)
    cpu_flags = ['', '-mavx', '-msse4.2', '-msse2', '-msse']
    for compiler in compilers:
        for cpu_flag in cpu_flags:
            for sort in [True, False]:
                for buildtype in ['debugoptimized', 'release']:
                    times = measure_one(builddir, compiler, cpu_flag, sort, buildtype)
                    measurements.append({'compiler': compiler,
                                         'cpu_flag': cpu_flag,
                                         'sort': sort,
                                         'buildtype': buildtype,
                                         'times': times,
                    })
    return measurements

if __name__ == '__main__':
    if not os.path.isfile('meson.build'):
        print('This script must be run in the top of the source dir.')
        sys.exit(1)
    measurements = do_measurements()
    json.dump(measurements, open('measurements.json', 'w'))

