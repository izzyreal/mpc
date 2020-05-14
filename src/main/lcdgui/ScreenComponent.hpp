#pragma once

#include "Component.hpp"
#include "Field.hpp"

#include <controls/BaseControls.hpp>

#include <observer/Observer.hpp>

#include <memory>
#include <string>

namespace mpc
{
	class Mpc;
}

namespace mpc::sampler
{
	class Sampler;
}

namespace mpc::sequencer
{
	class Sequencer;
}

namespace mpc::lcdgui
{

	class ScreenComponent
		: public Component, public moduru::observer::Observer, public mpc::controls::BaseControls
	{

	private:
		const int layer;

	protected:
		mpc::Mpc& mpc;
		const std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
		const std::shared_ptr<mpc::sampler::Sampler> sampler;

	public:
		const int& getLayer();

	protected:
		std::weak_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);

	public:
		virtual void open() {}
			virtual void close() {}

	public:
		ScreenComponent(const std::string& name, const int layer);

	};

}
