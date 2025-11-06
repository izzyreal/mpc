#pragma once

#include "lcdgui/ScreenRegistry.hpp"
#include "lcdgui/screens/Fwd.hpp"

namespace mpc::lcdgui
{
    enum class ScreenId
    {
#define X(ns, Class, name) Class,
        SCREEN_LIST
#undef X
            Count
    };
    template <ScreenId ID> struct ScreenTypeMap;

#define X(ns, Class, name)                                                     \
    template <> struct ScreenTypeMap<ScreenId::Class>                          \
    {                                                                          \
        using type = ns::Class;                                                \
    };
    SCREEN_LIST
#undef X
} // namespace mpc::lcdgui
