#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc::controls::sampler {

	class LoopControls
		: public AbstractSamplerControls
	{

	public:
		void init() override;

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
		void setSlider(int i) override;

	};
}
