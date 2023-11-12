#import <Foundation/Foundation.h>
#include <string>

std::string getMacMainBundleResourcesPath() {
    NSString *path = [[NSBundle mainBundle] resourcePath];
    if (path) {
        return [path UTF8String];
    } else {
        return "";
    }
}

