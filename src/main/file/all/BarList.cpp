#include <file/all/BarList.hpp>

#include <file/all/Bar.hpp>
#include <sequencer/Sequence.hpp>

#include <Util.hpp>

using namespace mpc::file::all;

BarList::BarList(const std::vector<char>& loadBytes)
{
	Bar* previousBar = nullptr;
	
    for (int i = 0; i < 999; i++)
    {
		auto bar = new Bar(Util::vecCopyOfRange(loadBytes, i * 4, (i * 4) + 4), previousBar);
		
        if (bar->lastTick == 0)
        {
            delete bar;
            break;
        }
	
        bars.push_back(bar);
		previousBar = bar;
	}
}

BarList::~BarList()
{
    for (auto& b : bars)
        if (b != nullptr)
            delete b;
}

BarList::BarList(mpc::sequencer::Sequence* seq)
{
	saveBytes = std::vector<char>(3996);
	auto& barLengths = seq->getBarLengthsInTicks();
	auto ticksPerBeat = 0;
	auto lastTick = 0;
	
    for (int i = 0; i < seq->getLastBarIndex() + 1; i++)
    {
		lastTick += barLengths[i];
		ticksPerBeat = static_cast<int>(barLengths[i] / seq->getNumerator(i));
		
        Bar bar(ticksPerBeat, lastTick);
        
		for (auto j = 0; j < 4; j++)
			saveBytes[(i * 4) + j] = bar.getBytes()[j];

	}
	
    Bar bar(ticksPerBeat, 0);
    
	for (int i = 0; i < 4; i++)
		saveBytes[(seq->getLastBarIndex() + 1) * 4 + i] = bar.getBytes()[i];

}

std::vector<Bar*> BarList::getBars()
{
    return bars;
}

std::vector<char>& BarList::getBytes()
{
    return saveBytes;
}
