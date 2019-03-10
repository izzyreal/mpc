#include <file/pgmwriter/PgmName.hpp>

#include <sampler/Program.hpp>

using namespace mpc::file::pgmwriter;
using namespace std;

PgmName::PgmName(mpc::sampler::Program* program)
{
	auto ca = program->getName();
	auto temp = vector<char>(17);
	for (int i = 0; i < ca.length(); i++)
		temp[i] = ca[i];

	for (int i = ca.length(); i < 16; i++)
		temp[i] = 32;

	temp[16] = 0;
	programNameArray = temp;
	sampleNamesSize = program->getNumberOfSamples() * 17;
}

vector<char> PgmName::getPgmNameArray()
{
    return programNameArray;
}
