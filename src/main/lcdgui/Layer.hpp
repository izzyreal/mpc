#pragma once
#include "Component.hpp"
#include "Background.hpp"

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
        std::string getFocus();
    };
} // namespace mpc::lcdgui
