## Building Instructions

Tuppence uses CMake to generate project files.

Open CMake and set the source code location to tuppence/src. It is recommended to set the build directory to tuppence/build.

Then Configure and Generate. The tuppence project is the interpreter and the test project builds an executable named runUnitTests for running the GTest tests for Tuppence.

If you are using Visual Studio and get an error `gtest.lib(gtest-all.obj) : error LNK2038: mismatch detected for 'RuntimeLibrary': value 'MTd_StaticDebug' doesn't match value 'MDd_DynamicDebug'`, make sure to check the gtest > gtest_force_shared_crt option in CMake.

```
cd tuppence
mkdir build-xcode
cd build-xcode
cmake -DLLVM_DIR=/Users/brenton/llvm/build/lib/cmake/llvm/ -DGTEST_ROOT=/Users/brenton/Documents/GitHub/googletest/build/install/ -G Xcode ..
```




