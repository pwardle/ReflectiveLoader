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

#ifndef __IMAGELOADER_PROXY__
#define __IMAGELOADER_PROXY__

#include "ImageLoaderMachO.h"

namespace isolator {

/**
 * Stub implementations of all virtual methods of ImageLoader interface.
 * Major part of methods will throw exception in case of call. Some of
 * them will return valid default values like false, zero or do nothing.
 *
 * Can be use as a base class for other inherited implementations to avoid
 * stubs reimplementation.
 */
class ImageLoaderUnimplementedStub : public ImageLoader {
    using ImageLoader::ImageLoader;
private:
#define UNIMPLEMENTED_METHOD { dyld::throwf("ImageLoaderProxy: unimplemented method"); }
#define STUB_METHOD(_val) { return _val; }
#define STUB_NOTHING { }

    /** Unimplemented method section. They should be called never */
    virtual	void			initializeCoalIterator(CoalIterator&, unsigned int loadOrder, unsigned imageIndex) UNIMPLEMENTED_METHOD;
    virtual	bool			incrementCoalIterator(CoalIterator&) UNIMPLEMENTED_METHOD;
    virtual	uintptr_t		getAddressCoalIterator(CoalIterator&, const LinkContext& context) UNIMPLEMENTED_METHOD;
    virtual	void			updateUsesCoalIterator(CoalIterator&, uintptr_t newAddr, ImageLoader* target, unsigned targetIndex, const LinkContext& context) UNIMPLEMENTED_METHOD;

    virtual const char*			getInstallPath() const UNIMPLEMENTED_METHOD;

    virtual bool			containsSymbol(const void* addr) const UNIMPLEMENTED_METHOD;
    virtual void*			getEntryFromLC_MAIN() const UNIMPLEMENTED_METHOD;
    virtual void*			getEntryFromLC_UNIXTHREAD() const UNIMPLEMENTED_METHOD;
    virtual const struct mach_header*   machHeader() const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			getSlide() const UNIMPLEMENTED_METHOD;
    virtual const void*			getEnd() const UNIMPLEMENTED_METHOD;
    virtual bool			hasCoalescedExports() const UNIMPLEMENTED_METHOD;
    virtual const Symbol*		findExportedSymbol(const char* name, bool searchReExports, const char* thisPath, const ImageLoader** foundIn) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t 			getExportedSymbolAddress(const ImageLoader::Symbol* sym, const ImageLoader::LinkContext& context,
                                               const ImageLoader* requestor, bool runResolver, const char* symbolName) const UNIMPLEMENTED_METHOD;

    virtual DefinitionFlags		getExportedSymbolInfo(const Symbol* sym) const UNIMPLEMENTED_METHOD;
    virtual const char*			getExportedSymbolName(const Symbol* sym) const UNIMPLEMENTED_METHOD;
    virtual uint32_t			getExportedSymbolCount() const UNIMPLEMENTED_METHOD;
    virtual const Symbol*		getIndexedExportedSymbol(uint32_t index) const UNIMPLEMENTED_METHOD;

    virtual uint32_t			getImportedSymbolCount() const UNIMPLEMENTED_METHOD;
    virtual const Symbol*		getIndexedImportedSymbol(uint32_t index) const UNIMPLEMENTED_METHOD;
    virtual ReferenceFlags		getImportedSymbolInfo(const Symbol* sym) const UNIMPLEMENTED_METHOD;
    virtual const char*			getImportedSymbolName(const Symbol* sym) const UNIMPLEMENTED_METHOD;
    virtual const char*			findClosestSymbol(const void* addr, const void** closestAddr) const UNIMPLEMENTED_METHOD;
    virtual bool			isBundle() const UNIMPLEMENTED_METHOD;
    virtual bool			isDylib() const UNIMPLEMENTED_METHOD;
    virtual bool			isExecutable() const UNIMPLEMENTED_METHOD;
    virtual bool			isPositionIndependentExecutable() const UNIMPLEMENTED_METHOD;
    virtual bool			forceFlat() const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			doBindLazySymbol(uintptr_t* lazyPointer, const LinkContext& context) UNIMPLEMENTED_METHOD;
    virtual uintptr_t			doBindFastLazySymbol(uint32_t lazyBindingInfoOffset, const LinkContext& context, void (*lock)(), void (*unlock)()) UNIMPLEMENTED_METHOD;
    virtual void			doTermination(const LinkContext& context) UNIMPLEMENTED_METHOD;
    virtual bool			needsInitialization() UNIMPLEMENTED_METHOD;
    virtual bool			getSectionContent(const char* segmentName, const char* sectionName, void** start, size_t* length) UNIMPLEMENTED_METHOD;
#if !UNSIGN_TOLERANT
    virtual void			getUnwindInfo(dyld_unwind_sections* info) = 0;
#endif
    virtual const struct macho_section* findSection(const void* imageInterior) const UNIMPLEMENTED_METHOD;
    virtual bool			findSection(const void* imageInterior, const char** segmentName, const char** sectionName, size_t* sectionOffset) UNIMPLEMENTED_METHOD;
    virtual bool			isPrebindable() const UNIMPLEMENTED_METHOD;
    virtual bool			usablePrebinding(const LinkContext& context) const UNIMPLEMENTED_METHOD;
    virtual void			getRPaths(const LinkContext& context, std::vector<const char*>&) const STUB_NOTHING;
    virtual bool			participatesInCoalescing() const UNIMPLEMENTED_METHOD;
    virtual bool			getUUID(uuid_t) const UNIMPLEMENTED_METHOD;
    virtual void			dynamicInterpose(const LinkContext& context) UNIMPLEMENTED_METHOD;

    virtual const char*			libPath(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual unsigned int		segmentCount() const UNIMPLEMENTED_METHOD;
    virtual const char*			segName(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segSize(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segFileSize(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segHasTrailingZeroFill(unsigned int) UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segFileOffset(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segReadable(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segWriteable(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segExecutable(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segUnaccessible(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			segHasPreferredLoadAddress(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segPreferredLoadAddress(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segActualLoadAddress(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uintptr_t			segActualEndAddress(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual uint32_t			sdkVersion() const UNIMPLEMENTED_METHOD;
    virtual uint32_t			minOSVersion() const UNIMPLEMENTED_METHOD;
    virtual void			registerInterposing(const LinkContext& context) UNIMPLEMENTED_METHOD;

    virtual ImageLoader*		libImage(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			libReExported(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual bool			libIsUpward(unsigned int) const UNIMPLEMENTED_METHOD;
    virtual void			setLibImage(unsigned int, ImageLoader*, bool, bool) UNIMPLEMENTED_METHOD;
    virtual void			doGetDependentLibraries(DependentLibraryInfo libs[]) STUB_NOTHING;
    virtual LibraryInfo			doGetLibraryInfo(const LibraryInfo& requestorInfo) { return {}; };
    virtual void			doRebase(const LinkContext& context) STUB_NOTHING;
    virtual void			doBind(const LinkContext& context, bool forceLazysBound, const ImageLoader* reExportParent) STUB_NOTHING;
    virtual void			doBindJustLazies(const LinkContext& context) UNIMPLEMENTED_METHOD;
    virtual void			doGetDOFSections(const LinkContext& context, std::vector<DOFInfo>& dofs) UNIMPLEMENTED_METHOD;
    virtual void			doInterpose(const LinkContext& context) UNIMPLEMENTED_METHOD;
    virtual bool			doInitialization(const LinkContext& context) STUB_METHOD(false);
    virtual bool			needsTermination() STUB_METHOD(false);
    virtual bool			segmentsMustSlideTogether() const UNIMPLEMENTED_METHOD;
    virtual bool			segmentsCanSlide() const UNIMPLEMENTED_METHOD;
    virtual void			setSlide(intptr_t slide) UNIMPLEMENTED_METHOD;
    virtual bool			isSubframeworkOf(const LinkContext& context, const ImageLoader* image) const STUB_METHOD(false);
    virtual bool			hasSubLibrary(const LinkContext& context, const ImageLoader* child) const  STUB_METHOD(false);
#undef UNIMPLEMENTED_METHOD
};

/**
 * Proxy Image Loader is a wrapper on top of regular system dlopen/dlsym
 * methods. It provides a minimal interface to use inside recursive load
 * of shared modules.
 */
class ImageLoaderProxy : public ImageLoaderUnimplementedStub {
public:
    /**
     * Construct ImageLoaderProxy based on modulePath.
     * Will look up symbols in this module and dependencies.
     *
     * Note: limited(minimal) support of rpath specification.
     */
    static ImageLoaderProxy* instantiate(const char* modulePath);

    /**
     * Construct ImageLoaderProxy based on RTLD_DEFAULT.
     * Will look up symbols in global space
     */
    static ImageLoaderProxy* instantiateDefault();

    virtual ~ImageLoaderProxy();

    virtual const Symbol* findExportedSymbol(const char* name, bool searchReExports, const char* thisPath, const ImageLoader** foundIn) const;

    virtual uintptr_t getExportedSymbolAddress(const ImageLoader::Symbol* sym, const ImageLoader::LinkContext& context,
        const ImageLoader* requestor, bool runResolver, const char* symbolName) const;

private:
    ImageLoaderProxy(const char* moduleName);
    ImageLoaderProxy();
    void* hdl = 0;
};

} // namespace isolator

#endif // __IMAGELOADER_PROXY__
