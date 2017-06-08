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

# Measuring and plotting

To run the full test measurement run do:

    ./measure.py output_file.json

Creating plots requires Matplotlib and is run like this:

    ./plot.py output_file.json output_dir

The plotter creates one picture per measurement setup.

# Contributing

If you have an algorithm you want to contribute, please file a pull request.

Some simple rules:

 - no inline assembly, threads, LTO or PGO
 - your submission does not need to be faster than the fastest so far just as long as it is interesting
 - no obviously stupid solutions
 - try to avoid undefined behaviour

# Measurements

Intel i7, GCC 6.3, -O2:

    paradd     38030 μs
    lut        54987 μs
    bucket     54990 μs
    multi      81016 μs
    simple     81256 μs
    bitfiddle  62836 μs
    partition 379627 μs
    zeroing   367643 μs

Raspberry Pi 2B+, Raspbian Jessie, GCC 4.9.2, -O2:

    Bit fiddling          988622 μs
    Bucket                992603 μs
    Simple loop          1104662 μs
    Parallel add lookup  1158125 μs
    Lookup table         1223642 μs
    Zeroing              1267950 μs
    Multi                1222455 μs
    Partitioning         1939782 μs

Please note that the measurements vary wildly between -O2 and -O3,
flags such as -mfpu=neon etc, the compiler used etc. Do your own
instead of blindly following these.
