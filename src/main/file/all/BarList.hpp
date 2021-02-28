#pragma once

#include <vector>

namespace mpc::sequencer {
class Sequence;
}

namespace mpc::file::all {
class Bar;

class BarList
{
    
    
public:
    std::vector<Bar*> bars;
    std::vector<char> saveBytes;
    
public:
    std::vector<Bar*> getBars();
    
public:
    std::vector<char>& getBytes();
    BarList(const std::vector<char>& loadBytes);
    
public:
    BarList(mpc::sequencer::Sequence* seq);
    ~BarList();
};
}
