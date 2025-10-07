#pragma once

#include <functional>
#include <unordered_map>
#include <string>

namespace mpc::inputlogic {
    class InputAction;
    class InputMapper {
    public:
        using Callback = std::function<void(const InputAction&)>;

        void bind(const std::string& actionId, Callback callback);

        void trigger(const InputAction& action) const;

    private:
        std::unordered_map<std::string, Callback> bindings;
    };
} // namespace mpc::inputlogic
