#include <engine/audio/core/Encoding.hpp>

using namespace ctoot::audio::core;
using namespace std;

Encoding::Encoding(string name)
{
	this->name = name;
}

Encoding*& Encoding::PCM_SIGNED()
{
	return PCM_SIGNED_;
}
Encoding* Encoding::PCM_SIGNED_ = new Encoding("pcm_signed");

Encoding*& Encoding::PCM_UNSIGNED()
{
	return PCM_UNSIGNED_;
}
Encoding* Encoding::PCM_UNSIGNED_ = new Encoding("pcm_unsigned");
