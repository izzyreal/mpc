#include "MidiOutputMonitorScreen.hpp"

#include "Mpc.hpp"
#include "audiomidi/EventHandler.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;

MidiOutputMonitorScreen::MidiOutputMonitorScreen(mpc::Mpc &mpc,
                                                 const int layerIndex)
    : ScreenComponent(mpc, "midi-output-monitor", layerIndex)
{
}

void MidiOutputMonitorScreen::open()
{
    mpc.getEventHandler()->addObserver(this);
}

void MidiOutputMonitorScreen::close()
{
    mpc.getEventHandler()->deleteObserver(this);

    if (blinkThread.joinable())
    {
        blinkThread.join();
    }
}

void MidiOutputMonitorScreen::static_blink(
    void *arg1, const std::weak_ptr<mpc::lcdgui::Label> &label)
{
    static_cast<MidiOutputMonitorScreen *>(arg1)->runBlinkThread(label);
}

void MidiOutputMonitorScreen::runBlinkThread(
    const std::weak_ptr<mpc::lcdgui::Label> &label)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    label.lock()->setText(" ");
}

void MidiOutputMonitorScreen::initTimer(std::weak_ptr<mpc::lcdgui::Label> label)
{
    if (blinkThread.joinable())
    {
        blinkThread.join();
    }

    blinkThread =
        std::thread(&MidiOutputMonitorScreen::static_blink, this, label);
}

void MidiOutputMonitorScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);

    int deviceNumber = stoi(msg.substr(1));

    if (msg[0] == 'b')
    {
        deviceNumber += 16;
    }

    const auto label = findLabel(std::to_string(deviceNumber));
    label->setText(u8"\u00CC");
    initTimer(label);
}

MidiOutputMonitorScreen::~MidiOutputMonitorScreen()
{
    if (blinkThread.joinable())
    {
        blinkThread.join();
    }
}
