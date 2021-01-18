#include "KeyEventHandler.hpp"

#include "KeyEvent.hpp"

#include <hardware/Hardware.hpp>

#include <thirdp/wrpkey/key.hxx>

#include <Logger.hpp>

#include <sstream>
#include <iomanip>

using namespace mpc::controls;
using namespace mpc::hardware;
using namespace std;

KeyEventHandler::KeyEventHandler(weak_ptr<Hardware> hardware)
    : hardware (hardware)
{
}

void KeyEventHandler::handle(const KeyEvent& keyEvent)
{
    stringstream rawKeyCodeHex;
    rawKeyCodeHex << std::hex << keyEvent.rawKeyCode;
    auto rawKeyCodeHexString = string(rawKeyCodeHex.str());
    MLOG("KeyEventHandler::handle keyEvent with rawKeyCode " + to_string(keyEvent.rawKeyCode) + ", " + rawKeyCodeHexString);
    
    auto* helper = &WonderRabbitProject::key::key_helper_t::instance();
//    auto name = helper->name(keyEvent.rawKeyCode);
//    MLOG("WRP key name: " + name);
}
