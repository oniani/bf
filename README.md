# project

A zero-dependency, header-only, and minimal C++ bloom filter.

## API

```cpp
#include "include/bf.hpp"

int main() {
  auto bf = bf::BloomFilter{6, 1e-2};
  bf.insert("hi");
}
```

## Build

```console
$ mkdir build
$ cd build
$ cmake -DCOMPILER=clang -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=VCPKG_TOOLCHAIN_FILE ..
$ cmake --build .
```

## References

- [Bloom filter][bloomfilter]

## License

[MIT License][license]

[bloomfilter]: https://en.wikipedia.org/wiki/Bloom_filter
[license]: LICENSE
