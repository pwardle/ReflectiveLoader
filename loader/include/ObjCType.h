// Pulled from here
// https://github.com/mull-project/llvm-jit-objc/tree/master/llvm-jit-objc/src/include
// as POC from article at https://stanislaw.github.io/2018-09-03-llvm-jit-objc-and-swift-knowledge-dump.html

#pragma once

#include <objc/runtime.h>

#include <string>
#include <cassert>

#ifndef _HERE_OBJC_RUNTIME_CONFIG_H_
#define _HERE_OBJC_RUNTIME_CONFIG_H_

// Define SUPPORT_GC_COMPAT=1 to enable compatibility where GC once was.
// OBJC_NO_GC and OBJC_NO_GC_API in objc-api.h mean something else.
#if !TARGET_OS_OSX
#define SUPPORT_GC_COMPAT 0
#else
#define SUPPORT_GC_COMPAT 1
#endif

// Define SUPPORT_ZONES=1 to enable malloc zone support in NXHashTable.
#if !TARGET_OS_OSX
#define SUPPORT_ZONES 0
#else
#define SUPPORT_ZONES 1
#endif

// Define SUPPORT_MOD=1 to use the mod operator in NXHashTable and objc-sel-set
#if defined(__arm__)
#define SUPPORT_MOD 0
#else
#define SUPPORT_MOD 1
#endif

// Define SUPPORT_PREOPT=1 to enable dyld shared cache optimizations
#if TARGET_OS_WIN32 || TARGET_OS_SIMULATOR
#define SUPPORT_PREOPT 0
#else
#define SUPPORT_PREOPT 1
#endif

// Define SUPPORT_TAGGED_POINTERS=1 to enable tagged pointer objects
// Be sure to edit tagged pointer SPI in objc-internal.h as well.
#if !(__OBJC2__ && __LP64__)
#define SUPPORT_TAGGED_POINTERS 0
#else
#define SUPPORT_TAGGED_POINTERS 1
#endif

// Define SUPPORT_MSB_TAGGED_POINTERS to use the MSB
// as the tagged pointer marker instead of the LSB.
// Be sure to edit tagged pointer SPI in objc-internal.h as well.
#if !SUPPORT_TAGGED_POINTERS || !TARGET_OS_IPHONE
#define SUPPORT_MSB_TAGGED_POINTERS 0
#else
#define SUPPORT_MSB_TAGGED_POINTERS 1
#endif

// Define SUPPORT_INDEXED_ISA=1 on platforms that store the class in the isa
// field as an index into a class table.
// Note, keep this in sync with any .s files which also define it.
// Be sure to edit objc-abi.h as well.
#if __ARM_ARCH_7K__ >= 2
#define SUPPORT_INDEXED_ISA 1
#else
#define SUPPORT_INDEXED_ISA 0
#endif

// Define SUPPORT_PACKED_ISA=1 on platforms that store the class in the isa
// field as a maskable pointer with other data around it.
#if (!__LP64__ || TARGET_OS_WIN32 || TARGET_OS_SIMULATOR)
#define SUPPORT_PACKED_ISA 0
#else
#define SUPPORT_PACKED_ISA 1
#endif

// Define SUPPORT_NONPOINTER_ISA=1 on any platform that may store something
// in the isa field that is not a raw pointer.
#if !SUPPORT_INDEXED_ISA && !SUPPORT_PACKED_ISA
#define SUPPORT_NONPOINTER_ISA 0
#else
#define SUPPORT_NONPOINTER_ISA 1
#endif

// Define SUPPORT_FIXUP=1 to repair calls sites for fixup dispatch.
// Fixup messaging itself is no longer supported.
// Be sure to edit objc-abi.h as well (objc_msgSend*_fixup)
#if !(defined(__x86_64__) && (TARGET_OS_OSX || TARGET_OS_SIMULATOR))
#define SUPPORT_FIXUP 0
#else
#define SUPPORT_FIXUP 1
#endif

// Define SUPPORT_ZEROCOST_EXCEPTIONS to use "zero-cost" exceptions for OBJC2.
// Be sure to edit objc-exception.h as well (objc_add/removeExceptionHandler)
#if !__OBJC2__ || (defined(__arm__) && __USING_SJLJ_EXCEPTIONS__)
#define SUPPORT_ZEROCOST_EXCEPTIONS 0
#else
#define SUPPORT_ZEROCOST_EXCEPTIONS 1
#endif

// Define SUPPORT_ALT_HANDLERS if you're using zero-cost exceptions
// but also need to support AppKit's alt-handler scheme
// Be sure to edit objc-exception.h as well (objc_add/removeExceptionHandler)
#if !SUPPORT_ZEROCOST_EXCEPTIONS || TARGET_OS_IPHONE || TARGET_OS_EMBEDDED
#define SUPPORT_ALT_HANDLERS 0
#else
#define SUPPORT_ALT_HANDLERS 1
#endif

// Define SUPPORT_RETURN_AUTORELEASE to optimize autoreleased return values
#if TARGET_OS_WIN32
#define SUPPORT_RETURN_AUTORELEASE 0
#else
#define SUPPORT_RETURN_AUTORELEASE 1
#endif

// Define SUPPORT_STRET on architectures that need separate struct-return ABI.
#if defined(__arm64__)
#define SUPPORT_STRET 0
#else
#define SUPPORT_STRET 1
#endif

// Define SUPPORT_MESSAGE_LOGGING to enable NSObjCMessageLoggingEnabled
#if TARGET_OS_WIN32 || TARGET_OS_EMBEDDED
#define SUPPORT_MESSAGE_LOGGING 0
#else
#define SUPPORT_MESSAGE_LOGGING 1
#endif

// Define SUPPORT_QOS_HACK to work around deadlocks due to QoS bugs.
#if !__OBJC2__ || TARGET_OS_WIN32
#define SUPPORT_QOS_HACK 0
#else
#define SUPPORT_QOS_HACK 1
#endif

// OBJC_INSTRUMENTED controls whether message dispatching is dynamically
// monitored.  Monitoring introduces substantial overhead.
// NOTE: To define this condition, do so in the build command, NOT by
// uncommenting the line here.  This is because objc-class.h heeds this
// condition, but objc-class.h can not #include this file (objc-config.h)
// because objc-class.h is public and objc-config.h is not.
// #define OBJC_INSTRUMENTED

#endif

static inline void *
memdup(const void *mem, size_t len)
{
  void *dup = malloc(len);
  memcpy(dup, mem, len);
  return dup;
}

namespace mull
{
  namespace objc
  {

    struct method64_t
    {
      SEL name;          /* SEL (64-bit pointer) */
      const char *types; /* const char * (64-bit pointer) */
      uint64_t imp;      /* IMP (64-bit pointer) */

      struct SortBySELAddress : public std::binary_function<const method64_t &,
                                                            const method64_t &, bool>
      {
        bool operator()(const method64_t &lhs,
                        const method64_t &rhs)
        {
          return lhs.name < rhs.name;
        }
      };

      std::string getDebugDescription(int level = 0) const;
    };

    struct method_list64_t
    {
      uint32_t entsize;
      uint32_t count;
      struct method64_t first; // These structures follow inline

      const method64_t *getFirstMethodPointer() const
      {
        assert((&this->entsize + 2) == (void *)(&this->first));
        return (const method64_t *)(&this->first);
      }

      std::string getDebugDescription(int level = 0) const;
    };

    struct ivar64_t
    {
      void *offset;     /* uintptr_t * (64-bit pointer) */
      const char *name; /* const char * (64-bit pointer) */
      const char *type; /* const char * (64-bit pointer) */
      uint32_t alignment;
      uint32_t size;
    };

    struct ivar_list64_t
    {
      uint32_t entsize;
      uint32_t count;
      struct ivar64_t first;
    };

    struct objc_property64
    {
      const char *name;       /* const char * (64-bit pointer) */
      const char *attributes; /* const char * (64-bit pointer) */
    };

    struct objc_property_list64
    {
      uint32_t entsize;
      uint32_t count;
      struct objc_property64 first;
    };

    struct class_ro64_t
    {
      uint32_t flags;
      uint32_t instanceStart;
      uint32_t instanceSize;
      uint32_t reserved;
      uint64_t ivarLayout;                               // const uint8_t * (64-bit pointer)
      const char *name;                                  // const char * (64-bit pointer)
      method_list64_t *baseMethods;                      // const method_list_t * (64-bit pointer)
      uint64_t baseProtocols;                            // const protocol_list_t * (64-bit pointer)
      const ivar_list64_t *ivars;                        // const ivar_list_t * (64-bit pointer)
      uint64_t weakIvarLayout;                           // const uint8_t * (64-bit pointer)
      const struct objc_property_list64 *baseProperties; // const struct objc_property_list (64-bit pointer)

      bool isMetaClass() const
      {
        return (flags & 0x1) == 1;
      }

      const char *getName() const
      {
        return (const char *)name;
      }

      method_list64_t *getMethodListPtr() const
      {
        return baseMethods;
      }

      std::string getDebugDescription(int level = 0) const;
    };

    enum Description
    {
      Clazz = 0,
      IsaOrSuperclass = 1
    };

    struct class64_t
    {
      class64_t *isa;        // class64_t * (64-bit pointer)
      class64_t *superclass; // class64_t * (64-bit pointer)
      uint64_t cache;        // Cache (64-bit pointer)
      uint64_t vtable;       // IMP * (64-bit pointer)
      class_ro64_t *data;    // class_ro64_t * (64-bit pointer)

      class64_t *getIsaPointer() const
      {
        return (class64_t *)isa;
      }
      class64_t *getSuperclassPointer() const
      {
        return (class64_t *)superclass;
      }
      class_ro64_t *getDataPointer() const
      {
#define FAST_DATA_MASK 0x00007ffffffffff8UL

        return (class_ro64_t *)((uintptr_t)data & FAST_DATA_MASK);
        //    return (class_ro64_t *)data;
      }

      std::string getDebugDescription(Description = Clazz) const;
      bool isFastDataMask() const
      {
        // #define FAST_DATA_MASK          0x00007ffffffffff8ULL

        return (uintptr_t)data & FAST_DATA_MASK;
      }

      bool isSwift() const
      {
#define FAST_IS_SWIFT (1UL << 0)

        return (uintptr_t)data & FAST_IS_SWIFT;
      }
    };

  }
} // namespace mull { namespace objc {

template <typename Element, typename List, uint32_t FlagMask>
struct entsize_list_tt
{
  uint32_t entsizeAndFlags;
  uint32_t count;
  Element first;

  uint32_t entsize() const
  {
    return entsizeAndFlags & ~FlagMask;
  }
  uint32_t flags() const
  {
    return entsizeAndFlags & FlagMask;
  }

  Element &getOrEnd(uint32_t i) const
  {
    assert(i <= count);
    return *(Element *)((uint8_t *)&first + i * entsize());
  }
  Element &get(uint32_t i) const
  {
    assert(i < count);
    return getOrEnd(i);
  }

  size_t byteSize() const
  {
    return sizeof(*this) + (count - 1) * entsize();
  }

  List *duplicate() const
  {
    return (List *)memdup(this, this->byteSize());
  }

  struct iterator;
  const iterator begin() const
  {
    return iterator(*static_cast<const List *>(this), 0);
  }
  iterator begin()
  {
    return iterator(*static_cast<const List *>(this), 0);
  }
  const iterator end() const
  {
    return iterator(*static_cast<const List *>(this), count);
  }
  iterator end()
  {
    return iterator(*static_cast<const List *>(this), count);
  }

  struct iterator
  {
    uint32_t entsize;
    uint32_t index; // keeping track of this saves a divide in operator-
    Element *element;

    typedef std::random_access_iterator_tag iterator_category;
    typedef Element value_type;
    typedef ptrdiff_t difference_type;
    typedef Element *pointer;
    typedef Element &reference;

    iterator() {}

    iterator(const List &list, uint32_t start = 0)
        : entsize(list.entsize()), index(start), element(&list.getOrEnd(start))
    {
    }

    const iterator &operator+=(ptrdiff_t delta)
    {
      element = (Element *)((uint8_t *)element + delta * entsize);
      index += (int32_t)delta;
      return *this;
    }
    const iterator &operator-=(ptrdiff_t delta)
    {
      element = (Element *)((uint8_t *)element - delta * entsize);
      index -= (int32_t)delta;
      return *this;
    }
    const iterator operator+(ptrdiff_t delta) const
    {
      return iterator(*this) += delta;
    }
    const iterator operator-(ptrdiff_t delta) const
    {
      return iterator(*this) -= delta;
    }

    iterator &operator++()
    {
      *this += 1;
      return *this;
    }
    iterator &operator--()
    {
      *this -= 1;
      return *this;
    }
    iterator operator++(int)
    {
      iterator result(*this);
      *this += 1;
      return result;
    }
    iterator operator--(int)
    {
      iterator result(*this);
      *this -= 1;
      return result;
    }

    ptrdiff_t operator-(const iterator &rhs) const
    {
      return (ptrdiff_t)this->index - (ptrdiff_t)rhs.index;
    }

    Element &operator*() const { return *element; }
    Element *operator->() const { return element; }

    operator Element &() const { return *element; }

    bool operator==(const iterator &rhs) const
    {
      return this->element == rhs.element;
    }
    bool operator!=(const iterator &rhs) const
    {
      return this->element != rhs.element;
    }

    bool operator<(const iterator &rhs) const
    {
      return this->element < rhs.element;
    }
    bool operator>(const iterator &rhs) const
    {
      return this->element > rhs.element;
    }
  };
};

// Two bits of entsize are used for fixup markers.
struct method_list_t : entsize_list_tt<mull::objc::method64_t, method_list_t, 0x3>
{
  bool isFixedUp() const;
  void setFixedUp()
  {
    static uint32_t fixed_up_method_list = 3;

    //    assert(!isFixedUp());
    entsizeAndFlags = entsize() | fixed_up_method_list;
  }

  uint32_t indexOfMethod(const mull::objc::method64_t *meth) const
  {
    uint32_t i =
        (uint32_t)(((uintptr_t)meth - (uintptr_t)this) / entsize());
    assert(i < count);
    return i;
  }
};

/***********************************************************************
 * list_array_tt<Element, List>
 * Generic implementation for metadata that can be augmented by categories.
 *
 * Element is the underlying metadata type (e.g. method_t)
 * List is the metadata's list type (e.g. method_list_t)
 *
 * A list_array_tt has one of three values:
 * - empty
 * - a pointer to a single list
 * - an array of pointers to lists
 *
 * countLists/beginLists/endLists iterate the metadata lists
 * count/begin/end iterate the underlying metadata elements
 **********************************************************************/
template <typename Element, typename List>
class list_array_tt
{
  struct array_t
  {
    uint32_t count;
    List *lists[0];

    static size_t byteSize(uint32_t count)
    {
      return sizeof(array_t) + count * sizeof(lists[0]);
    }
    size_t byteSize()
    {
      return byteSize(count);
    }
  };

protected:
  class iterator
  {
    List **lists;
    List **listsEnd;
    typename List::iterator m, mEnd;

  public:
    iterator(List **begin, List **end)
        : lists(begin), listsEnd(end)
    {
      if (begin != end)
      {
        m = (*begin)->begin();
        mEnd = (*begin)->end();
      }
    }

    const Element &operator*() const
    {
      return *m;
    }
    Element &operator*()
    {
      return *m;
    }

    bool operator!=(const iterator &rhs) const
    {
      if (lists != rhs.lists)
        return true;
      if (lists == listsEnd)
        return false; // m is undefined
      if (m != rhs.m)
        return true;
      return false;
    }

    const iterator &operator++()
    {
      assert(m != mEnd);
      m++;
      if (m == mEnd)
      {
        assert(lists != listsEnd);
        lists++;
        if (lists != listsEnd)
        {
          m = (*lists)->begin();
          mEnd = (*lists)->end();
        }
      }
      return *this;
    }
  };

private:
  union
  {
    List *list;
    uintptr_t arrayAndFlag;
  };

  bool hasArray() const
  {
    return arrayAndFlag & 1;
  }

  array_t *array()
  {
    return (array_t *)(arrayAndFlag & ~1);
  }

  void setArray(array_t *array)
  {
    arrayAndFlag = (uintptr_t)array | 1;
  }

public:
  uint32_t count()
  {
    uint32_t result = 0;
    for (auto lists = beginLists(), end = endLists();
         lists != end;
         ++lists)
    {
      result += (*lists)->count;
    }
    return result;
  }

  iterator begin()
  {
    return iterator(beginLists(), endLists());
  }

  iterator end()
  {
    List **e = endLists();
    return iterator(e, e);
  }

  uint32_t countLists()
  {
    if (hasArray())
    {
      return array()->count;
    }
    else if (list)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }

  List **beginLists()
  {
    if (hasArray())
    {
      return array()->lists;
    }
    else
    {
      return &list;
    }
  }

  List **endLists()
  {
    if (hasArray())
    {
      return array()->lists + array()->count;
    }
    else if (list)
    {
      return &list + 1;
    }
    else
    {
      return &list;
    }
  }

  void attachLists(List *const *addedLists, uint32_t addedCount)
  {
    if (addedCount == 0)
      return;

    if (hasArray())
    {
      // many lists -> many lists
      uint32_t oldCount = array()->count;
      uint32_t newCount = oldCount + addedCount;
      setArray((array_t *)realloc(array(), array_t::byteSize(newCount)));
      array()->count = newCount;
      memmove(array()->lists + addedCount, array()->lists,
              oldCount * sizeof(array()->lists[0]));
      memcpy(array()->lists, addedLists,
             addedCount * sizeof(array()->lists[0]));
    }
    else if (!list && addedCount == 1)
    {
      // 0 lists -> 1 list
      list = addedLists[0];
    }
    else
    {
      // 1 list -> many lists
      List *oldList = list;
      uint32_t oldCount = oldList ? 1 : 0;
      uint32_t newCount = oldCount + addedCount;
      setArray((array_t *)malloc(array_t::byteSize(newCount)));
      array()->count = newCount;
      if (oldList)
        array()->lists[addedCount] = oldList;
      memcpy(array()->lists, addedLists,
             addedCount * sizeof(array()->lists[0]));
    }
  }

  void tryFree()
  {
    if (hasArray())
    {
      for (uint32_t i = 0; i < array()->count; i++)
      {
        try_free(array()->lists[i]);
      }
      try_free(array());
    }
    else if (list)
    {
      try_free(list);
    }
  }

  template <typename Result>
  Result duplicate()
  {
    Result result;

    if (hasArray())
    {
      array_t *a = array();
      result.setArray((array_t *)memdup(a, a->byteSize()));
      for (uint32_t i = 0; i < a->count; i++)
      {
        result.array()->lists[i] = a->lists[i]->duplicate();
      }
    }
    else if (list)
    {
      result.list = list->duplicate();
    }
    else
    {
      result.list = nil;
    }

    return result;
  }
};

class method_array_t : public list_array_tt<mull::objc::method64_t, method_list_t>
{
  typedef list_array_tt<mull::objc::method64_t, method_list_t> Super;

public:
  method_list_t **beginCategoryMethodLists()
  {
    return beginLists();
  }

  method_list_t **endCategoryMethodLists(Class cls);

  method_array_t duplicate()
  {
    return Super::duplicate<method_array_t>();
  }
};

//  struct method_array_t {
//    uint32_t count;
//    method64_t **lists;
//  };

struct property_t
{
  const char *name;
  const char *attributes;
};

struct property_list_t : entsize_list_tt<property_t, property_list_t, 0>
{
};

class property_array_t : public list_array_tt<property_t, property_list_t>
{
  typedef list_array_tt<property_t, property_list_t> Super;

public:
  property_array_t duplicate()
  {
    return Super::duplicate<property_array_t>();
  }
};
typedef uintptr_t protocol_ref_t; // protocol_t *, but unremapped

struct protocol_list_t
{
  // count is 64-bit by accident.
  uintptr_t count;
  protocol_ref_t list[0]; // variable-size

  size_t byteSize() const
  {
    return sizeof(*this) + count * sizeof(list[0]);
  }

  protocol_list_t *duplicate() const
  {
    return (protocol_list_t *)memdup(this, this->byteSize());
  }

  typedef protocol_ref_t *iterator;
  typedef const protocol_ref_t *const_iterator;

  const_iterator begin() const
  {
    return list;
  }
  iterator begin()
  {
    return list;
  }
  const_iterator end() const
  {
    return list + count;
  }
  iterator end()
  {
    return list + count;
  }
};

class protocol_array_t : public list_array_tt<protocol_ref_t, protocol_list_t>
{
  typedef list_array_tt<protocol_ref_t, protocol_list_t> Super;

public:
  protocol_array_t duplicate()
  {
    return Super::duplicate<protocol_array_t>();
  }
};

struct here_class_rw_t
{
  // Be warned that Symbolication knows the layout of this structure.
  uint32_t flags;
  uint32_t version;

  mull::objc::class_ro64_t *ro;

  method_array_t methods;
  property_array_t properties;
  protocol_array_t protocols;

  Class firstSubclass;
  Class nextSiblingClass;

  char *demangledName;

  // #if SUPPORT_INDEXED_ISA
  //     uint32_t index;
  // #endif
};
struct class_data_bits_t
{

  // Values are the FAST_ flags above.
  uintptr_t bits;

public:
  here_class_rw_t *data()
  {
    return (here_class_rw_t *)(bits & FAST_DATA_MASK);
  }

  void setData(here_class_rw_t *newData)
  {
    //      assert(!data()  ||  (newData->flags & (RW_REALIZING | RW_FUTURE)));
    // Set during realization or construction only. No locking needed.
    // Use a store-release fence because there may be concurrent
    // readers of data and data's contents.
    uintptr_t newBits = (bits & ~FAST_DATA_MASK) | (uintptr_t)newData;
    bits = newBits;
  }

  bool isSwift()
  {
    return getBit(FAST_IS_SWIFT);
  }
  bool setSwift()
  {
    setBits(FAST_IS_SWIFT);
    return true;
  }

  bool getBit(uintptr_t bit)
  {
    return bits & bit;
  }

  void setBits(uintptr_t set)
  {
    bits = bits | set;
  }
};

#define RO_META (1 << 0)
#define RW_REALIZED (1 << 31)

struct cache_t
{
  uintptr_t __unused_buckets;
  uint32_t _mask;
  uint32_t _occupied;
};

union isa_t
{
  isa_t() {}
  isa_t(uintptr_t value) : bits(value) {}

  Class cls;
  uintptr_t bits;
};

struct here_objc_object
{
  isa_t isa;

public:
  Class ISA()
  {
#define ISA_MASK 0x00007ffffffffff8ULL

    //    assert(!isTaggedPointer());
#if SUPPORT_INDEXED_ISA
    if (ISA.nonpointer)
    {
      uintptr_t slot = isa.indexcls;
      return classForIndex((unsigned)slot);
    }
    return (Class)isa.bits;
#else
    return (Class)(((uintptr_t)isa.bits) & ISA_MASK);
#endif
  }
};

struct here_objc_class : public here_objc_object
{
  Class superclass;
  cache_t cache;
  class_data_bits_t bits; // class_rw_t * plus custom rr/alloc flags

  void setInstanceSize(uint32_t newSize)
  {
    assert(isRealized());
    if (newSize != data()->ro->instanceSize)
    {

#define RW_COPIED_RO (1 << 27)

      assert(data()->flags & RW_COPIED_RO);
      *const_cast<uint32_t *>(&data()->ro->instanceSize) = newSize;
    }
    //    bits.setFastInstanceSize(newSize);
  }

  here_class_rw_t *data()
  {
    return bits.data();
  }

  void setData(here_class_rw_t *newData)
  {
    bits.setData(newData);
  }

  bool isSwift()
  {
    return bits.isSwift();
  }
  void setSwift()
  {
    bits.setSwift();
  }
  bool isRealized()
  {
    return data()->flags & RW_REALIZED;
  }
  bool isMetaClass()
  {
    assert(this);
    assert(isRealized());
    return data()->ro->flags & RO_META;
  }
};

OBJC_EXPORT const uintptr_t objc_debug_isa_class_mask;

struct here_swift_class_t : here_objc_class
{
  uint32_t flags;
  uint32_t instanceAddressOffset;
  uint32_t instanceSize;
  uint16_t instanceAlignMask;
  uint16_t reserved;

  uint32_t classSize;
  uint32_t classAddressOffset;
  void *description;
  // ...

  void *baseAddress()
  {
    return (void *)((uint8_t *)this - classAddressOffset);
  }
};

struct category_t
{
  const char *name;
  mull::objc::class64_t *cls;
  struct method_list_t *instanceMethods;
  struct method_list_t *classMethods;
  struct protocol_list_t *protocols;
  struct property_list_t *instanceProperties;
  // Fields below this point are not always present on disk.
  struct property_list_t *_classProperties;
};
