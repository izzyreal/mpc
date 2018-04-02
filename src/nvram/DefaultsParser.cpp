#include <nvram/DefaultsParser.hpp>

#include <file/all/AllParser.hpp>
#include <file/all/Defaults.hpp>

#include <VecUtil.hpp>

using namespace mpc::nvram;
using namespace std;

DefaultsParser::DefaultsParser(moduru::file::File* file)
{
	vector<char> data;
	file->getData(&data);
	defaults = new mpc::file::all::Defaults(moduru::VecUtil::CopyOfRange(&data, 0, mpc::file::all::AllParser::DEFAULTS_LENGTH));
}

DefaultsParser::DefaultsParser(mpc::ui::UserDefaults* ud)
{
	auto defaults = new mpc::file::all::Defaults(ud);
	saveBytes = defaults->getBytes();
}

mpc::file::all::Defaults* DefaultsParser::getDefaults()
{
    return defaults;
}

vector<char> DefaultsParser::getBytes()
{
    return saveBytes;
}
