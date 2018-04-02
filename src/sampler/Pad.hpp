#pragma once

#include <observer/Observable.hpp>
#include <sampler/StereoMixerChannel.hpp>
#include <sampler/IndivFxMixerChannel.hpp>

#include <memory>

namespace mpc {

	class Mpc;

	namespace sampler {

		class Pad
			: public moduru::observer::Observable
		{

		private:
			mpc::Mpc* mpc;

		private:
			int note{ 0 };
			int number{ 0 };
			std::shared_ptr<StereoMixerChannel> stereoMixerChannel{};
			std::shared_ptr<IndivFxMixerChannel> indivFxMixerChannel{};

		public:
			void setNote(int i);
			int getNote();
			std::weak_ptr<StereoMixerChannel> getStereoMixerChannel();
			std::weak_ptr<IndivFxMixerChannel> getIndivFxMixerChannel();
			int getNumber();

		public:
			Pad(mpc::Mpc* mpc, int number);
			~Pad();

		};

	}
}
