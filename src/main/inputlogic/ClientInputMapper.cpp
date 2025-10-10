#include "inputlogic/ClientInputMapper.h"

#include "inputlogic/ClientInputAction.h"

using namespace mpc::inputlogic;

void ClientInputMapper::bind(const std::string& actionId, Callback callback)
{
    bindings[actionId] = std::move(callback);
}

void ClientInputMapper::trigger(const ClientInputAction& action) const
{
    if (auto it = bindings.find(action.id); it != bindings.end())
    {
        it->second(action);
    }
}

