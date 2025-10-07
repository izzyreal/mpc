#include "inputlogic/InputMapper.h"

#include "inputlogic/InputAction.h"

using namespace mpc::inputlogic;

void InputMapper::bind(const std::string& actionId, Callback callback)
{
    bindings[actionId] = std::move(callback);
}

void InputMapper::trigger(const InputAction& action) const
{
    if (auto it = bindings.find(action.id); it != bindings.end())
    {
        it->second(action);
    }
}

