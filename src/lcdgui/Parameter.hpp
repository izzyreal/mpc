#pragma once
#include <string>
#include<memory>
//#include <IPlugBase.h>

namespace mpc {
	
	namespace lcdgui {

		class Label;
		class Field;
		class Parameter {

		private:
			std::weak_ptr<Field> tf;
			std::weak_ptr<Label> label;

		public:
			std::weak_ptr<Field> getTf();
			std::weak_ptr<Label> getLabel();

		public:
			Parameter(std::weak_ptr<Field> tf, std::weak_ptr<Label> label, std::string labelStr, std::string name, int x, int y, int size);
			~Parameter();

		};

	}
}
