#include "PushUpCommand.hpp"
#include "Mpc.hpp"

namespace mpc::command
{

    PushUpCommand::PushUpCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushUpCommand::execute()
    {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocusedFieldName() == "dummy")
        {
            return;
        }

        mpc.getLayeredScreen()->transferUp();
    }

} // namespace mpc::command
