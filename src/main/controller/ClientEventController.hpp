#pragma once

#include "client/event/ClientEvent.hpp"

#include "input/HostToClientTranslator.hpp"
#include "input/KeyboardBindings.hpp"

#include "sequencer/RecordingMode.hpp"

#include "controller/Bank.hpp"

#include "Observer.hpp"

#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::eventregistry
{
    class PerformanceManager;
}

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::controller
{
    class ClientHardwareEventController;
    class ClientMidiEventController;

    class ClientEventController final
        : public std::enable_shared_from_this<ClientEventController>,
          public Observable
    {

    public:
        explicit ClientEventController(Mpc &);

        void init();

        void dispatchHostInput(const input::HostInputEvent &hostEvent);

        void handleClientEvent(const client::event::ClientEvent &) const;

        std::shared_ptr<input::KeyboardBindings> getKeyboardBindings()
        {
            return keyboardBindings;
        }

        std::shared_ptr<ClientMidiEventController>
        getClientMidiEventController();

        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;

        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();

        sequencer::RecordingMode determineRecordingMode() const;

        bool isAfterEnabled() const;
        void setAfterEnabled(bool);
        bool isFullLevelEnabled() const;
        void setFullLevelEnabled(bool);
        bool isSixteenLevelsEnabled() const;
        void setSixteenLevelsEnabled(bool);
        void setActiveBank(Bank);
        Bank getActiveBank() const;
        void setSelectedNote(DrumNoteNumber);
        void setSelectedPad(ProgramPadIndex);
        DrumNoteNumber getSelectedNote() const;
        ProgramPadIndex getSelectedPad() const;

        bool isRecMainWithoutPlaying() const;
        bool isEraseButtonPressed() const;

    private:
        Mpc &mpc;
        input::HostToClientTranslator hostToClientTranslator;
        std::shared_ptr<ClientMidiEventController> clientMidiEventController;
        std::shared_ptr<input::KeyboardBindings> keyboardBindings;
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;

        bool afterEnabled = false;
        bool fullLevelEnabled = false;
        bool sixteenLevelsEnabled = false;

        Bank activeBank = Bank::A;
        ProgramPadIndex selectedPad{MinProgramPadIndex};
        DrumNoteNumber selectedNote{60};
    };
} // namespace mpc::controller
