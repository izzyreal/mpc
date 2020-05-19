#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <vector>
#include <memory>

namespace mpc::sequencer
{
	class Event;
}

namespace mpc::lcdgui::screens
{
	class StepEditorScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::vector<std::weak_ptr<mpc::sequencer::Event>> visibleEvents{};

	private:
		void downOrUp(int increment);

	public:
		void init() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;
		void prevStepEvent() override;
		void nextStepEvent() override;
		void prevBarStart() override;
		void nextBarEnd() override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void shift() override;

		StepEditorScreen(const int& layer);

		void open() override;

	private:
		void refreshSelection();
		void initVisibleEvents();

	public:
		void refreshEventRows();
		void refreshViewModeNotes();
		void setViewModeNotesText();

	};
}
