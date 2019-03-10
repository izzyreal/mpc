#include <controls/vmpc/BufferSizeControls.hpp>

#include <audiomidi/AudioMidiServices.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

BufferSizeControls::BufferSizeControls(mpc::Mpc* mpc)
	: AbstractVmpcControls(mpc)
{
	reset = -1;
}

void BufferSizeControls::function(int i)
{
	init();
	switch (i) {
	case 3:
//		if (reset != -1) {
//			ams.lock()->setDriverIndex(reset);
//		}
		ls.lock()->openScreen("audio");
		break;
	}
}

void BufferSizeControls::turnWheel(int i) {
	{
		init();
		auto lAms = ams.lock();
		if (param.compare("frames") == 0) {
			/*
			if (lAms->isDirectSound()) {
				auto candidate = lAms->getBufferSize() + i;
				if (candidate < 512) {
					candidate = 512;
				}
				else if (candidate > 8192) {
					candidate = 8192;
				}
				if (candidate != lAms->getBufferSize()) {
					//lAms->setBufferSize(candidate);
				}
			}
			else if (lAms->isAsio()) {
				const int sizes[9] = { 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
				int size = lAms->getBufferSize();
				int index = -1;
				for (int i = 0; i < 9; i++) {
					if (sizes[i] == size) {
						index = i;
						break;
					}
				}
				if (index == -1) return;
				if (i > 0) {
					index++;
				}
				else {
					index--;
				}
				if (index < 0 || index > 8) return;
				//lAms->setBufferSize(sizes[index]);
			}
			else if (lAms->isCoreAudio()) {
				auto candidate = lAms->getBufferSize() + i;
				if (candidate < 32) {
					candidate = 32;
				}
				else if (candidate > 2048) {
					candidate = 2048;
				}
				if (candidate != lAms->getBufferSize()) {
					//lAms->setBufferSize(candidate);
				}
			}
			*/
		}
	}
}
