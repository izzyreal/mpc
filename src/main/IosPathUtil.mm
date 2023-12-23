#import <Foundation/Foundation.h>
#include <string>

static NSString *const myGroupId = @"group.nl.izmar.vmpc2000xl";

std::string getIosSharedDocumentsFolder() {
    NSFileManager *fileManager = [NSFileManager defaultManager];

    NSURL *sharedDocumentsURL = [[fileManager containerURLForSecurityApplicationGroupIdentifier:myGroupId] URLByAppendingPathComponent:@"Documents"];

    if (![fileManager fileExistsAtPath:[sharedDocumentsURL path]]) {
        NSError *error = nil;
        [fileManager createDirectoryAtURL:sharedDocumentsURL withIntermediateDirectories:YES attributes:nil error:&error];

        if (error) {
            NSLog(@"Error creating directory: %@", error);
            return "";
        }
    }

    NSString *urlString = [sharedDocumentsURL absoluteString];

    NSString *fileScheme = @"file://";
    if ([urlString hasPrefix:fileScheme]) {
        urlString = [urlString substringFromIndex:[fileScheme length]];
    }

    return std::string([urlString UTF8String]);
}
