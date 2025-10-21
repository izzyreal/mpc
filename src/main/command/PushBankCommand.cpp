#include "PushBankCommand.hpp"
#include "Mpc.hpp"

namespace mpc::command {

    PushBankCommand::PushBankCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i) {}

    void PushBankCommand::execute() {
        mpc.setBank(i);
    }

}
