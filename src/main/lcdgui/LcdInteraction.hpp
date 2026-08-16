#pragma once

#include "LcdGeometry.hpp"

#include <string>
#include <variant>

namespace mpc::lcdgui
{
    struct LcdHitTestOptions
    {
        float horizontalPadding = 0.f;
        float verticalPadding = 0.f;
        float minimumTargetWidth = 0.f;
        float minimumTargetHeight = 0.f;
    };

    struct LcdFieldHitTarget
    {
        std::string fieldName;
    };

    struct LcdFunctionKeyHitTarget
    {
        int functionKeyIndex;
    };

    using LcdHitTarget =
        std::variant<LcdFieldHitTarget, LcdFunctionKeyHitTarget>;

    enum class FieldFocusResult
    {
        NoTarget,
        AlreadyFocused,
        FocusChanged
    };
} // namespace mpc::lcdgui
