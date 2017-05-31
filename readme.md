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

# Currently fastest implementations

Ubuntu zesty, with an i7, GCC 6.3.0:

 - O2: lookup table and bucket
 - O3: bit fiddling

Raspberry Pi 2B+, Raspbian Jessie, GCC 4.9.2:

 - O2: bit fiddling and bucket
 - O3: simple loop

Macbook Pro, Clang 8.0.0:

 - O2: lookup
 - O3: lookup

If you have access to hardware not listed above, please submit
measurement results.
