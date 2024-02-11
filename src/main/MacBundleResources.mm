#ifdef __APPLE__

#include "MacBundleResources.h"
#import <Foundation/Foundation.h>

using namespace mpc;

std::string MacBundleResources::getResourcePath(const std::string& resourceName) {

    NSString *resourceNameNSString = [NSString stringWithUTF8String:resourceName.c_str()];
    
    NSArray<NSString *> *components = [resourceNameNSString componentsSeparatedByString:@"."];

    NSString *name = @"";
    NSString *extension = @"";

    if (components.count > 1) {
        name = components[0];
        extension = components.lastObject;
    }
    
    NSBundle* bundle = [NSBundle mainBundle];
    
    if ([bundle.bundlePath hasSuffix:@".appex"]) {
        name = [@"../../" stringByAppendingString:name];
    }

    NSURL *fileURL = [bundle URLForResource:name withExtension:extension];
                      
    if (!fileURL) {
        [NSException raise:@"File not found exception" format:@"%@ file not found.", resourceNameNSString];
    }
    
    NSString *filePath = [fileURL path];
    
    if (!filePath) {
        [NSException raise:@"Path conversion exception" format:@"Could not convert URL to POSIX path for %@.", resourceNameNSString];
    }
    
    return std::string([filePath UTF8String]);
}

#endif
