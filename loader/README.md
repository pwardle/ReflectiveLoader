### https://github.com/octoml/macho-dyld
### (PW) Modified to support in-memory loading

### NOTE: Build this via cmake
### % cd loader/build
### % cmake ..
### % make 

### ...this will create library in build/ that the PoC links against. 

## Custom Mach-O Image Loader. Tolerant to unsigned modules.

### Purpose of this module
This is a simplified version of dynamic linker inherited from original Apple sources.
The key difference is in a switched off signature check for loaded binaries. iOS platform
doesn't provide mechanic to load unsigned binary, but you may need it for some developer
purposes like benchmarking/testing code with real device. Using of this linker version
let you get around this limitation.

This library exposes next symbols:
 - custom_dlopen
 - custom_dlclose
 - custom_dlsym
 - custom_dlerror

Use it instead of original Posix version.

### Known limitations
- Load only by absolute path
- There is no recurrent dependencies loading (all required modules should be
  preloaded in process before)
- Works only on system with enabled JIT permissions. Ex: iOS under debugger.
- Only RTLD_LAZY mode is supported 

### Borrowed files
- ImageLoader.h
- ImageLoader.cpp
- ImageLoaderMachO.h
- ImageLoaderMachO.cpp
- ImageLoaderMachOCompressed.h
- ImageLoaderMachOCompressed.cpp

All specific changes of original Apple code are under conditional compilation with 
`#if UNSIGN_TOLERANT` macros. All classes are moved into namespace "isolator" 
to avoid intersection with original symbols from libdyld.

The file `dyld_stubs.cpp` contains some utils and other stub functions to make this code 
compilable. Most of them has no implementation, just for signature compatibility.

### Link to original sources
https://opensource.apple.com/source/dyld/dyld-832.7.3

### Licence
All source files and implementations are provided under an [Apple Public Source License][APSL].
API header file is provided under [Apache License v2][ALv2].

[APSL]: https://opensource.apple.com/license/apsl
[ALv2]: https://www.apache.org/licenses/LICENSE-2.0
