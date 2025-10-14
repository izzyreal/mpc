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

        void setNoteRepeatLocked(bool);
        bool isNoteRepeatLocked();
        bool isStepRecording();
        bool isRecMainWithoutPlaying();
        
        std::weak_ptr<KbMapping> getKbMapping();

    private:
        std::shared_ptr<KbMapping> kbMapping;
        bool noteRepeatLocked = false;
        mpc::Mpc& mpc;
    };
}
