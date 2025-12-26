#include "Paths.hpp"

#include <platform_folders.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS
std::string getIosSharedDocumentsFolder();
#endif

using namespace mpc;

Paths::Documents::Documents(const Paths *paths) : paths(paths) {}

fs::path Paths::Documents::appDocumentsPath() const
{
    return paths->appDocumentsPath();
}

Paths::Paths() : documents(std::make_unique<Documents>(this)) {}

fs::path Paths::appDocumentsPath() const
{
#if TARGET_OS_IOS
    auto path = fs::path(getIosSharedDocumentsFolder());
#else
    auto path = fs::path(sago::getDocumentsFolder()) / "VMPC2000XL";
#endif
    return path;
}

fs::path Paths::appConfigHome() const
{
    auto path = fs::path(sago::getConfigHome()) / "VMPC2000XL";
    return path;
}

fs::path Paths::configPath() const
{
    auto path = appConfigHome() / "config";
    return path;
}

fs::path Paths::keyboardBindingsPath() const
{
    auto path = appConfigHome() / "keyboard_bindings.json";
    return path;
}

Paths::Documents *Paths::getDocuments() const
{
    return documents.get();
}

fs::path Paths::Documents::storesPath() const
{
    auto storesPath = appDocumentsPath() / "Volumes";
    return storesPath;
}

fs::path Paths::Documents::defaultLocalVolumePath() const
{
    auto storesPath = appDocumentsPath() / "Volumes" / "MPC2000XL";
    return storesPath;
}

fs::path Paths::Documents::logFilePath() const
{
    auto logFilePath = appDocumentsPath() / "vmpc.log";
    return logFilePath;
}

fs::path Paths::Documents::recordingsPath() const
{
    auto recPath = appDocumentsPath() / "Recordings";
    return recPath;
}

fs::path Paths::Documents::midiControlPresetsPath() const
{
    auto path = appDocumentsPath() / "MidiControlPresets";
    return path;
}

fs::path Paths::Documents::autoSavePath() const
{
    auto path = appDocumentsPath() / "AutoSave";
    return path;
}

fs::path Paths::Documents::demoDataPath() const
{
    auto demoDataPath = defaultLocalVolumePath() / "DEMOS";
    return demoDataPath;
}

fs::path Paths::Documents::tempPath() const
{
    auto tempPath = appDocumentsPath() / "Temp";
    return tempPath;
}
