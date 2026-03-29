#include "DefaultsParser.hpp"

#include "file/all/AllParser.hpp"
#include "file/all/Defaults.hpp"

#include "Logger.hpp"
#include "Util.hpp"
#include <utility>

using namespace mpc::nvram;
using namespace mpc::file::all;

Defaults DefaultsParser::AllDefaultsFromFile(Mpc &mpc, mpc_fs::path p)
{
    const auto dataRes = get_file_data(p);
    if (!dataRes)
    {
        MLOG("DefaultsParser::AllDefaultsFromFile failed for '" + p.string() +
             "': " + dataRes.error().message);
        return {mpc, {}};
    }

    const auto &data = *dataRes;
    return {mpc, Util::vecCopyOfRange(data, 0, AllParser::DEFAULTS_LENGTH)};
}

DefaultsParser::DefaultsParser(Mpc &mpc)

{
    saveBytes = Defaults(mpc).getBytes();
}

std::vector<char> DefaultsParser::getBytes()
{
    return saveBytes;
}
