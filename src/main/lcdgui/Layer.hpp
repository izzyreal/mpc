#pragma once
#include "Component.hpp"
#include "Background.hpp"
#include "FunctionKeys.hpp"

#include <string>

namespace mpc::lcdgui
{

    class Layer : public Component
    {
        std::string focus;

    public:
        explicit Layer(int index);

        Background *getBackground();
        FunctionKeys *getFunctionKeys();
        bool setFocus(const std::string &textFieldName);
        std::string getFocus();
    };
} // namespace mpc::lcdgui
