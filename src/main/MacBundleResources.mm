#ifdef __APPLE__

#include "MacBundleResources.hpp"
#import <Foundation/Foundation.h>
#include <TargetConditionals.h>

#include <dlfcn.h>

using namespace mpc;

namespace
{
std::string getLoadedModulePath()
{
    Dl_info info;

    if (dladdr((void*)MacBundleResources::getResourcePath, &info) == 0 ||
        info.dli_fname == nullptr)
    {
        return {};
    }

    char* path_copy = realpath(info.dli_fname, nullptr);

    if (!path_copy)
    {
        return {};
    }

    const std::string result(path_copy);
    free(path_copy);
    return result;
}

bool fileExists(const std::string& path)
{
    if (path.empty())
    {
        return false;
    }

    NSString* filePath = [NSString stringWithUTF8String:path.c_str()];
    if (filePath == nil)
    {
        return false;
    }

    return [[NSFileManager defaultManager] fileExistsAtPath:filePath];
}

std::string getNonLv2ResourcePathFromLoadedModule(const std::string& resourceName)
{
    const auto modulePath = getLoadedModulePath();
    if (modulePath.empty())
    {
        return {};
    }

    const auto markerPos = modulePath.rfind("/Contents/MacOS/");
    if (markerPos == std::string::npos)
    {
        return {};
    }

    const auto bundleRoot = modulePath.substr(0, markerPos);
    return bundleRoot + "/Contents/Resources/" + resourceName;
}
} // namespace

std::string getNonLv2ResourcePath(const std::string& resourceName)
{
    NSString *resourceNameNSString = [NSString stringWithUTF8String:resourceName.c_str()];

    NSBundle* bundle = [NSBundle mainBundle];

    if (![bundle.bundlePath containsString:@"VMPC2000XL"] && ![bundle.bundlePath containsString:@"mpc-tests"]) {
        bundle = [NSBundle bundleWithIdentifier:@"nl.izmar.vmpc2000xl"];
    }

    NSURL* appBundleURL = [bundle bundleURL];

    if ([bundle.bundlePath hasSuffix:@".app"] && ![bundle.bundlePath containsString:@"mpc-tests"]) {

#if TARGET_OS_OSX
        NSURL *appexURL = [appBundleURL URLByAppendingPathComponent:@"Contents/PlugIns/VMPC2000XL.appex/"];

        if ([[NSFileManager defaultManager] fileExistsAtPath:[appexURL path]]) {
            appBundleURL = appexURL;
        }
#else
        appBundleURL = [appBundleURL URLByAppendingPathComponent:@"PlugIns/VMPC2000XL.appex/"];
#endif
    }

#if TARGET_OS_OSX
    NSString* filePath = [appBundleURL.path stringByAppendingString:@"/Contents/Resources/"];
#else
    NSString* filePath = [appBundleURL.path stringByAppendingString:@"/"];
#endif

    filePath = [filePath stringByAppendingString:resourceNameNSString];
    return {[filePath UTF8String]};
}

std::string getLv2ResourcePath(const std::string& resourceName)
{
    const auto modulePath = getLoadedModulePath();
    if (modulePath.empty() || modulePath.find(".lv2") == std::string::npos)
    {
        return {};
    }

    const auto lastSlash = modulePath.rfind('/');
    if (lastSlash == std::string::npos)
    {
        return {};
    }

    const auto moduleDir = modulePath.substr(0, lastSlash);
    return moduleDir + "/resources/" + resourceName;
}

std::string MacBundleResources::getResourcePath(const std::string& resourceName)
{
    auto lv2ResourcePath = getLv2ResourcePath(resourceName);

    if (!lv2ResourcePath.empty())
    {
        return lv2ResourcePath;
    }

    auto moduleDerivedPath = getNonLv2ResourcePathFromLoadedModule(resourceName);
    if (fileExists(moduleDerivedPath))
    {
        return moduleDerivedPath;
    }

    return getNonLv2ResourcePath(resourceName);
}

#endif
