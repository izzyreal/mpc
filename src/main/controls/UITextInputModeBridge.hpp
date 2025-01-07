#pragma once

#if __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS

#include <string>

const std::string getAutomaticHardwareLayout();

#endif // TARGET_OS_IOS
#endif // __APPLE__
