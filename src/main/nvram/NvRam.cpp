#include "NvRam.hpp"

#include <nvram/DefaultsParser.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>

#include <file/all/Defaults.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::nvram;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;
using namespace std;

void NvRam::loadUserScreenValues(mpc::Mpc& mpc)
{
    string path = mpc::Paths::configPath() + "nvram.vmp";
    auto file = File(path, nullptr);
    
    if (!file.exists())
        return;
    
    auto defaults = DefaultsParser::AllDefaultsFromFile(mpc, file);
    auto userScreen = mpc.screens->get<UserScreen>("user");
    
    userScreen->lastBar = defaults.getBarCount() - 1;
    userScreen->bus = defaults.getBusses()[0];
    
    for (int i = 0; i < 33; i++)
    userScreen->setDeviceName(i, defaults.getDefaultDevNames()[i]);
    
    userScreen->setSequenceName(defaults.getDefaultSeqName());
    auto defTrackNames = defaults.getDefaultTrackNames();
    
    for (int i = 0; i < 64; i++)
    userScreen->setTrackName(i, defTrackNames[i]);
    
    userScreen->setDeviceNumber(defaults.getDevices()[0]);
    userScreen->setTimeSig(defaults.getTimeSigNum(), defaults.getTimeSigDen());
    userScreen->setPgm(defaults.getPgms()[0]);
    userScreen->setTempo(defaults.getTempo() / 10.0);
    userScreen->setVelo(defaults.getTrVelos()[0]);
}

void NvRam::saveUserScreenValues(mpc::Mpc& mpc)
{
    DefaultsParser dp(mpc);
    
    string fileName = mpc::Paths::configPath() + "nvram.vmp";
    
    File file(fileName, nullptr);
    
    if (!file.exists())
        file.create();
    
    auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);
    auto bytes = dp.getBytes();
    stream.write(&bytes[0], bytes.size());
    stream.close();
}

void NvRam::saveVmpcSettings(mpc::Mpc& mpc)
{
    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
    auto audioMidiServices  = mpc.getAudioMidiServices().lock();
    string fileName = mpc::Paths::configPath() + "vmpc-specific.ini";
    
    File file(fileName, nullptr);
    
    if (!file.exists())
        file.create();
    
    auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);

    vector<char> bytes{
        (char) (vmpcSettingsScreen->initialPadMapping),
        (char) (vmpcSettingsScreen->_16LevelsEraseMode),
        (char) (vmpcAutoSaveScreen->autoSaveOnExit),
        (char) (vmpcAutoSaveScreen->autoLoadOnStart),
        (char) (audioMidiServices->getRecordLevel()),
        (char) (audioMidiServices->getMasterLevel()),
        (char) (mpc.getHardware().lock()->getSlider().lock()->getValue())
    };
    
    stream.write(&bytes[0], bytes.size());
    stream.close();
}

void NvRam::loadVmpcSettings(mpc::Mpc& mpc)
{
    string path = mpc::Paths::configPath() + "vmpc-specific.ini";
    File file(path, nullptr);

    auto audioMidiServices  = mpc.getAudioMidiServices().lock();

    if (!file.exists() || file.getLength() != 7)
    {
        audioMidiServices->setRecordLevel(DEFAULT_REC_GAIN);
        audioMidiServices->setMasterLevel(DEFAULT_MAIN_VOLUME);
        return;
    }
    
    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
    
    vector<char> bytes(7);
    file.getData(&bytes);
    
    vmpcSettingsScreen->initialPadMapping = bytes[0];
    vmpcSettingsScreen->_16LevelsEraseMode = bytes[1];
    vmpcAutoSaveScreen->autoSaveOnExit = bytes[2];
    vmpcAutoSaveScreen->autoLoadOnStart = bytes[3];
    audioMidiServices->setRecordLevel(bytes[4]);
    audioMidiServices->setMasterLevel(bytes[5]);
    mpc.getHardware().lock()->getSlider().lock()->setValue(bytes[6]);
    
}
