#pragma once

#include <functional>
#include <unordered_map>
#include <string>

namespace mpc::inputlogic {
    struct ClientInputAction;
    class ClientInputMapper {
    public:
        using Callback = std::function<void(const ClientInputAction&)>;

        void bind(const std::string& actionId, Callback callback);

        void trigger(const ClientInputAction& action) const;

    private:
        std::unordered_map<std::string, Callback> bindings;
    };
} // namespace mpc::inputlogic
