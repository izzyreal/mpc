#pragma once

#include <memory>

namespace mpc
{
  class Mpc;
}

namespace mpc::controls
{
  class KbMapping;
}

namespace mpc::controls
{
    class Controls
    {
    public:
        Controls(mpc::Mpc &mpc);

        bool isStepRecording();
        
        std::weak_ptr<KbMapping> getKbMapping();

    private:
        std::shared_ptr<KbMapping> kbMapping;
        mpc::Mpc& mpc;
    };
}
