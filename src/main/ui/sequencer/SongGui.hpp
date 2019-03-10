#pragma once
#include <observer/Observable.hpp>

#include <string>

namespace mpc {
	namespace ui {
		namespace sequencer {

			class SongGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				int offset{};
				int selectedSongIndex{};
				std::string defaultSongName{};
				bool loop{};

			public:
				void setOffset(int i);
				int getOffset();
				int getSelectedSongIndex();
				void setSelectedSongIndex(int i);
				void setDefaultSongName(std::string s);
				std::string getDefaultSongName();
				void setLoop(bool b);
				bool isLoopEnabled();

				SongGui();
			};

		}
	}
}
