#pragma once
#include <observer/Observer.hpp>

#include <thread>
#include <memory>

namespace mpc {
	class Mpc;
	namespace audiomidi {
		class AudioMidiServices;
	}

	namespace lcdgui {
		class Field;
	}

	namespace ui {

		namespace vmpc {

			class BufferSizeObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::lcdgui::Field> framesField{};
				mpc::audiomidi::AudioMidiServices* ams{ nullptr };

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

			private:
				void displayFrames();

			public:
				BufferSizeObserver(mpc::Mpc* mpc);
				~BufferSizeObserver();
			};

		}
	}
}
