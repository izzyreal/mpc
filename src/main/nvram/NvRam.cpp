#include "NvRam.hpp"

#include <nvram/DefaultsParser.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>

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

void NvRam::loadUserScreenValues(mpc::Mpc& mpc)
{
  std::string path = mpc::Paths::configPath() + "nvram.vmp";
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

  std::string fileName = mpc::Paths::configPath() + "nvram.vmp";
  
  File file(fileName, nullptr);
  
  if (!file.exists())
    file.create();
  
  auto stream = FileUtil::ofstreamw(fileName, std::ios::binary | std::ios::out);
  auto bytes = dp.getBytes();
  stream.write(&bytes[0], bytes.size());
  stream.close();
}

void NvRam::saveVmpcSettings(mpc::Mpc& mpc)
{
  auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
  auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
  auto othersScreen = mpc.screens->get<OthersScreen>("others");
  
  auto audioMidiServices  = mpc.getAudioMidiServices();
  std::string fileName = mpc::Paths::configPath() + "vmpc-specific.ini";
  
  File file(fileName, nullptr);
  
  if (!file.exists()) file.create();
  
  auto stream = FileUtil::ofstreamw(fileName, std::ios::binary | std::ios::out);
  
  std::vector<char> bytes{
    (char) (vmpcSettingsScreen->initialPadMapping),
    (char) (vmpcSettingsScreen->_16LevelsEraseMode),
    (char) (vmpcAutoSaveScreen->autoSaveOnExit),
    (char) (vmpcAutoSaveScreen->autoLoadOnStart),
    (char) (audioMidiServices->getRecordLevel()),
    (char) (audioMidiServices->getMasterLevel()),
    (char) (mpc.getHardware()->getSlider()->getValue()),
    (char) (vmpcSettingsScreen->autoConvertWavs),
    (char) (othersScreen->getTapAveraging()),
    (char) (othersScreen->getContrast()),
    (char) (vmpcSettingsScreen->midiControlMode)
  };
  
  stream.write(&bytes[0], bytes.size());
  stream.close();
}

void NvRam::loadVmpcSettings(mpc::Mpc& mpc)
{
    std::string path = mpc::Paths::configPath() + "vmpc-specific.ini";
  File file(path, nullptr);
  
  auto audioMidiServices  = mpc.getAudioMidiServices();
  
  if (!file.exists())
  {
    audioMidiServices->setRecordLevel(DEFAULT_REC_GAIN);
    audioMidiServices->setMasterLevel(DEFAULT_MAIN_VOLUME);
    return;
  }
  
  auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
  auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
  auto othersScreen = mpc.screens->get<OthersScreen>("others");
  
  std::vector<char> bytes;
  file.getData(&bytes);
  
  if (bytes.size() > 0) vmpcSettingsScreen->initialPadMapping = bytes[0];
  if (bytes.size() > 1) vmpcSettingsScreen->_16LevelsEraseMode = bytes[1];
  if (bytes.size() > 2) vmpcAutoSaveScreen->autoSaveOnExit = bytes[2];
  if (bytes.size() > 3) vmpcAutoSaveScreen->autoLoadOnStart = bytes[3];
  if (bytes.size() > 4) audioMidiServices->setRecordLevel(bytes[4]);
  if (bytes.size() > 5) audioMidiServices->setMasterLevel(bytes[5]);
  if (bytes.size() > 6) mpc.getHardware()->getSlider()->setValue(bytes[6]);
  if (bytes.size() > 7) vmpcSettingsScreen->autoConvertWavs = bytes[7];
  if (bytes.size() > 8) othersScreen->setTapAveraging(bytes[8]);
  if (bytes.size() > 9) othersScreen->setContrast(bytes[9]);
  if (bytes.size() > 10) vmpcSettingsScreen->midiControlMode = bytes[10];
}
