#pragma once
#include <observer/Observer.hpp>

#include <vector>
#include <memory>

namespace mpc::sampler {
	class PgmSlider;
	class Sampler;
	class Program;
}

namespace mpc::lcdgui {
	class Field;
}

namespace mpc::ui::sequencer {

	class AssignObserver
		: public moduru::observer::Observer
	{

	private:
		static std::vector<std::string> TYPE_NAMES;
		mpc::sampler::PgmSlider* slider{ nullptr };
		std::weak_ptr<mpc::lcdgui::Field> assignNoteField{};
		std::weak_ptr<mpc::lcdgui::Field> parameterField{};
		std::weak_ptr<mpc::lcdgui::Field> highRangeField{};
		std::weak_ptr<mpc::lcdgui::Field> lowRangeField{};
		std::weak_ptr<mpc::lcdgui::Field> assignNvField{};
		std::weak_ptr<mpc::sampler::Sampler> sampler{};
		std::weak_ptr<mpc::sampler::Program> program{};

	private:
		void displayAssignNote();
		void displayParameter();
		void displayHighRange();
		void displayLowRange();
		void displayAssignNv();

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	public:
		AssignObserver();
		~AssignObserver();
	};
}
