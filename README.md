# scnlib

[![Linux builds](https://github.com/eliaskosunen/scnlib/actions/workflows/linux.yml/badge.svg)](https://github.com/eliaskosunen/scnlib/actions/workflows/linux.yml)
[![macOS builds](https://github.com/eliaskosunen/scnlib/actions/workflows/macos.yml/badge.svg)](https://github.com/eliaskosunen/scnlib/actions/workflows/macos.yml)
[![Windows builds](https://github.com/eliaskosunen/scnlib/actions/workflows/windows.yml/badge.svg)](https://github.com/eliaskosunen/scnlib/actions/workflows/windows.yml)
[![Other architectures](https://github.com/eliaskosunen/scnlib/actions/workflows/arch.yml/badge.svg)](https://github.com/eliaskosunen/scnlib/actions/workflows/arch.yml)
[![Code Coverage](https://codecov.io/gh/eliaskosunen/scnlib/graph/badge.svg?token=LyWrDluna1)](https://codecov.io/gh/eliaskosunen/scnlib)

[![Latest Release](https://img.shields.io/github/v/release/eliaskosunen/scnlib?sort=semver&display_name=tag)](https://github.com/eliaskosunen/scnlib/releases)
[![License](https://img.shields.io/github/license/eliaskosunen/scnlib.svg)](https://github.com/eliaskosunen/scnlib/blob/master/LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20%2F23-blue.svg)](https://img.shields.io/badge/C%2B%2B-17%2F20%2F23-blue.svg)
[![Documentation](https://img.shields.io/badge/Documentation-scnlib.dev-blue)](https://scnlib.dev)

```cpp
#include <scn/scan.h>
#include <print> // for std::println (C++23)

int main() {
    // Read two integers from stdin
    // with an accompanying message
    if (auto result =
            scn::prompt<int, int>("What are your two favorite numbers? ", "{} {}")) {
        auto [a, b] = result->values();
        std::println("Oh, cool, {} and {}!", a, b);
    } else {
        std::println(stderr, "Error: {}", result.error().msg());
    }
}
```

Try out in [Compiler Explorer](https://godbolt.org/z/djzKx6Evz).

## What is this?

`scnlib` is a modern C++ library for replacing `scanf` and `std::istream`.
This library attempts to move use ever so much closer to replacing `iostream`s
and C stdio altogether.
It's faster than `iostream` (see Benchmarks), and type-safe, unlike `scanf`.
Think [{fmt}](https://github.com/fmtlib/fmt) or C++20 `std::format`, but in the
other direction.

This library (or rather, the v2 version of it) is the reference implementation of the ISO C++ standards proposal
[P1729 "Text Parsing"](https://wg21.link/p1729).

The previous major release (v1.1.3) is hosted at the `v1`-branch.
It has a substantially different interface, and support for C++11 and C++14,
but it's unlikely that it'll get updated.

## Documentation

The documentation can be found online, from https://scnlib.dev.

To build the docs yourself, build the `scn_docs` target generated by CMake.
These targets are generated only if the variable `SCN_DOCS` is set in CMake
(done automatically if scnlib is the root project).
The `scn_docs` target requires Doxygen, Python 3.8 or better, and the `pip3`
package `poxy`.

## Examples

See more examples in the `examples/` folder.

### Reading a `std::string`

```cpp
#include <scn/scan.h>
#include <print>

int main() {
    // Reading a std::string will read until the first whitespace character
    if (auto result = scn::scan<std::string>("Hello world!", "{}")) {
        // Will output "Hello":
        // Access the read value with result->value()
        std::println("{}", result->value());
        
        // Will output " world":
        // result->range() returns a subrange containing the unused input
        // C++23 is required for the std::string_view range constructor used below
        std::println("{}", std::string_view{result->range()});
    } else {
        std::println("Couldn't parse a word: {}", result.error().msg());
    }
}
```

### Reading multiple values

```cpp
#include <scn/scan.h>

int main() {
    auto input = std::string{"123 456 foo"};
    
    auto result = scn::scan<int, int>(input, "{} {}");
    // result == true
    // result->range(): " foo"
    
    // All read values can be accessed through a tuple with result->values()
    auto [a, b] = result->values();
    
    // Read from the remaining input
    // Could also use scn::ranges::subrange{result->begin(), result->end()} as input
    auto result2 = scn::scan<std::string>(result->range(), "{}");
    // result2 == true
    // result2->range().empty() == true
    // result2->value() == "foo"
}
```

### Reading from a fancier range

```cpp
#include <scn/scan.h>

// scn::ranges is
//  - std::ranges on C++20 or later (if available)
//  - nano::ranges on C++17 (bundled implementation)
namespace ranges = scn::ranges;

int main() {
    auto result = scn::scan<int>("123" | ranges::views::reverse, "{}");
    // result == true
    // result->begin() is an iterator into a reverse_view
    // result->range() is empty
    // result->value() == 321
}
```

### Repeated reading

```cpp
#include <scn/scan.h>
#include <vector>

int main() {
    std::vector<int> vec{};
    auto input = scn::ranges::subrange{std::string_view{"123 456 789"}};
    
    while (auto result = scn::scan<int>(input), "{}")) {
        vec.push_back(result->value());
        input = result->range();
    }
}
```

## Features

* Blazing fast parsing of values (see Benchmarks)
* Modern C++ interface, featuring
    * type safety (variadic templates, types not determined by the format
      string)
    * convenience (ranges)
    * ergonomics (values returned from `scn::scan`, no output parameters)
* `"{python}"`-like format string syntax
    * Including compile-time format string checking
* Minimal code size increase (in user code, see Benchmarks)
* Usable without exceptions, RTTI, or `<iostream>`s
    * Configurable through build flags
    * Limited functionality if enabled
* Supports, and requires Unicode (input is UTF-8, UTF-16, or UTF-32)
* Highly portable
    * Tested on multiple platforms, see CI
    * Works on multiple architectures, tested on x86, x86-64, arm, aarch64,
      riscv64, ppc64le, and riscv64

## Installing

`scnlib` uses CMake.
If your project already uses CMake, integration should be trivial, through
whatever means you like:
`make install` + `find_package`, `FetchContent`, `git submodule` + `add_subdirectory`,
or something else.

The `scnlib` CMake target is `scn::scn`

```cmake
# Target with which you'd like to use scnlib
add_executable(my_program ...)
target_link_libraries(my_program scn::scn)
```

See docs for usage without CMake.

## Compiler support

A C++17 compatible compiler is required. The following compilers are tested in
CI:

* GCC 7 and newer
* Clang 8 and newer
* Visual Studio 2019 and 2022

Including the following environments:

* 32-bit and 64-bit builds on Windows
* libc++ on Linux
* AppleClang on macOS 11 (Big Sur) and 12 (Monterey)
* clang-cl with VS 2019 and 2022
* MinGW
* GCC on armv6, armv7, aarch64, riscv64, s390x, and ppc64le
* Visual Studio 2022, cross compiling to arm64

## Benchmarks

### Run-time performance

All times below are in nanoseconds of CPU time.
Lower is better.

#### Integer parsing (`int`)

![Integer result, chart](benchmark/runtime/results/int.png)

| Test                     | Test 1 `"single"` | Test 2 `"repeated"` |
|:-------------------------|------------------:|--------------------:|
| `scn::scan`              |              27.2 |                34.4 |
| `scn::scan_value`        |              22.2 |                29.9 |
| `scn::scan_int`          |              16.8 |                24.7 |
| `std::stringstream`      |               112 |                56.2 |
| `sscanf`                 |              72.1 |                 477 |
| `strtol`                 |              16.5 |                24.5 |
| `std::from_chars`        |              8.16 |                13.5 |
| `fast_float::from_chars` |              7.23 |                12.0 |

#### Floating-point number parsing (`double`)

![Float result, chart](benchmark/runtime/results/float.png)

| Test                     | Test 1 `"single"` | Test 2 `"repeated"` |
|:-------------------------|------------------:|--------------------:|
| `scn::scan`              |              66.3 |                82.7 |
| `scn::scan_value`        |              61.9 |                76.7 |
| `std::stringstream`      |               270 |                 272 |
| `sscanf`                 |               161 |                 713 |
| `strtod`                 |              85.1 |                 155 |
| `std::from_chars`        |              15.7 |                29.1 |
| `fast_float::from_chars` |              16.6 |                29.1 |

#### String "word" (whitespace-separated character sequence) parsing (`string` and `string_view`)

![String result, chart](benchmark/runtime/results/string.png)

| Test                           |      |
|:-------------------------------|-----:|
| `scn::scan<string>`            | 32.4 |
| `scn::scan<string_view>`       | 25.2 |
| `scn::scan_value<string>`      | 24.5 |
| `scn::scan_value<string_view>` | 20.7 |
| `std::stringstream`            |  127 |
| `sscanf`                       | 99.7 |

#### Conclusions

* `scn::scan` is always faster than using `stringstream`s and `sscanf`
* `std::from_chars`/`fast_float::from_chars` is faster than `scn::scan`, but it
  supports fewer features
* `strtod` is slower than `scn::scan`, and supports fewer features
* `scn::scan_value` is slightly faster compared to `scn::scan`
* `scn::scan_int` is faster than both `scn::scan` and `scn::scan_value`
* `strtol` is ~on-par with `scn::scan_int`.

#### About

Above,

* "Test 1" refers to scanning a single value from a string,
  which only contains the text representation for that value.
  The time used for creating any state needed for the scanner is included,
  for example, constructing a `stringstream`. This test is called `"single"` in
  the benchmark sources.
* "Test 2" refers to the average time of scanning a value,
  which contains multiple values in their text representations, separated by
  spaces. The time used for creating any state needed for the scanner
  is not included. This test is called `"repeated"` in the benchmark sources.
* The string test is an exception: strings are read one after another from a
  sample of Lorem Ipsum.

The difference between "Test 1" and "Test 2" is most pronounced when using
a `stringstream`, which is relatively expensive to construct,
and seems to be adding around ~100ns of runtime.
With `sscanf`, it seems like using the `%n` specifier and skipping whitespace
are really expensive (~400ns of runtime).
With `scn::scan` and `std::from_chars`, there's really no state to construct,
and the results for "Test 1" and "Test 2" are thus quite similar.

These benchmarks were run on a Fedora 39 machine, running Linux kernel version
6.6.8, with an AMD Ryzen 7 5700X processor, and compiled with clang version
17.0.6,
with `-O3 -DNDEBUG -march=haswell` and LTO enabled.
C++20 was used, with the library-bundled ranges implementation (`nanorange`).
These benchmarks were run on 2024-01-09 (commit 629c3c5).

The source code for these benchmarks can be found in the `benchmark` directory.
You can run these benchmarks yourself by enabling the CMake
variable `SCN_BENCHMARKS`.
This variable is `ON` by default, if `scnlib` is the root CMake project,
and `OFF` otherwise.

```sh
$ cd build
$ cmake -DSCN_BENCHMARKS=ON \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
        -DSCN_USE_HASWELL_ARCH=ON ..
$ cmake --build .
# choose benchamrks to run in ./benchmark/runtime/*/*_bench
$ ./benchmark/runtime/integer/scn_int_bench
```

### Executable size

All sizes below are in kibibytes (KiB), measuring the compiled executable.
"Stripped size" shows the size of the executable after running `strip`.
Lower is better.

#### Release build (`-O3 -DNDEBUG` + LTO)

![Release result, chart](benchmark/binarysize/graph-release.png)

Size of `scnlib` shared library (`.so`): 1.4M

| Method         | Executable size | Stripped size |
|:---------------|----------------:|--------------:|
| empty          |            16.1 |          14.6 |
| `std::scanf`   |            17.4 |          14.8 |
| `std::istream` |            17.8 |          14.8 |
| `scn::input`   |            17.7 |          14.8 |

#### Minimized (MinSizeRel) build (`-Os -DNDEBUG` + LTO)

![MinSizeRel result, chart](benchmark/binarysize/graph-minsizerel.png)

Size of `scnlib` shared library (`.so`): 1.1M

| Method         | Executable size | Stripped size |
|:---------------|----------------:|--------------:|
| empty          |            16.1 |          14.6 |
| `std::scanf`   |            17.3 |          14.8 |
| `std::istream` |            17.7 |          14.8 |
| `scn::input`   |            18.8 |          14.8 |

#### Debug build (`-g -O0`)

![Debug result, chart](benchmark/binarysize/graph-debug.png)

Size of `scnlib` shared library (`.so`): 19M

| Method         | Executable size | Stripped size |
|:---------------|----------------:|--------------:|
| empty          |            25.6 |          14.6 |
| `std::scanf`   |             569 |          26.9 |
| `std::istream` |             527 |          18.8 |
| `scn::input`   |            2112 |          42.8 |

#### Conclusions

When using optimized builds, depending on compiler flags, scnlib provides a
binary, the size of which is within ~5% of what would be produced with `scanf`
or `<iostream>`s. Interestingly, when doing a MinSizeRel-build,
the scnlib binary is bigger, than when doing a Release-build.
In a Debug-environment, scnlib is ~5x bigger when compared to `scanf`
or `<iostream>`. After `strip`ing the binaries,
these differences largely go away, except in Debug builds.

#### About

In these tests, 25 translation units are generated, in all of which values are
read from `stdin` five times.
This is done to simulate a small project.
`scnlib` is linked dynamically, to level the playing field with the standard
library, which is also dynamically linked.

The code was compiled on Fedora 39, with gcc 13.2.1.
See the directory `benchmark/binarysize` for the source code.

You can run these benchmarks yourself by enabling the CMake
variable `SCN_BENCHMARKS_BINARYSIZE`.
This variable is `ON` by default, if `scnlib` is the root CMake project,
and `OFF` otherwise.

```sh
$ cd build
# For Debug
$ cmake -DCMAKE_BUILD_TYPE=Debug \
        -DSCN_BENCHMARKS_BINARYSIZE=ON \
        -DBUILD_SHARED_LIBS=ON ..
# For Release and MinSizeRel,
# add -DCMAKE_BUILD_TYPE=$BUILD_TYPE and
# -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

$ cmake --build .
$ ./benchmark/binarysize/run_binarysize_bench.py ./benchmark/binarysize $BUILD_TYPE
```

### Build time

#### Build time

Time is in seconds of CPU time (user time + sys/kernel time).
Lower is better.

| Method       | Debug | Release |
|:-------------|------:|--------:|
| empty        |  0.05 |    0.05 |
| `scanf`      |  0.20 |    0.19 |
| `<iostream>` |  0.26 |    0.25 |
| `scn::input` |  1.06 |    0.97 |

#### Memory consumption

Memory is in mebibytes (MiB) used while compiling.
Lower is better.

| Method       | Debug | Release |
|:-------------|------:|--------:|
| empty        |  21.0 |    23.3 |
| `scanf`      |  54.7 |    52.4 |
| `<iostream>` |  66.4 |    63.9 |
| `scn::input` |   203 |     185 |

#### Conclusions

Code using scnlib takes around 3x-5x longer to compile compared to `<iostream>`,
and also uses around 3x-4x more memory.
Debug and Release builds make no major difference.

#### About

This tests measures the time it takes to compile a binary, when using different
libraries.
The time taken to compile the library itself is not taken into account (the
standard library is precompiled, anyway).

These tests were run on a Fedora 39 machine, with an AMD Ryzen 7 5700X
processor, using gcc version 13.2.1.
The compiler flags used for a Debug build were `-g`, and `-O3 -DNDEBUG` for a
Release build.

You can run these benchmarks yourself by enabling the CMake
variable `SCN_BENCHMARKS_BUILDTIME`.
This variable is `ON` by default, if `scnlib` is the root CMake project,
and `OFF` otherwise.
In order for these tests to work, `c++` must point to a gcc-compatible C++
compiler binary,
and a somewhat POSIX-compatible `/usr/bin/time` must be available.

```sh
$ cd build
$ cmake -DSCN_BENCMARKS_BUILDTIME=ON ..
$ cmake --build .
$ ./benchmark/buildtime/run-buildtime-tests.sh
```

## Acknowledgements

The contents of this library are heavily influenced by {fmt} and its derivative
works.  
https://github.com/fmtlib/fmt

The design of this library is also inspired by the Python `parse` library:  
https://github.com/r1chardj0n3s/parse

### Third-party libraries

NanoRange for C++17 Ranges implementation:  
https://github.com/tcbrindle/NanoRange

fast_float for floating-point number parsing:  
https://github.com/fastfloat/fast_float

simdutf for Unicode handling:  
https://github.com/simdutf/simdutf

## License

scnlib is licensed under the Apache License, version 2.0.  
Copyright (c) 2017 Elias Kosunen  
See LICENSE for further details.
