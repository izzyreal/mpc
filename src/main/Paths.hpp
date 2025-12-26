#pragma once

#include "mpc_fs.hpp"

#include <memory>

namespace mpc
{
    class Paths
    {
    protected:
        virtual fs::path appConfigHome() const;
        virtual fs::path appDocumentsPath() const;

    public:
        struct Documents
        {
            explicit Documents(const Paths *paths);

            fs::path appDocumentsPath() const;

            fs::path storesPath() const;
            fs::path defaultLocalVolumePath() const;

            fs::path logFilePath() const;
            fs::path recordingsPath() const;
            fs::path midiControlPresetsPath() const;
            fs::path autoSavePath() const;

            fs::path demoDataPath() const;
            fs::path tempPath() const;

        private:
            const Paths *paths;
        };

        Paths();

        fs::path configPath() const;

        fs::path keyboardBindingsPath() const;

        Documents *getDocuments() const;

    private:
        const std::unique_ptr<Documents> documents;
    };
} // namespace mpc
