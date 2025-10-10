#include "ClientInputController.h"

#include "inputlogic/ClientInputAction.h"
#include "controller/PadContextFactory.h"
#include "controls/GlobalReleaseControls.hpp"
#include "controls/BaseControls.hpp"
#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"

using namespace mpc::controller;
using namespace mpc::inputlogic; 

ClientInputController::ClientInputController(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
}

void ClientInputController::handleAction(const ClientInputAction& action)
{
    if (startsWith(action.id, "pad-") && endsWith(action.id, "-press")) return handlePadPress(action);
    if (startsWith(action.id, "pad-") && endsWith(action.id, "-aftertouch")) return handlePadAftertouch(action);
    if (startsWith(action.id, "pad-") && endsWith(action.id, "-release")) return handlePadRelease(action);
    if (startsWith(action.id, "datawheel-")) return handleDataWheel(action);
    if (action.id == "slider-move") return handleSlider(action);
    if (action.id == "pot-move") return handlePot(action);
    if (endsWith(action.id, "-press")) return handleButtonPress(action);
    if (endsWith(action.id, "-release")) return handleButtonRelease(action);
}

bool ClientInputController::startsWith(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
}

bool ClientInputController::endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

void ClientInputController::handlePadPress(const ClientInputAction& a)
{
    assert(a.value.has_value());
    const auto num = std::stoi(a.id.substr(4, a.id.find("-press") - 4));
    //std::printf("[logic] pad %d pressed with velocity %d\n", num, *a.value);
    auto padIndexWithBank = num + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPadPushContext(mpc, padIndexWithBank, *a.value, mpc.getLayeredScreen()->getCurrentScreenName());
    mpc::controls::BaseControls::pad(ctx, padIndexWithBank, *a.value);
}

void ClientInputController::handlePadRelease(const ClientInputAction& a)
{
    assert(!a.value.has_value());
    const auto num = std::stoi(a.id.substr(4, a.id.find("-release") - 4));
    //std::printf("[logic] pad %d released\n", num);
    auto padIndexWithBank = num + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, padIndexWithBank, mpc.getLayeredScreen()->getCurrentScreenName());
    mpc::controls::GlobalReleaseControls::simplePad(ctx);
}

void ClientInputController::handlePadAftertouch(const ClientInputAction& a)
{
    const auto num = std::stoi(a.id.substr(4, a.id.find("-aftertouch") - 4));
    std::printf("[logic] pad %d aftertouch pressure %d\n", num, a.value.value_or(0));
}

void ClientInputController::handleDataWheel(const ClientInputAction& a)
{
    assert(a.value.has_value());
    const char* dir = a.id == "datawheel-up" ? "up" : "down";
    std::printf("[logic] data wheel turned %s (%d)\n", dir, *a.value);
    mpc.getActiveControls()->turnWheel(*a.value);
}

void ClientInputController::handleSlider(const ClientInputAction& a)
{
    std::printf("[logic] slider moved to %d\n", a.value.value_or(0));
}

void ClientInputController::handlePot(const ClientInputAction& a)
{
    std::printf("[logic] pot moved to %d\n", a.value.value_or(0));
}

void ClientInputController::handleButtonPress(const ClientInputAction& a)
{
    std::printf("[logic] button %s pressed\n", a.id.c_str());
}

void ClientInputController::handleButtonRelease(const ClientInputAction& a)
{
    std::printf("[logic] button %s released\n", a.id.c_str());
}

