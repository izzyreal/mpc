#include <ServiceManagement/ServiceManagement.h>

extern "C" bool InstallHelperTool() {
    bool success = false;

    @autoreleasepool {
        CFErrorRef error = NULL;
        // "com.example.HelperTool" is the helper's bundle identifier.
        success = SMJobBless(kSMDomainSystemLaunchd, CFSTR("com.example.HelperTool"), NULL, &error);

        if (!success) {
            CFStringRef errorDescription = CFErrorCopyDescription(error);
            if (errorDescription) {
                // Convert the CFStringRef to a C string.
                char errorStr[1024];  // Adjust the buffer size if needed.
                if (CFStringGetCString(errorDescription, errorStr, sizeof(errorStr), kCFStringEncodingUTF8)) {
                    printf("Error: %s\n", errorStr);
                } else {
                    printf("Error occurred, but couldn't convert error string.\n");
                }
                CFRelease(errorDescription);
            } else {
                printf("Error occurred, but couldn't retrieve error string.\n");
            }
            if (error) {
                CFRelease(error);
            }
        }
    }

    return success;
}