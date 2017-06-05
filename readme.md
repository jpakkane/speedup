# Loop speedup experiment

[In a Reddit
discussion](https://www.reddit.com/r/cpp/comments/6dnj6d/writing_a_really_really_fast_json_parser/di4fb60/)
the following piece of code was posted:

    for (unsigned c = 0; c < arraySize; ++c) {
        if (data[c] >= 128)
            sum += data[c];
    }

This seems to be the optimal solution but it turns out not to be. This
repository is an attempt to look at different ways of making it faster
on real hardware.

# Compiling and running

    meson build
    cd build
    ninja
    ./speedup

To switch between O2 and O3 do:

    mesonconf -Dbuildtype=debugoptimized [or release]
    ninja
    ./speedup

# Contributing

If you have an algorithm you want to contribute, please file a pull request.

Some simple rules:

 - no inline assembly, threads, LTO or PGO
 - your submission does not need to be faster than the fastest so far just as long as it is interesting
 - no obviously stupid solutions
 - try to avoid undefined behaviour

# Measurements

MBPro, Clang 8.0, -O2:

    Parallel add lookup  35089 μs
    Lookup table         42877 μs
    Multi                53129 μs
    Bucket               92197 μs
    Simple loop          94968 μs
    Bit fiddling        117988 μs
    Zeroing             492027 μs
    Partitioning        519611 μs

Raspberry Pi 2B+, Raspbian Jessie, GCC 4.9.2, -O2:

    Bit fiddling         979319 μs
    Bucket               985220 μs
    Simple loop         1094934 μs
    Parallel add lookup 1208879 μs
    Multi               1213093 μs
    Lookup table        1215792 μs
    Zeroing             1256000 μs
    Partitioning        1922750 μs

Please note that the measurements vary wildly between -O2 and -O3,
flags such as -mfpu=neon etc, the compiler used etc. Do your own
instead of blindly following these.
