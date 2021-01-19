#pragma once
#include <lcdgui/Component.hpp>

namespace mpc { class Mpc; }

namespace mpc::lcdgui::kbeditor {

	class KbEditor
		: public Component
	{

	public:
		void Draw(std::vector< std::vector<bool>>* pixels) override;
        KbEditor(mpc::Mpc&);

    private:
        mpc::Mpc& mpc;
        
        int row = 0;
        int rowOffset = 0;
        
        void updateRows();
        
	};
}
