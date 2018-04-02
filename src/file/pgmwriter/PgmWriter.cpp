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
	auto sn = new SampleNames(program, sampler);
	sampleNames = sn->getSampleNamesArray();
	auto pwheader = new PWHeader(sn->getNumberOfSamples());
	header = pwheader->getHeaderArray();
	auto pwpgmname = new PgmName(program);
	pgmName = pwpgmname->getPgmNameArray();
	auto pwslider = new Slider(program);
	slider = pwslider->getSliderArray();
	auto pwmidinotes = new MidiNotes(program, sn->getSnConvTable());
	midiNotes = pwmidinotes->midiNotesArray;
	auto pwmixer = new Mixer(program);
	mixer = pwmixer->getMixerArray();
	auto pwpads = new Pads(program);
	pads = pwpads->getPadsArray();
}

vector<char> PgmWriter::get()
{
	auto caa = vector<vector<char>>(7);
	caa[0] = header;
	caa[1] = sampleNames;
	caa[2] = pgmName;
	caa[3] = slider;
	caa[4] = midiNotes;
	caa[5] = mixer;
	caa[6] = pads;
	auto programFileSize = 0;
	for (auto ca : caa)
		programFileSize += ca.size();

	auto bb = vector<char>(programFileSize);
	int counter = 0;
	for (auto ca : caa) {
		for (auto c : ca) {
			bb[counter++] = c;
		}
	}
	return bb;
}
