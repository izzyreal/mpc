#import <Foundation/Foundation.h>
#include <string>

// ExampleFile.mm
#import <Foundation/Foundation.h>
#include <string>

std::string getBundleResourcesPath() {
    NSString *path = [[NSBundle mainBundle] resourcePath];
    if (path) {
        return [path UTF8String];
    } else {
        return "";
    }
}

