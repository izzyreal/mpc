#include <nvram/DefaultsParser.hpp>

#include <file/all/AllParser.hpp>
#include <file/all/Defaults.hpp>

#include <VecUtil.hpp>

using namespace mpc::nvram;
using namespace std;

mpc::file::all::Defaults DefaultsParser::AllDefaultsFromFile(moduru::file::File& file)
{
	vector<char> data;
	file.getData(&data);
	return mpc::file::all::Defaults(moduru::VecUtil::CopyOfRange(&data, 0, mpc::file::all::AllParser::DEFAULTS_LENGTH));
}

DefaultsParser::DefaultsParser(weak_ptr<mpc::ui::UserDefaults> ud)
{
	auto defaults = mpc::file::all::Defaults(ud.lock().get());
	saveBytes = defaults.getBytes();
}

vector<char> DefaultsParser::getBytes()
{
    return saveBytes;
}
