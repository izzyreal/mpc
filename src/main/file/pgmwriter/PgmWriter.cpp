#include <file/pgmwriter/PgmWriter.hpp>

#include <file/pgmwriter/PWHeader.hpp>
#include <file/pgmwriter/MidiNotes.hpp>
#include <file/pgmwriter/Mixer.hpp>
#include <file/pgmwriter/Pads.hpp>
#include <file/pgmwriter/PgmName.hpp>
#include <file/pgmwriter/SampleNames.hpp>
#include <file/pgmwriter/PWSlider.hpp>
#include <sampler/Program.hpp>

using namespace mpc::file::pgmwriter;
using namespace std;

PgmWriter::PgmWriter(mpc::sampler::Program* program, weak_ptr<mpc::sampler::Sampler> sampler)
{
	this->program = program;
	
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

vector<char> PgmWriter::get()
{
	auto charArrayChunks = vector<vector<char>>(7);
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

	vector<char> result(programFileSize);
	int counter = 0;

	for (auto ca : charArrayChunks)
	{
		for (auto c : ca)
			result[counter++] = c;
	}
	return result;
}
