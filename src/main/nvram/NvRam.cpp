#include "NvRam.hpp"

#include "nvram/DefaultsParser.hpp"

#include "Mpc.hpp"

#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/OthersScreen.hpp"

#include "engine/EngineHost.hpp"

#include "hardware/Hardware.hpp"
#include "sequencer/TimeSignature.hpp"

#include "file/all/AllParser.hpp"
#include "file/all/Defaults.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

void NvRam::loadUserScreenValues(Mpc &mpc)
{
    const auto path = mpc.paths->configPath() / "nvram.vmp";

    const auto existsRes = mpc_fs::exists(path);
    if (!existsRes)
    {
        MLOG("NvRam::loadUserScreenValues could not check existence for '" +
             path.string() + "': " + existsRes.error().message);
        return;
    }

    if (!*existsRes)
    {
        return;
    }

    const auto sizeRes = mpc_fs::file_size(path);
    if (!sizeRes)
    {
        MLOG("NvRam::loadUserScreenValues could not read size for '" +
             path.string() + "': " + sizeRes.error().message);
        return;
    }

    if (*sizeRes != file::all::AllParser::DEFAULTS_LENGTH)
    {
        return;
    }

    auto defaults = DefaultsParser::AllDefaultsFromFile(mpc, path);
    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();

    userScreen->lastBar = defaults.getBarCount() - 1;
    userScreen->busType = sequencer::busIndexToBusType(defaults.getBusses()[0]);

    userScreen->device = defaults.getDevices()[0];

    sequencer::TimeSignature timeSignature;
    timeSignature.numerator = TimeSigNumerator(defaults.getTimeSigNum());
    timeSignature.denominator = TimeSigDenominator(defaults.getTimeSigDen());
    userScreen->timeSig = timeSignature;
    userScreen->pgm = defaults.getPgms()[0];
    userScreen->tempo = defaults.getTempo() / 10.0;
    userScreen->velo = defaults.getTrVelos()[0];
}

void NvRam::saveUserScreenValues(Mpc &mpc)
{
    DefaultsParser dp(mpc);
    const auto path = mpc.paths->configPath() / "nvram.vmp";
    const auto writeRes = set_file_data(path, dp.getBytes());
    if (!writeRes)
    {
        MLOG("NvRam::saveUserScreenValues failed for '" + path.string() +
             "': " + writeRes.error().message);
    }
}

void NvRam::saveVmpcSettings(Mpc &mpc)
{
    const auto vmpcSettingsScreen =
        mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    const auto vmpcAutoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    const auto othersScreen = mpc.screens->get<ScreenId::OthersScreen>();

    const auto engineHost = mpc.getEngineHost();
    const auto path = mpc.paths->vmpcSpecificConfigPath();

    const std::vector<char> bytes{
        static_cast<char>(vmpcSettingsScreen->initialPadMapping),
        static_cast<char>(vmpcSettingsScreen->sixteenLevelsEraseMode),
        static_cast<char>(vmpcAutoSaveScreen->autoSaveOnExit),
        static_cast<char>(vmpcAutoSaveScreen->autoLoadOnStart),
        static_cast<char>(engineHost->getRecordLevel()),
        static_cast<char>(engineHost->getMainLevel()),
        static_cast<char>(mpc.getHardware()->getSlider()->getValue()),
        static_cast<char>(vmpcSettingsScreen->autoConvertWavs),
        0x00, // This was tap averaging, but it does not belong here
        static_cast<char>(othersScreen->getContrast()),
        0x00, // This was MIDI control mode (VMPC or ORIGINAL), but this feature
              // (configurability) was removed.
        static_cast<char>(vmpcSettingsScreen->nameTypingWithKeyboardEnabled),
        static_cast<char>(vmpcSettingsScreen->bigTimeShiftEnabled)};

    const auto writeRes = set_file_data(path, bytes);
    if (!writeRes)
    {
        MLOG("NvRam::saveVmpcSettings failed for '" + path.string() + "': " +
             writeRes.error().message);
    }
}

void NvRam::loadVmpcSettings(Mpc &mpc)
{
    const auto engineHost = mpc.getEngineHost();

    const auto path = mpc.paths->vmpcSpecificConfigPath();

    const auto existsRes = mpc_fs::exists(path);
    if (!existsRes)
    {
        MLOG("NvRam::loadVmpcSettings could not check existence for '" +
             path.string() + "': " + existsRes.error().message);
    }

    if (!existsRes || !*existsRes)
    {
        engineHost->setRecordLevel(DEFAULT_REC_GAIN);
        mpc.getHardware()->getRecPot()->setValue(DEFAULT_REC_GAIN * 0.01f);
        engineHost->setMainLevel(DEFAULT_MAIN_VOLUME);
        mpc.getHardware()->getVolPot()->setValue(DEFAULT_MAIN_VOLUME * 0.01f);
        return;
    }

    const auto vmpcSettingsScreen =
        mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    const auto vmpcAutoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    const auto othersScreen = mpc.screens->get<ScreenId::OthersScreen>();

    const auto bytesRes = get_file_data(path);
    if (!bytesRes)
    {
        MLOG("NvRam::loadVmpcSettings failed for '" + path.string() + "': " +
             bytesRes.error().message);
        return;
    }
    const auto &bytes = *bytesRes;

    if (bytes.size() > 0)
    {
        vmpcSettingsScreen->initialPadMapping = bytes[0];
    }
    if (bytes.size() > 1)
    {
        vmpcSettingsScreen->sixteenLevelsEraseMode = bytes[1];
    }
    if (bytes.size() > 2)
    {
        vmpcAutoSaveScreen->autoSaveOnExit = bytes[2];
    }

    // I've removed Ask mode out of the equation, so any auto-persisted values
    // over 1 are now invalid.
    if (vmpcAutoSaveScreen->autoSaveOnExit == 2)
    {
        vmpcAutoSaveScreen->autoSaveOnExit = 1;
    }

    if (bytes.size() > 3)
    {
        vmpcAutoSaveScreen->autoLoadOnStart = bytes[3];
    }
    if (bytes.size() > 4)
    {
        engineHost->setRecordLevel(bytes[4]);
        mpc.getHardware()->getRecPot()->setValue(bytes[4] * 0.01f);
    }

    if (bytes.size() > 5)
    {
        engineHost->setMainLevel(bytes[5]);
        mpc.getHardware()->getVolPot()->setValue(bytes[5] * 0.01f);
    }

    if (bytes.size() > 6)
    {
        mpc.getHardware()->getSlider()->setValue(bytes[6]);
    }
    if (bytes.size() > 7)
    {
        vmpcSettingsScreen->autoConvertWavs = bytes[7];
    }
    // We used to have tap averaging here, but it doesn't belong here, so
    // for now we ignore this byte.
    if (bytes.size() > 9)
    {
        othersScreen->setContrast(bytes[9]);
    }
    // We used to have MIDI control mode here, but this configurability was
    // removed, so we ignore this byte.
    if (bytes.size() > 11)
    {
        vmpcSettingsScreen->nameTypingWithKeyboardEnabled =
            static_cast<bool>(bytes[11]);
    }
    if (bytes.size() > 12)
    {
        vmpcSettingsScreen->bigTimeShiftEnabled = static_cast<bool>(bytes[12]);
    }
}
