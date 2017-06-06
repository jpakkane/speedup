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

import sys, os, shutil, json
import matplotlib.pyplot as plt

def find_max(measurements):
    y_max = 0
    for m in measurements:
        for t in m['times'].values():
            t = int(t)
            if t>y_max:
                y_max = t
    return y_max

def do_plots(mfile, outdir):
    platform = os.path.splitext(os.path.split(mfile)[1])[0]
    measurements = json.load(open(mfile))
    y_max = find_max(measurements)
    for i, m in enumerate(measurements):
        ofilename = os.path.join(outdir, '%d.png' % i)
        plot_single(platform, y_max, m, ofilename)

def plot_single(platform, y_max, measurements, ofilename):
    times = measurements['times']
    fig, ax = plt.subplots()
    fig.subplots_adjust(bottom=0.2)
    values = []
    methods = []
    for m in sorted(times.keys()):
        if m == 'cheaty':
            continue
        methods.append(m)
        values.append(int(times[m]))
    ind = list(range(len(values)))
    width = 0.8
    p1 = plt.bar(ind, values, width)

    compiler = measurements['compiler']
    buildtype = measurements['buildtype']
    is_sorted = 'sorted' if measurements['sort'] else 'unsorted'
    cpu_flag = measurements['cpu_flag']
    if cpu_flag == '':
        cpu_flag = 'no CPU flags'
    title = '%s: %s, %s, %s, %s' % (platform, compiler, buildtype, is_sorted, cpu_flag)
    plt.ylabel('Microseconds')
    plt.title(title)
    plt.xticks(ind, methods)
    plt.ylim(0, y_max)
    plt.setp(plt.gca().get_xticklabels(), rotation=45, horizontalalignment='right')
#    plt.show()
    plt.savefig(ofilename)
    plt.close(fig)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(sys.argv[0], 'measurement_file output_dir')
        sys.exit(1)
    mfile = sys.argv[1]
    outdir = sys.argv[2]
    if not os.path.isdir(outdir):
        os.mkdir(outdir)
    do_plots(mfile, outdir)

