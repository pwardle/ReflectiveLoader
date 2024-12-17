/* -*- mode: C++; c-basic-offset: 4; tab-width: 4 -*-
 *
 * Copyright (c) 2004-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#include <custom_dlfcn.h>
#include <fstream>
#include <sys/mman.h>

#include "ImageLoaderMachO.h"

#include "mach-o/dyld.h"

#import <mach-o/fat.h>
#import <mach-o/arch.h>
#import <mach-o/swap.h>
#import <mach-o/loader.h>



namespace isolator {

// actual definition for opaque type
struct __NSObjectFileImage
{
    ImageLoader*    image;
    const void*        imageBaseAddress;    // not used with OFI created from files
    size_t            imageLength;        // not used with OFI created from files
};
typedef __NSObjectFileImage*  NSObjectFileImage;

static NSModule _dyld_link_module(NSObjectFileImage object_addr, size_t object_size, const char* moduleName, uint32_t options);


extern ImageLoader::LinkContext g_linkContext;

thread_local char* _err_buf = nullptr;
thread_local size_t _err_buf_size = 0;

void clean_error() {
  if (_err_buf)
    _err_buf[0] = 0;
}

void set_dlerror(const std::string &msg) {
  if (msg.size() >= _err_buf_size) {
    if (_err_buf)
      free(_err_buf);

    _err_buf = static_cast<char*>(malloc(msg.size() + 1));
    _err_buf_size = msg.size() + 1;
  }
  strcpy(_err_buf, msg.c_str());
}

static bool is_absolute_path(const char * path) {
  return path && path[0] == '/';
}

std::string base_name(const std::string &path) {
  return path.substr(path.find_last_of("/\\") + 1);
}

void* with_limitation(std::string msg) {
    static std::string disclaimer = "\n"
            "DISCLAIMER: You are using non system mach-o dynamic loader. "
            "Avoid to using it in production code.\n";
    set_dlerror("Limitation: " + msg + disclaimer);
    return nullptr;
}

void* with_error(std::string msg) {
  set_dlerror(msg);
  return nullptr;
}

extern "C" char* custom_dlerror(void) {
    if (_err_buf && _err_buf[0] == 0)
        return nullptr;

    return _err_buf;
}

extern "C" void* custom_dlopen(const char * __path, int __mode) {
  try {
    clean_error();
    if (!is_absolute_path(__path))
      return with_limitation("Only absolute path is supported. Please specify "
                             "full path to binary.");

    std::fstream lib_f(__path, std::ios::in | std::ios::binary);
    if (!lib_f.is_open())
      return with_error("File does not exist.");

    std::streampos fsize = lib_f.tellg();
    lib_f.seekg(0, std::ios::end);
    fsize = lib_f.tellg() - fsize;
    lib_f.seekg(0, std::ios::beg);

    std::vector<char> buff(fsize);
    lib_f.read(buff.data(), fsize);
    lib_f.close();

    std::string file_name = base_name(__path);
    auto mh = reinterpret_cast<const macho_header*>(buff.data());
      
    // Load image step
    auto image = ImageLoaderMachO::instantiateFromMemory(file_name.c_str(), mh, fsize, g_linkContext);
      
    bool forceLazysBound = true;
    bool preflightOnly = false;
    bool neverUnload = false;

    // Link step
    std::vector<const char*> rpaths;
    ImageLoader::RPathChain loaderRPaths(NULL, &rpaths);
    image->link(g_linkContext, forceLazysBound, preflightOnly, neverUnload, loaderRPaths, __path);
      
    
    // Initialization of static objects step
    ImageLoader::InitializerTimingList initializerTimes[1];
    initializerTimes[0].count = 0;
    image->runInitializers(g_linkContext, initializerTimes[0]);

      
    return image;
  } catch (const char * msg) {
      printf("custom_dlopen: error %s\n", msg);
      
    return with_error("Error happens during dlopen execution. " + std::string(msg));
  } catch (...) {
      printf("custom_dlopen: error ??\n");
      
    return with_error("Error happens during dlopen execution. Unknown reason...");
  }
}

extern "C" void* custom_dlopen_from_memory(void* mh, int len) {
  
    const char * path = "foobar";
    
    // Load image step
    auto image = ImageLoaderMachO::instantiateFromMemory(path, (macho_header*)mh, len, g_linkContext);
      
    printf("dyld: 'ImageLoaderMachO::instantiateFromMemory' completed (image addr: %p)\n", image);

    bool forceLazysBound = true;
    bool preflightOnly = false;
    bool neverUnload = false;

    // Link step
    std::vector<const char*> rpaths;
    ImageLoader::RPathChain loaderRPaths(NULL, &rpaths);
    image->link(g_linkContext, forceLazysBound, preflightOnly, neverUnload, loaderRPaths, path);
    
    printf("dyld: 'image->link' completed\n");
    
    // Initialization of static objects step
    ImageLoader::InitializerTimingList initializerTimes[1];
    initializerTimes[0].count = 0;
    image->runInitializers(g_linkContext, initializerTimes[0]);
    
    printf("dyld: 'image->runInitializers' completed\n");

    return image;
  
}


extern "C" void* custom_dlsym(void * __handle, const char * __symbol) {
  try {
    clean_error();

    std::string underscoredName = "_" + std::string(__symbol);
    const ImageLoader* image = reinterpret_cast<ImageLoader*>(__handle);

    auto sym = image->findExportedSymbol(underscoredName.c_str(), true, &image);
    if (sym != NULL) {
      auto addr = image->getExportedSymbolAddress(sym, g_linkContext, nullptr, false,
                                                  underscoredName.c_str());
      return reinterpret_cast<void*>(addr);
    }
    return with_error("Symbol " + std::string(__symbol) + " is not found.");
  } catch (const char * msg) {
    return with_error("Error happens during dlsym execution. " + std::string(msg));
  } catch (...) {
    return with_error("Error happens during dlsym execution. Unknown reason...");
  }
}

extern "C" int custom_dlclose(void * __handle) {
  if (__handle == nullptr) {
    set_dlerror("Error happens during dlclose execution. Handle does not refer "
                "to an open object.");
    return -1;
  }
  ImageLoader* image = reinterpret_cast<ImageLoader*>(__handle);
  ImageLoader::deleteImage(image);
  return 0;
}


} // namespace isolator
