#include "file/pgmwriter/PgmName.hpp"

#include "sampler/Program.hpp"

using namespace mpc::file::pgmwriter;

PgmName::PgmName(sampler::Program *program)
{
    auto ca = program->getName();
    std::vector<char> temp(17);
    for (int i = 0; i < ca.length(); i++)
    {
        temp[i] = ca[i];
    }

    for (int i = ca.length(); i < 16; i++)
    {
        temp[i] = 32;
    }

    temp[16] = 0;
    programNameArray = temp;
    sampleNamesSize = program->getNumberOfSamples() * 17;
}

std::vector<char> PgmName::getPgmNameArray()
{
    return programNameArray;
}
