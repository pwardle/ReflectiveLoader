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
 * This file contains ScopedTimer stub implementation which will do nothing
 */

#ifndef __TRACING__
#define __TRACING__

#define DYLD_EXIT_REASON_DYLIB_MISSING          1
#define DYLD_EXIT_REASON_DYLIB_WRONG_ARCH       2
#define DYLD_EXIT_REASON_DYLIB_WRONG_VERSION    3
#define DYLD_EXIT_REASON_SYMBOL_MISSING         4
#define DYLD_EXIT_REASON_CODE_SIGNATURE         5
#define DYLD_EXIT_REASON_FILE_SYSTEM_SANDBOX    6
#define DYLD_EXIT_REASON_MALFORMED_MACHO        7

#define DBG_DYLD_TIMING_STATIC_INITIALIZER      1
#define DBG_DYLD_TIMING_APPLY_FIXUPS            2
#define DBG_DYLD_TIMING_ATTACH_CODESIGNATURE    3
#define DBG_DYLD_TIMING_APPLY_INTERPOSING       4

namespace isolator {
namespace dyld3 {

/**
 * @brief Empty stub implementation. Just to meet compilation requirements. Do nothing.
 */
class ScopedTimer {
public:
    ScopedTimer(uint32_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {}
};

}
}

#endif // __TRACING__