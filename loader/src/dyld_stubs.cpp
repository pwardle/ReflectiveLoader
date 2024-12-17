/* -*- mode: C++; c-basic-offset: 4; tab-width: 4 -*-
 *
 * Copyright (c) 2021 Apple Inc. All rights reserved.
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

/*
 * This file contains declaration of global g_linkContext which will be used
 * for macho_dlopen/macho_dlsym routines. It specify behaviour of loaders
 * and inject custom ImageLoaderProxy to solve external dependencies.
 */

#include "dyld_stubs.h"

#include "ImageLoader.h"
#include "ImageLoaderProxy.h"

#include <mach/mach_init.h>
#include <mach/vm_map.h>
#include <mach/vm_types.h>
#include <sys/mman.h>

namespace isolator {

namespace dyld {

    struct dyld_all_image_infos gProcessInfo_t { true };
    struct dyld_all_image_infos *gProcessInfo = &gProcessInfo_t;

    struct LibSystemHelpers gLibSystemHelpers_t;
    const struct LibSystemHelpers *gLibSystemHelpers = &gLibSystemHelpers_t;

    constexpr int BUFF_SIZE = 2048;

    void throwf(const char* format, ...) {
        char *buf = reinterpret_cast<char*>(malloc(BUFF_SIZE*sizeof(char)));
        va_list list;
        va_start(list, format);
        vsprintf(buf, format, list);
        va_end(list);
        throw static_cast<const char*>(buf);
    }

    void log(const char* format, ...) {
        va_list	list;
        va_start(list, format);
        vprintf(format, list);
        va_end(list);
    }

    void warn(const char* format, ...) {
        va_list	list;
        va_start(list, format);
        printf(format, list);
        va_end(list);
    }

    const char* mkstringf(const char* format, ...) {
        char *buf = reinterpret_cast<char*>(malloc(BUFF_SIZE*sizeof(char)));
        va_list	list;
        va_start(list, format);
        vsprintf(buf, format, list);
        va_end(list);
        return buf;
    }

    bool isTranslated() {
        return false;
    }
}

/** mmap/alloc functions. Suffix "__" added to avoid intersection with original implementations form dyld */
extern "C" int vm_alloc__(vm_address_t* addr, vm_size_t size, uint32_t flags) {
    return ::vm_allocate(mach_task_self(), addr, size, flags);
}

extern "C" void* xmmap__(void* addr, size_t len, int prot, int flags, int fd, off_t offset) {
    return ::mmap(addr, len, prot, flags, fd, offset);
}

/** Stub to do nothing. Will be never called */
extern "C" int _dyld_func_lookup__(const char* name, void** address) {
    *address = 0;
    return false;
}

/** Stub to do nothing. only for x86 */
void addAotImagesToAllAotImages(uint32_t aotInfoCount, const dyld_aot_image_info aotInfo[]) {};

/** Stubs to do nothing. To isolate form system link context */
void stub_notifySingle(dyld_image_states, const ImageLoader* image, ImageLoader::InitializerTimingList*) {}

void stub_notifyBatch(dyld_image_states state, bool preflightOnly) {}

void stub_setErrorStrings(unsigned errorCode, const char* errorClientOfDylibPath,
      const char* errorTargetDylibPath, const char* errorSymbol) {}

void stub_clearAllDepths() {}

unsigned int stub_imageCount() { return 0; };

void stub_addDynamicReference(ImageLoader* from, ImageLoader* to) {
    // ImageLoaderProxy doesn't require holding.
    if (dynamic_cast<ImageLoaderProxy*>(to) == nullptr)
        dyld::throwf("addDynamicReference: unsupported case");
}

bool stub_flatExportFinder(const char* name, const ImageLoader::Symbol** sym, const ImageLoader** image) {
    ImageLoader* globImage = ImageLoaderProxy::instantiateDefault();

    *sym = globImage->findExportedSymbol(name, true, image);
    return sym != nullptr;
}

ImageLoader* stub_loadLibrary(const char* libraryName, bool search, const char* origin,
        const ImageLoader::RPathChain* rpaths, unsigned& cacheIndex) {
    return ImageLoaderProxy::instantiate(libraryName);
}

unsigned int stub_getCoalescedImages(ImageLoader* images[], unsigned imageIndex[]) {
    return 0;
}

static ImageLoader::LinkContext make_default_link_context() {
    ImageLoader::LinkContext ctx = {}; // zeroing

    // Global notification symbols. Do nothing
    ctx.notifyBatch = stub_notifyBatch;
    ctx.notifySingle = stub_notifySingle;
    ctx.setErrorStrings = stub_setErrorStrings;

    ctx.clearAllDepths = stub_clearAllDepths;
    ctx.imageCount = stub_imageCount;

    // Enforce flat bind
    ctx.bindFlat = true; //
    ctx.prebindUsage = ImageLoader::kUseNoPrebinding;

    // Integrate ImageLoaderProxy image into recurrent
    // initialization of dependencies.
    ctx.addDynamicReference = stub_addDynamicReference;
    ctx.flatExportFinder = stub_flatExportFinder;
    ctx.getCoalescedImages = stub_getCoalescedImages;
    ctx.loadLibrary = stub_loadLibrary;

    return ctx;
}

// Global linker contexts to use from macho_dlopen/macho_dlsym
ImageLoader::LinkContext g_linkContext = make_default_link_context();

}
