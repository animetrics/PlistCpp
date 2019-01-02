# Building

## System Dependencies
In order to build this project, you need to have the following libraries installed:

* [UnitTest++](https://github.com/unittest-cpp/unittest-cpp)
* [Boost](https://www.boost.org)

On macOS this can conveniently be done through [Homebrew](https://brew.sh):

* `brew install unittest-cpp`

Or using [vcpkg](https://github.com/Microsoft/vcpkg):

* `vcpkg install boost unittest-cpp`

## Building with Tests
* `mkdir build && cd build`
* `cmake ..`
    * If you use `vcpkg`, add the flag `-DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake`
* `cmake --build build`
