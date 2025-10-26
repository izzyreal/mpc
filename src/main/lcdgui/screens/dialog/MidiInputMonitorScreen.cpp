#include "MidiInputMonitorScreen.hpp"

#include <Mpc.hpp>
#include "audiomidi/EventHandler.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;

MidiInputMonitorScreen::MidiInputMonitorScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "midi-input-monitor", layerIndex)
{
}

void MidiInputMonitorScreen::open()
{
    mpc.clientEventController->getClientMidiEventController()->addObserver(this);
}

void MidiInputMonitorScreen::close()
{
    mpc.clientEventController->getClientMidiEventController()->deleteObserver(this);

    if (blinkThread.joinable())
    {
        blinkThread.join();
    }
}

void MidiInputMonitorScreen::static_blink(void *arg1, std::weak_ptr<mpc::lcdgui::Label> label)
{
    static_cast<MidiInputMonitorScreen *>(arg1)->runBlinkThread(label);
}

void MidiInputMonitorScreen::runBlinkThread(std::weak_ptr<mpc::lcdgui::Label> label)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    label.lock()->setText(" ");
}

void MidiInputMonitorScreen::initTimer(std::weak_ptr<mpc::lcdgui::Label> label)
{
    if (blinkThread.joinable())
    {
        blinkThread.join();
    }

    blinkThread = std::thread(&MidiInputMonitorScreen::static_blink, this, label);
}

void MidiInputMonitorScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);

    int deviceNumber = stoi(msg.substr(1));

    if (msg[0] == 'b')
    {
        deviceNumber += 16;
    }

    auto label = findLabel(std::to_string(deviceNumber));
    label->setText(u8"\u00CC");
    initTimer(label);
}

MidiInputMonitorScreen::~MidiInputMonitorScreen()
{
    if (blinkThread.joinable())
    {
        blinkThread.join();
    }
}
