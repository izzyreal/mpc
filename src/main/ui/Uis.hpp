#pragma once

#include <vector>
#include <string>

namespace mpc::ui {

	class Uis
	{

	private:
		std::vector<std::string> someNoteNames{ "C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B." };

	public:
		static std::vector<std::string> noteNames;

	public:
		Uis();

	};
}
