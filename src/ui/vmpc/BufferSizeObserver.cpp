#include <ui/vmpc/BufferSizeObserver.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <lcdgui/Field.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

BufferSizeObserver::BufferSizeObserver(mpc::Mpc* mpc)
{
	ams = mpc->getAudioMidiServices().lock().get();
	ams->addObserver(this);
	framesField = mpc->getLayeredScreen().lock()->lookupField("frames");
	displayFrames();
}

void BufferSizeObserver::update(moduru::observer::Observable* o, std::any a)
{
	string param = std::any_cast<string>(a);
	if (param.compare("buffersize") == 0) {
		displayFrames();
	}
}


void BufferSizeObserver::displayFrames()
{
	auto frames = ams->getBufferSize();
	framesField.lock()->setText(to_string(frames));
}

BufferSizeObserver::~BufferSizeObserver() {
	ams->deleteObserver(this);
}
