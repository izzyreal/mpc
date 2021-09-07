#pragma once

#include <file/File.hpp>

#include <ostream>
#include <istream>
#include <vector>
#include <memory>

namespace mpc::midi {
	class MidiTrack;
}

namespace mpc::midi {

	class MidiFile
	{

	public:
		static const int HEADER_SIZE{ 14 };

	private:
		static std::vector<char> IDENTIFIER;

	public:
		static const int DEFAULT_RESOLUTION{ 480 };

	private:
		int mType{};
		int mTrackCount{};
		int mResolution{};
		std::vector<std::shared_ptr<MidiTrack>> mTracks{};

	public:
		void setType(int type);
		int getType();
		int getTrackCount();
		void setResolution(int res);
		int getResolution();
		int getLengthInTicks();
		std::vector<std::weak_ptr<MidiTrack>> getTracks();
		void addTrack(std::shared_ptr<MidiTrack> T);
		void addTrack(std::shared_ptr<MidiTrack> T, int pos);
		void removeTrack(int pos);
		void writeToOutputStream(std::shared_ptr<std::ostream>);

	private:
		void initFromBuffer(std::vector<char>& buffer);

	public:
		MidiFile();
		MidiFile(int resolution);
		MidiFile(int resolution, std::vector<MidiTrack*> tracks);
		MidiFile(std::shared_ptr<std::istream>);
		~MidiFile();

	};
}
