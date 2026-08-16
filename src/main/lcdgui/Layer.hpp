#pragma once
#include "Component.hpp"
#include "Background.hpp"
#include "LcdInteraction.hpp"

#include <optional>
#include <string>

namespace mpc::lcdgui
{

    class Layer : public Component
    {
        std::string focus;

    public:
        explicit Layer(int index);

        Background *getBackground();
        bool setFocus(const std::string &textFieldName);
        std::string getFocus() const;
        std::optional<LcdHitTarget>
        findLcdTargetAt(LcdPoint point, const LcdHitTestOptions &options) const;
        std::shared_ptr<Field>
        findFocusableFieldAt(LcdPoint point,
                             const LcdHitTestOptions &options) const;
    };
} // namespace mpc::lcdgui
