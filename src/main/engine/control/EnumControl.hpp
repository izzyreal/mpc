#pragma once

#include <engine/control/Control.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <variant>

namespace mpc::engine::control {
    class EnumControl : public Control
    {
    private:
        std::variant<int, std::string> value;

    public:
        void setValue(const std::variant<int, std::string>& value);

        std::string getValueString() override;
        EnumControl(int id, std::string name, const std::variant<int, std::string>& value);

    };

}
