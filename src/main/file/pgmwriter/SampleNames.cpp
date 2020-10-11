#include <file/pgmwriter/SampleNames.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <set>

using namespace mpc::file::pgmwriter;
using namespace std;

SampleNames::SampleNames(mpc::sampler::Program* program, weak_ptr<mpc::sampler::Sampler> sampler)
{
	vector<int> list;
	for (auto& nn : program->getNotesParameters()) {
		if (nn->getSoundIndex() != -1)
			list.push_back(nn->getSoundIndex());
	}
	//Collections::sort(list);
	set<string> sampleNamesSet;
	vector<string> finalNames;
	auto lSampler = sampler.lock();
	for (int i = 0; i < list.size(); i++) {
		if (sampleNamesSet.emplace(lSampler->getSoundName(list[i])).second) {
			finalNames.push_back(lSampler->getSoundName(list[i]));
		}
	}

	for (int i = 0; i < lSampler->getSoundCount(); i++) {
		int j = -1;
		for (int k = 0; k < finalNames.size(); k++)
			if (finalNames[k].compare(lSampler->getSoundName(i)) == 0) {
				j = k;
				break;
			}

		snConvTable.push_back(j);
	}
	numberOfSamples = finalNames.size();
	sampleNamesArray = vector<char>((numberOfSamples * 17) + 2);
	int counter = 0;
	for (auto&s : finalNames) {
		setSampleName(counter++, s);
	}
	sampleNamesArray[(int) (sampleNamesArray.size()) - 2] = 30;
	sampleNamesArray[(int)(sampleNamesArray.size()) - 1] = 0;
}

vector<char> SampleNames::getSampleNamesArray()
{
    return sampleNamesArray;
}

void SampleNames::setSampleName(int sampleNumber, string name)
{
	for (int i = 0; i < name.length(); i++)
		sampleNamesArray[i + (sampleNumber * 17)] = name[i];

	for (int i = name.length(); i < 16; i++)
		sampleNamesArray[i + (sampleNumber * 17)] = 32;

	sampleNamesArray[16 + (sampleNumber * 17)] = 0;
}

int SampleNames::getNumberOfSamples()
{
    return numberOfSamples;
}

vector<int> SampleNames::getSnConvTable()
{
    return snConvTable;
}
