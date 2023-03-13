#pragma once

#include <engine/control/Control.hpp>
#include <cstdint>
#include <string>

#include <engine/util/any.hpp>

namespace ctoot::control {
    class EnumControl : public Control
    {
    private:
        nonstd::any value;

    public:
        virtual void setValue(nonstd::any value);

        virtual nonstd::any getValue();

        virtual std::vector<nonstd::any> getValues() = 0;

    public:
        std::string getValueString() override;

        int getIntValue() override;

    public:
        EnumControl(int id, std::string name, nonstd::any value);

    };

}
