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
    class EventRegistry;
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

    class ClientEventController
        : public std::enable_shared_from_this<ClientEventController>,
          public mpc::Observable
    {

    public:
        ClientEventController(mpc::Mpc &);

        void init();

        void dispatchHostInput(const mpc::input::HostInputEvent &hostEvent);

        void handleClientEvent(const client::event::ClientEvent &);

        std::shared_ptr<mpc::input::KeyboardBindings> getKeyboardBindings()
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
        void setActiveBank(const Bank);
        Bank getActiveBank() const;
        void setSelectedNote(int note);
        void setSelectedPad(int padIndexWithBank);
        int getSelectedNote() const;
        int getSelectedPad() const;

        bool isRecMainWithoutPlaying() const;

    private:
        mpc::Mpc &mpc;
        std::shared_ptr<ClientMidiEventController> clientMidiEventController;
        std::shared_ptr<mpc::input::KeyboardBindings> keyboardBindings;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;

        bool afterEnabled = false;
        bool fullLevelEnabled = false;
        bool sixteenLevelsEnabled = false;

        Bank activeBank = Bank::A;
        int selectedPad = 0;
        int selectedNote = 60;
    };
} // namespace mpc::controller
