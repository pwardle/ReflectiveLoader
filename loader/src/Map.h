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
 * This file contains Map alias to std::unordered_map
 */

#ifndef __MAP__
#define __MAP__

#include <unordered_map>

// Is not used in this header. Just to provide missing structs and functions
// previously available in dyld2.h
#include "dyld_stubs.h"

// Is not used in this header. Just for compatibility with original sources
// where Map provide declaration of Array class.
#include "Array.h"

namespace isolator {
namespace dyld3 {

template<typename T, typename HS>
struct HashAdopter : private HS {
    size_t operator()( const T& a) const { return HS::hash(a); }
};

template<typename T, typename EQ>
struct EqualAdopter : private EQ {
    bool operator()( const T& a, const T& b ) const { return EQ::equal(a, b); }
};

template<typename KeyT, typename ValueT, class GetHash, class IsEqual>
using Map = std::unordered_map<KeyT, ValueT, HashAdopter<KeyT, GetHash>, EqualAdopter<KeyT, IsEqual>>;

}
}

#endif // __MAP__
