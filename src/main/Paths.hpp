#pragma once

#include "mpc_fs.hpp"

#include <memory>

namespace mpc
{
    class Paths
    {
    protected:
        virtual mpc_fs::path appConfigHome() const;
        virtual mpc_fs::path appDocumentsPath() const;

    public:
        struct Documents
        {
            explicit Documents(const Paths *paths);

            mpc_fs::path appDocumentsPath() const;

            mpc_fs::path storesPath() const;
            mpc_fs::path defaultLocalVolumePath() const;

            mpc_fs::path logFilePath() const;
            mpc_fs::path recordingsPath() const;
            mpc_fs::path midiControlPresetsPath() const;
            mpc_fs::path activeMidiControlPresetPath() const;
            mpc_fs::path autoSavePath() const;

            mpc_fs::path demoDataPath() const;
            mpc_fs::path tempPath() const;

        private:
            const Paths *paths;
        };

        Paths();

        mpc_fs::path configPath() const;

        mpc_fs::path keyboardBindingsPath() const;
        mpc_fs::path vmpcSpecificConfigPath() const;

        mpc_fs::path legacyKeyboardBindingsPath() const;

        mpc_fs::path legacyActiveMidiControlPresetPath() const;

        Documents *getDocuments() const;

    private:
        const std::unique_ptr<Documents> documents;
    };
} // namespace mpc
