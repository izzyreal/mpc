#include "Paths.hpp"

#include <sago/platform_folders.h>
#include <cstdlib>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS
std::string getIosSharedDocumentsFolder();
#endif

using namespace mpc;

Paths::Documents::Documents(const Paths *paths) : paths(paths) {}

mpc_fs::path Paths::Documents::appDocumentsPath() const
{
    return paths->appDocumentsPath();
}

Paths::Paths() : documents(std::make_unique<Documents>(this)) {}

mpc_fs::path Paths::appDocumentsPath() const
{
    if (const char *documentsPath = std::getenv("VMPC2000XL_DOCUMENTS_PATH");
        documentsPath != nullptr && *documentsPath != '\0')
    {
        return mpc_fs::path(documentsPath);
    }

#if TARGET_OS_IOS
    auto path = mpc_fs::path(getIosSharedDocumentsFolder());
#else
    auto path = mpc_fs::path(sago::getDocumentsFolder()) / "VMPC2000XL";
#endif
    return path;
}

mpc_fs::path Paths::appConfigHome() const
{
    auto path = mpc_fs::path(sago::getConfigHome()) / "VMPC2000XL";
    return path;
}

mpc_fs::path Paths::configPath() const
{
    auto path = appConfigHome() / "config";
    return path;
}

mpc_fs::path Paths::keyboardBindingsPath() const
{
    auto path = configPath() / "keyboard_bindings.json";
    return path;
}

mpc_fs::path Paths::vmpcSpecificConfigPath() const
{
    auto path = configPath() / "vmpc-specific.ini";
    return path;
}

Paths::Documents *Paths::getDocuments() const
{
    return documents.get();
}

mpc_fs::path Paths::Documents::storesPath() const
{
    auto storesPath = appDocumentsPath() / "Volumes";
    return storesPath;
}

mpc_fs::path Paths::Documents::defaultLocalVolumePath() const
{
    auto storesPath = appDocumentsPath() / "Volumes" / "MPC2000XL";
    return storesPath;
}

mpc_fs::path Paths::Documents::logFilePath() const
{
    auto logFilePath = appDocumentsPath() / "vmpc.log";
    return logFilePath;
}

mpc_fs::path Paths::Documents::recordingsPath() const
{
    auto recPath = appDocumentsPath() / "Recordings";
    return recPath;
}

mpc_fs::path Paths::Documents::midiControlPresetsPath() const
{
    auto path = appDocumentsPath() / "MidiControlPresets";
    return path;
}

mpc_fs::path Paths::Documents::activeMidiControlPresetPath() const
{
    auto path = midiControlPresetsPath() / "active_midi_control_preset.json";
    return path;
}

mpc_fs::path Paths::Documents::autoSavePath() const
{
    auto path = appDocumentsPath() / "AutoSave";
    return path;
}

mpc_fs::path Paths::Documents::demoDataPath() const
{
    auto demoDataPath = defaultLocalVolumePath() / "DEMOS";
    return demoDataPath;
}

mpc_fs::path Paths::Documents::tempPath() const
{
    auto tempPath = appDocumentsPath() / "Temp";
    return tempPath;
}

// LEGACY

mpc_fs::path Paths::legacyKeyboardBindingsPath() const
{
    auto path = configPath() / "keys.txt";
    return path;
}

mpc_fs::path Paths::legacyActiveMidiControlPresetPath() const
{
    auto path = configPath() / "midicontrolmapping.vmp";
    return path;
}
