#pragma once
#include <observer/Observer.hpp>
#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/NameGui.hpp>

namespace mpc {
	namespace lcdgui {
		class Underline;
	}

	namespace ui {
		
		class NameObserver
			: public moduru::observer::Observer
		{

		private:
			mpc::Mpc* mpc;
			std::weak_ptr<mpc::lcdgui::Field> a0Field{};
			std::weak_ptr<mpc::lcdgui::Field> a1Field{};
			std::weak_ptr<mpc::lcdgui::Field> a2Field{};
			std::weak_ptr<mpc::lcdgui::Field> a3Field{};
			std::weak_ptr<mpc::lcdgui::Field> a4Field{};
			std::weak_ptr<mpc::lcdgui::Field> a5Field{};
			std::weak_ptr<mpc::lcdgui::Field> a6Field{};
			std::weak_ptr<mpc::lcdgui::Field> a7Field{};
			std::weak_ptr<mpc::lcdgui::Field> a8Field{};
			std::weak_ptr<mpc::lcdgui::Field> a9Field{};
			std::weak_ptr<mpc::lcdgui::Field> a10Field{};
			std::weak_ptr<mpc::lcdgui::Field> a11Field{};
			std::weak_ptr<mpc::lcdgui::Field> a12Field{};
			std::weak_ptr<mpc::lcdgui::Field> a13Field{};
			std::weak_ptr<mpc::lcdgui::Field> a14Field{};
			std::weak_ptr<mpc::lcdgui::Field> a15Field{};
			NameGui* nameGui{ nullptr };
			std::weak_ptr<mpc::lcdgui::Underline> underline;

		private:
			void displayName();

		public:
			void update(moduru::observer::Observable* o, std::any arg) override;

			NameObserver(mpc::Mpc* mpc);
			~NameObserver();

		};

	}
}
