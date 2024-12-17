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

#include "ImageLoaderProxy.h"
#include <dlfcn.h>
#include <string>

namespace isolator {

ImageLoaderProxy* ImageLoaderProxy::instantiate(const char* modulePath) {
    std::string moduleName_str = modulePath;

    // we cannot handle rpath properly. So there are two cases to support:
    //  * Load by absolute path, rare case for iOS
    //  * Load by file name. Assume that moduleName format is "@rpath/libXXX.dylib".
    //    Have to just remove rpath prefix.
    std::string prefix = "@rpath/";
    if (!moduleName_str.compare(0, prefix.size(), prefix))
        moduleName_str.erase(0, prefix.size());

    return new ImageLoaderProxy(moduleName_str.c_str());
}

ImageLoaderProxy* ImageLoaderProxy::instantiateDefault() {
    static ImageLoaderProxy _loader = ImageLoaderProxy();
    return &_loader;
}

ImageLoaderProxy::ImageLoaderProxy(): ImageLoaderUnimplementedStub("", 0), hdl(RTLD_DEFAULT) {}

ImageLoaderProxy::ImageLoaderProxy(const char* modulePath): ImageLoaderUnimplementedStub(modulePath, 0) {
    hdl = dlopen(modulePath, RTLD_NOW);
    if (!hdl)
        dyld::throwf("ImageLoaderProxy: cannot load image %s \n\n reason: %s", modulePath, dlerror());
}

ImageLoaderProxy::~ImageLoaderProxy() {
    if (hdl)
        dlclose(hdl);
}

const ImageLoader::Symbol* ImageLoaderProxy::findExportedSymbol(const char* name, bool searchReExports,
        const char* thisPath, const ImageLoader** foundIn) const {
    // TODO: dyld_stub_binder is special function from libdyld.dylib which
    //  doesn't available via regular dlopen/dlsym interface cause it looks
    //  only for underscored export symbols.
    //  ===
    //  So will return any func pointer(like "instantiate") and hope it will
    //  be called never. Because it requires only for week binding, but we
    //  enforced immediate flat binding.
    if (std::string(name) == "dyld_stub_binder") {
        *foundIn = this;
        return reinterpret_cast<const ImageLoader::Symbol*>(instantiate);
    }

    if (name[0] == '_') // remove leading underscore
        name ++;

    auto sym = reinterpret_cast<const ImageLoader::Symbol*>(dlsym(hdl, name));

    if (sym) {
        *foundIn = this;
        return sym;
    } else {
        *foundIn = nullptr;
        return nullptr;
    }
}

uintptr_t ImageLoaderProxy::getExportedSymbolAddress(const ImageLoader::Symbol* sym, const ImageLoader::LinkContext& context,
        const ImageLoader* requestor, bool runResolver, const char* symbolName) const {
    return reinterpret_cast<uintptr_t>(sym);
}

}