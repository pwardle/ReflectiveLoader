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
 * This file contains Array alias to std::vector
 */

#ifndef __ARRAY__
#define __ARRAY__

#include <vector>

namespace isolator {
namespace dyld3 {

template<typename T>
class Array : public std::vector<T> {
public:
    uintptr_t count() const { return this->size(); }

    bool contains(const T& targ) const {
        auto found = std::find(this->begin(), this->end(), targ);
        return found != this->end();
    }
};

template<typename T>
using OverflowSafeArray = Array<T>;

#define STACK_ALLOC_ARRAY(_type, _name, _count) \
  dyld3::Array<_type> _name; \
  _name.reserve(_count);

}
}

#endif // __ARRAY__
