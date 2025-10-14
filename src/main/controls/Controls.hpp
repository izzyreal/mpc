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

        std::weak_ptr<KbMapping> getKbMapping();

    private:
        std::shared_ptr<KbMapping> kbMapping;
    };
}
