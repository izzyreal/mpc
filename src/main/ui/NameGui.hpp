#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {

		class NameGui
			: public moduru::observer::Observable
		{

		private:
			std::string name{ "" };
			bool editing{ false };
			std::string parameterName{ "" };
			int nameLimit{ 0 };

		public:
			void setName(std::string name);
			void setNameLimit(int i);
			int getNameLimit();
			void setName(std::string str, int i);
			std::string getName();
			void changeNameCharacter(int i, bool up);
			void setNameBeingEdited(bool b);
			void setParameterName(std::string s);
			std::string getParameterName();
			bool isNameBeingEdited();

			NameGui();
		};

	}
}
