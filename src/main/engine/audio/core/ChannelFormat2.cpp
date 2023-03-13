#include <engine/audio/core/ChannelFormat2.hpp>

using namespace ctoot::audio::core;
using namespace std;

ChannelFormat2::ChannelFormat2()
{
}

int ChannelFormat2::getCount()
{
    return 2;
}

std::vector<int8_t> ChannelFormat2::getLeft()
{
    return left;
}

std::vector<int8_t> ChannelFormat2::getRight()
{
    return right;
}