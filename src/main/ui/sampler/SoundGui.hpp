#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SoundGui
		: public moduru::observer::Observable
	{

	private:
		int view = 0;
		int playX = 0;

	public:
		void setPlayX(int i);
		int getPlayX();
		void setView(int i);
		int getView();

	};
}
