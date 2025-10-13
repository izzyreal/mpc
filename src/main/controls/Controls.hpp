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

        void setRecPressed(bool);
        void setRecLocked(bool);
        void setOverdubPressed(bool);
        void setOverdubLocked(bool);
        void setNoteRepeatLocked(bool);
        
        bool isRecPressed(bool includeLocked = true);
        bool isOverdubPressed(bool includeLocked = true);
        bool isNoteRepeatLocked();
        bool isStepRecording();
        bool isRecMainWithoutPlaying();
        
        std::weak_ptr<KbMapping> getKbMapping();

    private:
        std::shared_ptr<KbMapping> kbMapping;
        bool recPressed = false;
        bool recLocked = false;
        bool overDubPressed = false;
        bool overDubLocked = false;
        bool noteRepeatLocked = false;
        mpc::Mpc& mpc;
    };
}
