#include "PushLeftCommand.hpp"
#include "Mpc.hpp"

namespace mpc::command
{

    PushLeftCommand::PushLeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushLeftCommand::execute()
    {
        if (!mpc.getLayeredScreen()->getFocusedField() ||
            mpc.getLayeredScreen()->getFocusedFieldName() == "dummy")
        {
            return;
        }

        mpc.getLayeredScreen()->transferLeft();
    }

} // namespace mpc::command
