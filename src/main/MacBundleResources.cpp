#ifdef __APPLE__
#include <TargetConditionals.h>
#if not TARGET_OS_IPHONE

#include "MacBundleResources.h"
#include <CoreFoundation/CoreFoundation.h>

#include <iostream>

using namespace mpc;

std::string MacBundleResources::getResourcePath(const std::string& resourceName) {
    CFStringRef bundleIdentifier = CFStringCreateWithCString(kCFAllocatorDefault, "nl.izmar.vmpc2000xl", kCFStringEncodingUTF8);
    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(bundleIdentifier);
    CFRelease(bundleIdentifier);

    CFStringRef resourceNameCF = CFStringCreateWithCString(nullptr, resourceName.c_str(), kCFStringEncodingUTF8);
    if (!resourceNameCF) {
        std::cerr << "Failed to create CFString." << std::endl;
        return "";
    }

    CFURLRef resourceURL = CFBundleCopyResourceURL(bundle, resourceNameCF, nullptr, nullptr);
    CFRelease(resourceNameCF);

    if (!resourceURL) {
        std::cerr << "Failed to get resource URL." << std::endl;
        return "";
    }

    CFStringRef resourcePathCF = CFURLCopyFileSystemPath(resourceURL, kCFURLPOSIXPathStyle);
    CFRelease(resourceURL);

    if (!resourcePathCF) {
        std::cerr << "Failed to get resource path." << std::endl;
        return "";
    }

    const char* resourcePathCStr = CFStringGetCStringPtr(resourcePathCF, kCFStringEncodingUTF8);
    std::string resourcePath(resourcePathCStr);

    CFRelease(resourcePathCF);

    return resourcePath;
}

#endif
#endif
