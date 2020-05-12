#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

#include <memory>

namespace mpc::controls::sampler {

	class TrimControls
		: public AbstractSamplerControls
	{

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
		void setSlider(int i) override;

	public:
		TrimControls();

	};
}
