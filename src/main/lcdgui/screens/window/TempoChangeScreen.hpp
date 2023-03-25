#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <vector>

namespace mpc::lcdgui
{
	class HorizontalBar2;
}

namespace mpc::sequencer
{
	class TempoChangeEvent;
}

namespace mpc::lcdgui::screens::window
{

	class TempoChangeScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::vector<std::shared_ptr<mpc::lcdgui::HorizontalBar2>> bars;
		std::vector<std::shared_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;
		std::shared_ptr<mpc::lcdgui::Field> a0Field;
		std::shared_ptr<mpc::lcdgui::Field> a1Field;
		std::shared_ptr<mpc::lcdgui::Field> a2Field;
		std::shared_ptr<mpc::lcdgui::Field> b0Field;
		std::shared_ptr<mpc::lcdgui::Field> b1Field;
		std::shared_ptr<mpc::lcdgui::Field> b2Field;
		std::shared_ptr<mpc::lcdgui::Field> c0Field;
		std::shared_ptr<mpc::lcdgui::Field> c1Field;
		std::shared_ptr<mpc::lcdgui::Field> c2Field;
		std::shared_ptr<mpc::lcdgui::Field> d0Field;
		std::shared_ptr<mpc::lcdgui::Field> d1Field;
		std::shared_ptr<mpc::lcdgui::Field> d2Field;
		std::shared_ptr<mpc::lcdgui::Field> e0Field;
		std::shared_ptr<mpc::lcdgui::Field> e1Field;
		std::shared_ptr<mpc::lcdgui::Field> e2Field;
		std::shared_ptr<mpc::lcdgui::Field> f0Field;
		std::shared_ptr<mpc::lcdgui::Field> f1Field;
		std::shared_ptr<mpc::lcdgui::Field> f2Field;
		std::shared_ptr<mpc::lcdgui::Label> b1Label;
		std::shared_ptr<mpc::lcdgui::Label> b2Label;
		std::shared_ptr<mpc::lcdgui::Label> c1Label;
		std::shared_ptr<mpc::lcdgui::Label> c2Label;
		std::shared_ptr<mpc::lcdgui::Label> d1Label;
		std::shared_ptr<mpc::lcdgui::Label> d2Label;
		std::shared_ptr<mpc::lcdgui::Label> e1Label;
		std::shared_ptr<mpc::lcdgui::Label> e2Label;
		std::shared_ptr<mpc::lcdgui::Label> f1Label;
		std::shared_ptr<mpc::lcdgui::Label> f2Label;

	private:
		void initVisibleEvents();
		void displayTempoChange0();
		void displayTempoChange1();
		void displayTempoChange2();
		void displayTempoChangeOn();
		void displayInitialTempo();

	public:
		TempoChangeScreen(mpc::Mpc& mpc, const int layerIndex);
	
	public:
		void open() override;

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
		void setOffset(int i);
	};
}
