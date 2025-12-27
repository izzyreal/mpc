#pragma once

#include "disk/SoundSaver.hpp"
#include "disk/AllLoader.hpp"

#include <vector>
#include <string>
#include <thread>

#include <mpc_types.hpp>
#include <mpc_fs.hpp>

namespace mpc
{
    class Mpc;
}

namespace mpc::input::midi::legacy
{
    class MidiControlPreset;
}

namespace mpc::lcdgui::screens::window
{
    class LoadASequenceScreen;
    class LoadAProgramScreen;
    class LoadApsFileScreen;
    class Mpc2000XlAllFileScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::sampler
{
    class Program;
    class Sound;
} // namespace mpc::sampler

namespace mpc::file::wav
{
    class WavFile;
}

namespace mpc::file::sndreader
{
    class SndReader;
}

namespace mpc::disk
{

    class MpcFile;
    class Volume;

    class AbstractDisk
    {
    protected:
        explicit AbstractDisk(Mpc &);
        ~AbstractDisk();

        Mpc &mpc;
        const std::vector<std::string> extensions{
            "", "SND", "PGM", "APS", "MID", "ALL", "WAV", "SEQ", "SET"};
        std::vector<std::shared_ptr<MpcFile>> files;
        std::vector<std::shared_ptr<MpcFile>> allFiles;
        std::vector<std::shared_ptr<MpcFile>> parentFiles;

        virtual int getPathDepth() = 0;

    public:
        virtual std::shared_ptr<MpcFile> newFile(const std::string &name) = 0;
        bool deleteSelectedFile() const;

        std::vector<std::string> getFileNames();
        std::string getFileName(int i) const;
        std::vector<std::string> getParentFileNames() const;

        std::shared_ptr<MpcFile> getFile(int fileIndex);
        std::shared_ptr<MpcFile> getFile(const std::string &fileName);
        std::vector<std::shared_ptr<MpcFile>> &getAllFiles();
        std::shared_ptr<MpcFile> getParentFile(int i);

        void writeSnd(const std::shared_ptr<sampler::Sound> &,
                      const std::string &fileName);
        void writeWav(const std::shared_ptr<sampler::Sound> &,
                      const std::string &fileName);
        void writeMid(const std::shared_ptr<sequencer::Sequence> &,
                      const std::string &fileName);
        void writePgm(const std::shared_ptr<sampler::Program> &,
                      const std::string &fileName);
        void writeAps(const std::string &fileName);
        void writeAll(const std::string &fileName);
        void writeMidiControlPreset(
            std::shared_ptr<input::midi::legacy::MidiControlPreset> preset);

        void readMidiControlPreset(
            const fs::path &p,
            const std::shared_ptr<input::midi::legacy::MidiControlPreset> &);

        bool checkExists(const std::string &fileName);
        virtual bool deleteRecursive(std::weak_ptr<MpcFile>);

        bool isRoot();

        virtual void flush() = 0;
        virtual void close() = 0;
        virtual void initFiles() = 0;
        virtual std::string getDirectoryName() = 0;
        virtual bool moveBack() = 0;
        virtual bool moveForward(const std::string &directoryName) = 0;
        virtual bool deleteAllFiles(int dwGuiDelete) = 0;
        virtual bool newFolder(const std::string &newDirName) = 0;
        virtual std::string getAbsolutePath() = 0;
        virtual std::string getTypeShortName() = 0;

        virtual uint64_t getTotalSize() = 0;
        virtual std::string getVolumeLabel() = 0;
        virtual Volume &getVolume() = 0;
        virtual void initRoot() = 0;

        void readPgm2(std::shared_ptr<MpcFile>,
                      std::shared_ptr<sampler::Program>, int programIndex);

        sequence_or_error
        loadOneSequenceFromAllFile(std::shared_ptr<MpcFile>,
                                   SequenceIndex sourceIndexInAllFile,
                                   SequenceIndex destIndexInMpcMemory);

    private:
        std::thread programSoundsSaveThread = std::thread([] {});
        std::thread readApsThread = std::thread([] {});
        std::thread readPgmThread = std::thread([] {});
        std::unique_ptr<SoundSaver> soundSaver;
        std::unique_ptr<AllLoader> allLoader;

        friend class SoundLoader; // Temporary access to readWav2 and readSnd2
                                  // until a readWav/readSnd/etc interface is
                                  // exposed
        friend class lcdgui::screens::window::LoadASequenceScreen;
        friend class lcdgui::screens::window::LoadAProgramScreen;
        friend class lcdgui::screens::window::LoadApsFileScreen;
        friend class lcdgui::screens::window::Mpc2000XlAllFileScreen;

        template <typename return_type>
        tl::expected<return_type, mpc_io_error_msg> performIoOrOpenErrorPopup(
            std::function<tl::expected<return_type, mpc_io_error_msg>()>
                ioFunc);

        template <typename return_type>
        void performIoOrOpenErrorPopupNonReturning(
            std::function<tl::expected<return_type, mpc_io_error_msg>()>
                ioFunc);

        wav_or_error readWavMeta(std::shared_ptr<MpcFile>);
        sound_or_error readWav2(
            std::shared_ptr<MpcFile>,
            std::function<sound_or_error(std::shared_ptr<file::wav::WavFile>)>
                onSuccess);
        sound_or_error
            readSnd2(std::shared_ptr<MpcFile>,
                     std::function<sound_or_error(
                         std::shared_ptr<file::sndreader::SndReader>)>);
        sequence_or_error readMid2(std::shared_ptr<MpcFile>);

        void readAps2(std::shared_ptr<MpcFile>, std::function<void()> onSuccess,
                      bool headless);
        void readAll2(std::shared_ptr<MpcFile>,
                      std::function<void()> onSuccess);

        sequence_meta_infos_or_error
            readSequenceMetaInfosFromAllFile(std::shared_ptr<MpcFile>);
    };
} // namespace mpc::disk
