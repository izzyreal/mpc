#if __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS

#include "UITextInputModeBridge.hpp"

#import <UIKit/UIKit.h>

const std::string getAutomaticHardwareLayout()
{
    const UITextInputMode *mode = UITextInputMode.activeInputModes.firstObject;

    if (mode)
    {
        const auto candidate = [mode valueForKey:@"automaticHardwareLayout"];
        
        if (candidate)
        {
            return std::string([candidate UTF8String]);
        }
    }

    return "";
}

#endif // TARGET_OS_IOS
#endif // __APPLE__
