#ifdef __APPLE__

#include "MacBundleResources.hpp"
#import <Foundation/Foundation.h>
#include <TargetConditionals.h>

#include <dlfcn.h>

using namespace mpc;

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
    Dl_info info;

    if (dladdr((void*)MacBundleResources::getResourcePath, &info) == 0)
    {
        return {};
    }

    char* path_copy = realpath(info.dli_fname, NULL);
    if (!path_copy)
    {
        return "";
    }

    if (std::string(path_copy).find(".lv2") == std::string::npos)
    {
        free(path_copy);
        return "";
    }

    char* last_slash = strrchr(path_copy, '/');
    if (last_slash) *last_slash = '\0';

    char* resources_path = (char*)malloc(PATH_MAX);

    snprintf(resources_path, PATH_MAX, "%s/resources", path_copy);
    free(path_copy);

    const auto result = std::string(resources_path) + "/" + resourceName;
    free(resources_path);
    return result;
}

std::string MacBundleResources::getResourcePath(const std::string& resourceName)
{
    const auto lv2ResourcePath = getLv2ResourcePath(resourceName);

    if (!lv2ResourcePath.empty())
    {
        return lv2ResourcePath;
    }

    return getNonLv2ResourcePath(resourceName);
}

#endif
