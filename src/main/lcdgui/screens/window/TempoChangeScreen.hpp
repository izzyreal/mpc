#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <vector>

namespace mpc::lcdgui
{
	class HorizontalBar;
}

namespace mpc::sequence
{
	class TempoChangeEvent;
}

namespace mpc::lcdgui::screens::window
{

	class TempoChangeScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::vector<std::weak_ptr<mpc::lcdgui::HorizontalBar>> bars;
		std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;
		std::weak_ptr<mpc::lcdgui::Field> a0Field;
		std::weak_ptr<mpc::lcdgui::Field> a1Field;
		std::weak_ptr<mpc::lcdgui::Field> a2Field;
		std::weak_ptr<mpc::lcdgui::Field> b0Field;
		std::weak_ptr<mpc::lcdgui::Field> b1Field;
		std::weak_ptr<mpc::lcdgui::Field> b2Field;
		std::weak_ptr<mpc::lcdgui::Field> c0Field;
		std::weak_ptr<mpc::lcdgui::Field> c1Field;
		std::weak_ptr<mpc::lcdgui::Field> c2Field;
		std::weak_ptr<mpc::lcdgui::Field> d0Field;
		std::weak_ptr<mpc::lcdgui::Field> d1Field;
		std::weak_ptr<mpc::lcdgui::Field> d2Field;
		std::weak_ptr<mpc::lcdgui::Field> e0Field;
		std::weak_ptr<mpc::lcdgui::Field> e1Field;
		std::weak_ptr<mpc::lcdgui::Field> e2Field;
		std::weak_ptr<mpc::lcdgui::Field> f0Field;
		std::weak_ptr<mpc::lcdgui::Field> f1Field;
		std::weak_ptr<mpc::lcdgui::Field> f2Field;
		std::weak_ptr<mpc::lcdgui::Label> a0Label;
		std::weak_ptr<mpc::lcdgui::Label> a1Label;
		std::weak_ptr<mpc::lcdgui::Label> a2Label;
		std::weak_ptr<mpc::lcdgui::Label> b0Label;
		std::weak_ptr<mpc::lcdgui::Label> b1Label;
		std::weak_ptr<mpc::lcdgui::Label> b2Label;
		std::weak_ptr<mpc::lcdgui::Label> c0Label;
		std::weak_ptr<mpc::lcdgui::Label> c1Label;
		std::weak_ptr<mpc::lcdgui::Label> c2Label;
		std::weak_ptr<mpc::lcdgui::Label> d0Label;
		std::weak_ptr<mpc::lcdgui::Label> d1Label;
		std::weak_ptr<mpc::lcdgui::Label> d2Label;
		std::weak_ptr<mpc::lcdgui::Label> e0Label;
		std::weak_ptr<mpc::lcdgui::Label> e1Label;
		std::weak_ptr<mpc::lcdgui::Label> e2Label;
		std::weak_ptr<mpc::lcdgui::Label> f0Label;
		std::weak_ptr<mpc::lcdgui::Label> f1Label;
		std::weak_ptr<mpc::lcdgui::Label> f2Label;

	private:
		void initVisibleEvents();
		void displayTempoChange0();
		void displayTempoChange1();
		void displayTempoChange2();
		void displayTempoChangeOn();
		void displayInitialTempo();

	public:
		TempoChangeScreen(const int layerIndex);
	
	public:
		void open() override;
		void close() override;

	public:
		void left() override;
		void right() override;
		void function(int j) override;
		void init() override;
		void turnWheel(int j) override;
		void down() override;
		void up() override;

	private:
		std::weak_ptr<mpc::sequencer::TempoChangeEvent> previous;
		std::weak_ptr<mpc::sequencer::TempoChangeEvent> current;
		std::weak_ptr<mpc::sequencer::TempoChangeEvent> next;
		int offset = 0;
		void setTempoChangeOffset(int i);
	};
}
