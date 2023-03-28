#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include "sequencer/NoteEvent.hpp"

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

namespace mpc::controls
{
  class Controls
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

    std::weak_ptr<KeyEventHandler> getKeyEventHandler();
    std::shared_ptr<BaseControls> getControls();
    std::shared_ptr<GlobalReleaseControls> getReleaseControls();

    void setCtrlPressed(bool);
    void setAltPressed(bool);
    bool isCtrlPressed();
    bool isAltPressed();
    std::weak_ptr<KbMapping> getKbMapping();

    bool storePlayNoteEvent(int padIndexWithBank, std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly> event);
    std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly> retrievePlayNoteEvent(int padIndexWithBank);

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
    std::shared_ptr<KeyEventHandler> keyEventHandler;
    std::shared_ptr<BaseControls> controls;
    std::shared_ptr<GlobalReleaseControls> releaseControls;
    std::unordered_map<int, std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly>> playNoteStore = {};

  };
}
