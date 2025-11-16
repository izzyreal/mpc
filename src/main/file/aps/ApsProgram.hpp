#pragma once

#include <file/aps/ApsMixer.hpp>

#include <vector>
#include <string>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc::sampler
{
    class Program;
}

namespace mpc::file::aps
{
    class ApsNoteParameters;
    class ApsAssignTable;
    class ApsSlider;
} // namespace mpc::file::aps

namespace mpc::file::aps
{
    class ApsProgram
    {

    public:
        static const int INDEX_OFFSET{0};
        static const int UNKNOWN_OFFSET{1};
        const std::vector<char> UNKNOWN{0, 7, 4, 30, 0};
        static const int NAME_OFFSET{6};
        static const int NAME_LENGTH{16};
        static const int NAME_TERMINATOR_LENGTH{1};
        static const int SLIDER_OFFSET{23};
        static const int SLIDER_LENGTH{10};
        static const int PADDING1_LENGTH{5};
        static const int NOTE_PARAMETERS_OFFSET{38};
        static const int NOTE_PARAMETERS_LENGTH{26};
        static const int PADDING2_LENGTH{7};
        static const int MIXER_OFFSET{1703};
        static const int MIXER_END{2087};
        static const int PADDING3_LENGTH{3};
        static const int ASSIGN_TABLE_OFFSET{2090};
        static const int ASSIGN_TABLE_LENGTH{64};

        std::string name;
        int index;
        ApsSlider *slider = nullptr;
        std::vector<ApsNoteParameters *> noteParameters =
            std::vector<ApsNoteParameters *>(64);
        ApsMixer *mixer = nullptr;
        ApsAssignTable *assignTable = nullptr;
        std::vector<char> saveBytes;

        ApsNoteParameters *getNoteParameters(int noteIndex) const;
        ApsMixer *getMixer() const;
        ApsAssignTable *getAssignTable() const;
        ApsSlider *getSlider() const;
        std::string getName();

        std::vector<char> getBytes();

        performance::StereoMixer getStereoMixerChannel(int noteIndex) const;

        performance::IndivFxMixer getIndivFxMixerChannel(int noteIndex) const;

        ApsProgram(const std::vector<char> &loadBytes);
        ApsProgram(sampler::Program *program, int index);
        ~ApsProgram();
    };
} // namespace mpc::file::aps
