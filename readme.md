# DOptions Library

A simple argument parser for C++ programs


## How to install

### Global

```sh
git clone https://github.com/DavisLCVB/doptions.git
cd doptions
cmake -B build
cmake --install build
```
```

Then, on your CMakeLists.txt

```cmake
find_package(doptions REQUIRED)
target_link_libraries(mi_app PRIVATE doptions::doptions)
```

### As Subdirectory

```sh
git clone https://github.com/usuario/doptions.git vendor/doptions
```

Then, on your CMakeLists.txt

```cmake
add_subdirectory(vendor/doptions)
target_link_libraries(mi_app PRIVATE doptions::doptions)
```

>[!NOTE] Replace "vendor" with your third-party dependencies dir.

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(doptions
  GIT_REPOSITORY https://github.com/DavisLCVB/doptions.git
  GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(doptions)
target_link_libraries(mi_app PRIVATE doptions::doptions)
```

