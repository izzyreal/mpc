#pragma once

#include "ClientInputControllerBase.h"

#include <string>

namespace mpc::inputlogic {
    struct ClientInputAction;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

class ClientInputController : public ClientInputControllerBase {
    public:
        explicit ClientInputController(mpc::Mpc&);
        void handleAction(const mpc::inputlogic::ClientInputAction& action) override;

    private:
        mpc::Mpc &mpc;
        static bool startsWith(const std::string& s, const std::string& prefix);
        static bool endsWith(const std::string& s, const std::string& suffix);

        void handlePadPress(const mpc::inputlogic::ClientInputAction& a);

        void handlePadAftertouch(const mpc::inputlogic::ClientInputAction& a);

        void handlePadRelease(const mpc::inputlogic::ClientInputAction& a);

        void handleDataWheel(const mpc::inputlogic::ClientInputAction& a);

        void handleSlider(const mpc::inputlogic::ClientInputAction& a);

        void handlePot(const mpc::inputlogic::ClientInputAction& a);

        void handleButtonPress(const mpc::inputlogic::ClientInputAction& a);

        void handleButtonRelease(const mpc::inputlogic::ClientInputAction& a);
    };
} // namespace mpc::controller
