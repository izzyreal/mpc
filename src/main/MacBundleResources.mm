#ifdef __APPLE__

#include "MacBundleResources.h"
#import <Foundation/Foundation.h>
#include <TargetConditionals.h>

using namespace mpc;

std::string MacBundleResources::getResourcePath(const std::string& resourceName) {
    
    NSString *resourceNameNSString = [NSString stringWithUTF8String:resourceName.c_str()];

    NSBundle* bundle = [NSBundle mainBundle];

    if (![bundle.bundlePath containsString:@"VMPC2000XL"] && ![bundle.bundlePath containsString:@"mpc-tests"]) {
        bundle = [NSBundle bundleWithIdentifier:@"nl.izmar.vmpc2000xl"];
    }

    NSURL* appBundleURL = [bundle bundleURL];

    if ([bundle.bundlePath hasSuffix:@".app"] && ![bundle.bundlePath containsString:@"mpc-tests"]) {

#if TARGET_OS_OSX
        appBundleURL = [appBundleURL URLByAppendingPathComponent:@"Contents/PlugIns/VMPC2000XL.appex/"];
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

#endif
