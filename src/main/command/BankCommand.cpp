#include "BankCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    BankCommand::BankCommand(mpc::Mpc &mpc, int i) : mpc(mpc), i(i) {}

    void BankCommand::execute() {
        mpc.setBank(i);
    }

}
