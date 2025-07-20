// Pulled from here
// https://github.com/mull-project/llvm-jit-objc/tree/master/llvm-jit-objc/src/include
// as POC from article at https://stanislaw.github.io/2018-09-03-llvm-jit-objc-and-swift-knowledge-dump.html

#pragma once

#include "ObjCType.h"
#include <objc/objc.h>

#include <queue>
#include <vector>
#include <set>

#pragma mark -

namespace mull
{
  namespace objc
  {

    class Runtime
    {
      std::queue<class64_t **> classesToRegister;

      std::vector<class64_t *> classRefs;
      std::vector<class64_t *> metaclassRefs;

      std::set<Class> runtimeClasses;
      std::vector<std::pair<class64_t **, Class>> oldAndNewClassesMap;

      Class registerOneClass(class64_t **classrefPtr, Class superclass);
      void parsePropertyAttributes(const char *const attributesStr,
                                   char *const stringStorage,
                                   objc_property_attribute_t *attributes,
                                   size_t *count);

    public:
      ~Runtime();

      void registerSelectors(void *selRefsSectionPtr,
                             uintptr_t selRefsSectionSize);

      void addClassesFromSection(void *sectionPtr,
                                 uintptr_t sectionSize);
      void addClassesFromClassRefsSection(void *sectionPtr,
                                          uintptr_t sectionSize);
      void addClassesFromSuperclassRefsSection(void *sectionPtr,
                                               uintptr_t sectionSize);
      void addCategoriesFromSection(void *sectionPtr,
                                    uintptr_t sectionSize);

      void registerClasses();
    };

  }
}
