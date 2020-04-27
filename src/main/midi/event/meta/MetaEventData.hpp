#pragma once

#include <midi/util/VariableLengthInt.hpp>

#include <io/InputStream.hpp>

namespace mpc::midi::event::meta {

	class MetaEventData
	{

	public:
		int type{};
		mpc::midi::util::VariableLengthInt* length{};
		std::vector<char> data{};

	public:
		MetaEventData(moduru::io::InputStream* in);
	};
}
