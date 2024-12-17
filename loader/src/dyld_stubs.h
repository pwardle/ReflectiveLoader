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
 * This file contains dyld specific structure declaration, util functions
 * and global objects.
 */


#ifndef __DYLD_STUBS__
#define __DYLD_STUBS__

#include <sys/stat.h>

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <stdexcept>

namespace isolator {

struct dyld_interpose_tuple {
    const void* replacement;
    const void* replacee;
};

namespace dyld {
    // Means that libSystem is already initialized
    struct dyld_all_image_infos {
        bool libSystemInitialized;
    };
    extern struct dyld_all_image_infos*	gProcessInfo;

    struct LibSystemHelpers {};
    extern const struct LibSystemHelpers* gLibSystemHelpers;

    // Stub, always false. Only for x86
    bool isTranslated();
}

namespace dyld3 {
  // Alias on system stat. Only for x86
  const auto stat = ::stat;
}

}

#endif  // __DYLD_STUBS__
