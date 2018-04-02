#pragma once
#include <sequencer/Event.hpp>

namespace mpc {
	namespace sequencer {

		class SystemExclusiveEvent
			: public Event
		{

		private:
			std::vector<char> bytes{};

		public:
			void setByteA(int i);
			int getByteA();
			void setByteB(int i);
			int getByteB();
			void setBytes(std::vector<char>* ba);
			std::vector<char>* getBytes();

			void CopyValuesTo(std::weak_ptr<Event> dest) override;

			SystemExclusiveEvent();

		};

	}
}
