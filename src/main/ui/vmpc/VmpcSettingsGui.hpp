#pragma once
#include <observer/Observable.hpp>

namespace mpc::ui::vmpc {

	class VmpcSettingsGui
		: public moduru::observer::Observable
	{
	public:
		static VmpcSettingsGui& instance()
		{
			static auto vmpcSettingsGui = VmpcSettingsGui();
			return vmpcSettingsGui;
		}

	private:
		VmpcSettingsGui();
		~VmpcSettingsGui();

	public:
		int getInitialPadMapping();
		void setInitialPadMapping(int i);

	private:
		int initialPadMapping = 0;
	};
}
