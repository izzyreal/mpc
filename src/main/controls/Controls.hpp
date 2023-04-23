#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>
#include "sequencer/NoteEvent.hpp"
#include "sequencer/NoteEventStore.hpp"

namespace mpc
{
  class Mpc;
}

namespace mpc::controls
{
  class KbMapping;
  class KeyEventHandler;
  class BaseControls;
  class GlobalReleaseControls;
}
namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::controls
{
    class Controls : public mpc::sequencer::NoteEventStore<int> 
    {
    public:
        Controls(mpc::Mpc &mpc);

        void setErasePressed(bool);
        void setRecPressed(bool);
        void setOverDubPressed(bool);
        void setPlayPressed(bool);
        void setTapPressed(bool);
        void setGoToPressed(bool);
        void setShiftPressed(bool);
        void setF3Pressed(bool);
        void setF4Pressed(bool);
        void setF5Pressed(bool);
        void setF6Pressed(bool);
        void setNoteRepeatLocked(bool);
        
        void pressPad(int);
        void unpressPad(int);
        void clearAllPadStates();

        bool isErasePressed();
        bool isRecPressed();
        bool isOverDubPressed();
        bool isPlayPressed();
        bool isTapPressed();
        bool isNoteRepeatLocked();
        bool isGoToPressed();
        bool isShiftPressed();
        bool isF3Pressed();
        bool isF4Pressed();
        bool isF5Pressed();
        bool isF6Pressed();
        bool isStepRecording();
        bool isRecMainWithoutPlaying();
        
        bool isPadPressed(int);
        bool arePadsPressed();

        std::weak_ptr<KeyEventHandler> getKeyEventHandler();
        std::shared_ptr<BaseControls> getBaseControls();
        std::shared_ptr<GlobalReleaseControls> getReleaseControls();

        void setCtrlPressed(bool);
        void setAltPressed(bool);
        bool isCtrlPressed();
        bool isAltPressed();
        std::weak_ptr<KbMapping> getKbMapping();

    private:
        std::shared_ptr<KbMapping> kbMapping;
        bool ctrlPressed = false;
        bool altPressed = false;
        bool shiftPressed = false;
        bool recPressed = false;
        bool overDubPressed = false;
        bool playPressed = false;
        bool tapPressed = false;
        bool noteRepeatLocked = false;
        bool goToPressed = false;
        bool erasePressed = false;
        bool f3Pressed = false;
        bool f4Pressed = false;
        bool f5Pressed = false;
        bool f6Pressed = false;
        mpc::Mpc& mpc;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<KeyEventHandler> keyEventHandler;
        std::shared_ptr<BaseControls> baseControls;
        std::shared_ptr<GlobalReleaseControls> releaseControls;
        std::unordered_map<int/*index*/, int/*count*/> pressedPads;
    };
}
