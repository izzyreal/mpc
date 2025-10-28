#pragma once
#include "Component.hpp"
#include "Background.hpp"
#include "FunctionKeys.hpp"

#include <string>

namespace mpc::lcdgui
{

    class Layer : public Component
    {

    private:
        std::string focus;

    public:
        mpc::lcdgui::Background *getBackground();
        mpc::lcdgui::FunctionKeys *getFunctionKeys();
        bool setFocus(std::string textFieldName);
        std::string getFocus();

    public:
        Layer(int index);
    };
} // namespace mpc::lcdgui
