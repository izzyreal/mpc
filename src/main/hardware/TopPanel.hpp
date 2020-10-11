#pragma once

#include <string>

namespace mpc::hardware {

	class TopPanel
	{

	private:
		bool afterEnabled = false;
		bool fullLevelEnabled = false;
		bool sixteenLevelsEnabled = false;

	public:
		bool isAfterEnabled();
		void setAfterEnabled(bool b);
		bool isFullLevelEnabled();
		void setFullLevelEnabled(bool b);
		bool isSixteenLevelsEnabled();
		void setSixteenLevelsEnabled(bool b);

	};
}
