#include "NvRam.hpp"

#include <nvram/DefaultsParser.hpp>

#include <Mpc.hpp>

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>

#include <file/all/AllParser.hpp>
#include <file/all/Defaults.hpp>

using namespace mpc::nvram;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

void NvRam::loadUserScreenValues(mpc::Mpc& mpc)
{
  const auto path = mpc.paths->configPath() / "nvram.vmp";

  if (!fs::exists(path) ||
      fs::file_size(path) != mpc::file::all::AllParser::DEFAULTS_LENGTH)
  {
      return;
  }
  
  auto defaults = DefaultsParser::AllDefaultsFromFile(mpc, path);
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
  auto path = mpc.paths->configPath() / "nvram.vmp";
  set_file_data(path, dp.getBytes());
}

void NvRam::saveVmpcSettings(mpc::Mpc& mpc)
{
  auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
  auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
  auto othersScreen = mpc.screens->get<OthersScreen>("others");
  
  auto audioMidiServices  = mpc.getAudioMidiServices();
  auto path = mpc.paths->configPath() / "vmpc-specific.ini";
  
  std::vector<char> bytes{
    (char) (vmpcSettingsScreen->initialPadMapping),
    (char) (vmpcSettingsScreen->_16LevelsEraseMode),
    (char) (vmpcAutoSaveScreen->autoSaveOnExit),
    (char) (vmpcAutoSaveScreen->autoLoadOnStart),
    (char) (audioMidiServices->getRecordLevel()),
    (char) (audioMidiServices->getMainLevel()),
    (char) (mpc.getHardware()->getSlider()->getValue()),
    (char) (vmpcSettingsScreen->autoConvertWavs),
    0x00, // This was tap averaging, but it does not belong here
    (char) (othersScreen->getContrast()),
    (char) (vmpcSettingsScreen->midiControlMode)
  };

  set_file_data(path, bytes);
}

void NvRam::loadVmpcSettings(mpc::Mpc& mpc)
{
  auto audioMidiServices  = mpc.getAudioMidiServices();

  auto path = mpc.paths->configPath() / "vmpc-specific.ini";

  if (!fs::exists(path))
  {
    audioMidiServices->setRecordLevel(DEFAULT_REC_GAIN);
    audioMidiServices->setMainLevel(DEFAULT_MAIN_VOLUME);
    return;
  }
  
  auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
  auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");
  auto othersScreen = mpc.screens->get<OthersScreen>("others");
  
  auto bytes = get_file_data(path);
  
  if (bytes.size() > 0) vmpcSettingsScreen->initialPadMapping = bytes[0];
  if (bytes.size() > 1) vmpcSettingsScreen->_16LevelsEraseMode = bytes[1];
  if (bytes.size() > 2) vmpcAutoSaveScreen->autoSaveOnExit = bytes[2];

  // I've removed Ask mode out of the equation, so any auto-persisted values over 1 are now invalid.
  if (vmpcAutoSaveScreen->autoSaveOnExit == 2) vmpcAutoSaveScreen->autoSaveOnExit = 1;

  if (bytes.size() > 3) vmpcAutoSaveScreen->autoLoadOnStart = bytes[3];
  if (bytes.size() > 4) audioMidiServices->setRecordLevel(bytes[4]);
  if (bytes.size() > 5) audioMidiServices->setMainLevel(bytes[5]);
  if (bytes.size() > 6) mpc.getHardware()->getSlider()->setValue(bytes[6]);
  if (bytes.size() > 7) vmpcSettingsScreen->autoConvertWavs = bytes[7];
  // We used to have tap averaging here, but it doesn't belong here, so
  // for now we ignore this byte.
  if (bytes.size() > 9) othersScreen->setContrast(bytes[9]);
  if (bytes.size() > 10) vmpcSettingsScreen->midiControlMode = bytes[10];
}
