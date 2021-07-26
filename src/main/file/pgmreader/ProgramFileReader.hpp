#pragma once

#include <vector>
#include <memory>

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::file::pgmreader {
	class PgmHeader;
	class SoundNames;
	class ProgramName;
	class PgmAllNoteParameters;
	class Mixer;
	class Slider;
	class Pads;
}

namespace mpc::file::pgmreader {

	class ProgramFileReader
	{

	private:
		PgmHeader* pgmHeader;
		SoundNames* sampleNames;
		ProgramName* programName;
		PgmAllNoteParameters* midiNotes;
		Mixer* mixer;
		Slider* slider;
		Pads* pads;
		std::weak_ptr<mpc::disk::MpcFile> programFile;

	public:
		std::vector<char> readProgramFileArray();

	public:
		PgmHeader* getHeader();
		SoundNames* getSampleNames();
		PgmAllNoteParameters* getAllNoteParameters();
		Mixer* getMixer();
		Pads* getPads();
		ProgramName* getProgramName();
		Slider* getSlider();

		ProgramFileReader(std::weak_ptr<mpc::disk::MpcFile>);
		~ProgramFileReader();
	};
}
