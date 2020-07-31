#include "DefaultsParser.hpp"

#include <file/all/AllParser.hpp>
#include <file/all/Defaults.hpp>

#include <VecUtil.hpp>

using namespace mpc::nvram;

using namespace std;

mpc::file::all::Defaults DefaultsParser::AllDefaultsFromFile(mpc::Mpc& mpc, moduru::file::File& file)
{
	vector<char> data;
	file.getData(&data);
	return mpc::file::all::Defaults(mpc, moduru::VecUtil::CopyOfRange(&data, 0, mpc::file::all::AllParser::DEFAULTS_LENGTH));
}

DefaultsParser::DefaultsParser(mpc::Mpc& mpc)
{
	saveBytes = mpc::file::all::Defaults(mpc).getBytes();
}

vector<char> DefaultsParser::getBytes()
{
    return saveBytes;
}
