#pragma once

#include <file/aps/ApsMixer.hpp>
#include <file/aps/ApsProgram.hpp>
#include <file/aps/ApsDrumConfiguration.hpp>
#include <file/aps/ApsAssignTable.hpp>
#include <file/aps/ApsGlobalParameters.hpp>
#include <file/aps/ApsHeader.hpp>
#include <file/aps/ApsName.hpp>
#include <file/aps/ApsSoundNames.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class MpcFile;
}
namespace mpc::file::aps
{
    class ApsParser
    {

    public:
        static const int HEADER_OFFSET{0};
        static const int HEADER_LENGTH{4};
        static const int SOUND_NAME_LENGTH{17};
        static const int PAD_LENGTH1{2};
        static const int APS_NAME_LENGTH{17};
        static const int PARAMETERS_LENGTH{8};
        static const int TABLE_LENGTH{64};
        static const int PAD_LENGTH2{7};
        static const int MIXER_LENGTH{384};
        static const int DRUM_CONFIG_LENGTH{9};
        static const int DRUM_PAD_LENGTH{3};
        static const int PAD_LENGTH3{15};
        static const int PROGRAM_LENGTH{2350};
        static const int PROGRAM_PAD_LENGTH{4};
        static const int LAST_PROGRAM_PAD_LENGTH{2};
        static const char NAME_TERMINATOR{0};
        int programCount;
        std::unique_ptr<ApsHeader> header;
        std::unique_ptr<ApsSoundNames> soundNames;
        std::unique_ptr<ApsName> apsName;
        std::unique_ptr<ApsGlobalParameters> globalParameters;
        std::unique_ptr<ApsAssignTable> maTable;
        std::vector<std::unique_ptr<ApsMixer>> drumMixers =
            std::vector<std::unique_ptr<ApsMixer>>(4);
        std::vector<std::unique_ptr<ApsDrumConfiguration>> drumConfigurations =
            std::vector<std::unique_ptr<ApsDrumConfiguration>>(4);
        std::vector<std::unique_ptr<ApsProgram>> programs;
        std::vector<char> saveBytes;

        std::vector<std::string> getSoundNames() const;
        bool isHeaderValid() const;
        std::string getApsName() const;
        std::vector<ApsProgram *> getPrograms() const;
        std::vector<ApsMixer *> getDrumMixers() const;
        ApsDrumConfiguration *getDrumConfiguration(int i) const;
        ApsGlobalParameters *getGlobalParameters() const;

        std::vector<char> getBytes();

        ApsParser(const std::vector<char> &loadBytes);
        ApsParser(Mpc &, std::string apsNameString);
    };
} // namespace mpc::file::aps
