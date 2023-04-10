#include "DefaultsParser.hpp"

#include <file/all/AllParser.hpp>
#include <file/all/Defaults.hpp>

#include <VecUtil.hpp>
#include <utility>

using namespace mpc::nvram;
using namespace mpc::file::all;

Defaults DefaultsParser::AllDefaultsFromFile(mpc::Mpc& mpc, fs::path p)
{

    auto data = get_file_data(std::move(p));
	return { mpc, moduru::VecUtil::CopyOfRange(data, 0, AllParser::DEFAULTS_LENGTH) };
}

DefaultsParser::DefaultsParser(mpc::Mpc& mpc)

{
	saveBytes = Defaults(mpc).getBytes();
}

std::vector<char> DefaultsParser::getBytes()
{
    return saveBytes;
}
