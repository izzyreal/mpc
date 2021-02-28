#pragma once

#include <vector>
#include <string>

namespace mpc::sequencer {
class Song;
}

namespace mpc::file::all {
class Song
{
    
public:
    static const int LENGTH{ 528 };
    
private:
    static const int NAME_OFFSET{ 0 };
    
public:
    std::string name;
    std::vector<char> saveBytes;
    
public:
    std::string getName();
    
public:
    std::vector<char>& getBytes();
    
    Song(const std::vector<char>& b);
    Song(mpc::sequencer::Song* mpcSong);
};
}
