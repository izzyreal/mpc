#include "file/pgmwriter/PgmWriter.hpp"

#include "file/pgmwriter/PWHeader.hpp"
#include "file/pgmwriter/MidiNotes.hpp"
#include "file/pgmwriter/Mixer.hpp"
#include "file/pgmwriter/Pads.hpp"
#include "file/pgmwriter/PgmName.hpp"
#include "file/pgmwriter/SampleNames.hpp"
#include "file/pgmwriter/PWSlider.hpp"
#include "sampler/Program.hpp"

using namespace mpc::file::pgmwriter;

PgmWriter::PgmWriter(mpc::sampler::Program* program, std::weak_ptr<mpc::sampler::Sampler> sampler)
{
	SampleNames pwSampleNames(program, sampler);
	sampleNames = pwSampleNames.getSampleNamesArray();
	
	PWHeader pwHeader(pwSampleNames.getNumberOfSamples());
	header = pwHeader.getHeaderArray();
	
	PgmName pwPgmName(program);
	pgmName = pwPgmName.getPgmNameArray();

	Slider pwSlider(program);
	slider = pwSlider.getSliderArray();

	MidiNotes pwMidiNotes(program, pwSampleNames.getSnConvTable());
	midiNotes = pwMidiNotes.midiNotesArray;
	
	Mixer pwMixer(program);
	mixer = pwMixer.getMixerArray();
	
	Pads pwPads(program);
	pads = pwPads.getPadsArray();
}

std::vector<char> PgmWriter::get()
{
	std::vector<std::vector<char>> charArrayChunks(7);
	charArrayChunks[0] = header;
	charArrayChunks[1] = sampleNames;
	charArrayChunks[2] = pgmName;
	charArrayChunks[3] = slider;
	charArrayChunks[4] = midiNotes;
	charArrayChunks[5] = mixer;
	charArrayChunks[6] = pads;
	auto programFileSize = 0;

	for (auto ca : charArrayChunks)
		programFileSize += ca.size();

	std::vector<char> result(programFileSize);
	int counter = 0;

	for (auto ca : charArrayChunks)
	{
		for (auto c : ca)
			result[counter++] = c;
	}
	return result;
}
