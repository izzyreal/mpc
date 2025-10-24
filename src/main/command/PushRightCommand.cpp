#include "PushRightCommand.hpp"
#include "Mpc.hpp"

namespace mpc::command
{

    PushRightCommand::PushRightCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushRightCommand::execute()
    {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocusedFieldName() == "dummy")
        {
            return;
        }

        mpc.getLayeredScreen()->transferRight();
    }

} // namespace mpc::command
