#pragma once

#include <midi/util/VariableLengthInt.hpp>

#include <io/InputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData
				{

				public:
					int type{};
					mpc::midi::util::VariableLengthInt* length{};
					std::vector<char> data{};

				public:
					MetaEventData(moduru::io::InputStream* in);
					//friend class MetaEvent;
				};
			}
		}
	}
}
