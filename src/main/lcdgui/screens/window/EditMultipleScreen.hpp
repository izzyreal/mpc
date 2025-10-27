#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <sequencer/NoteEvent.hpp>

namespace mpc::lcdgui::screens
{
    class StepEditorScreen;
}
using mpc::sequencer::NoteOnEvent;

namespace mpc::lcdgui::screens::window
{

    class EditMultipleScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        EditMultipleScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;
        void close() override;

        void setChangeNoteTo(int i);

    private:
        int editType = 0;
        std::vector<std::string> noteVariationParameterNames = {"Tun", "Dcy",
                                                                "Atk", "Flt"};
        std::vector<std::string> editTypeNames = {"ADD VALUE", "SUB VALUE",
                                                  "MULT VAL%", "SET TO VAL"};
        std::vector<std::string> singleLabels = {
            "Change note to:", "Variation type:", "Variation value:"};
        const int xPosSingle = 60;
        const int yPosSingle = 25;
        std::vector<int> xPosDouble = {60, 84};
        std::vector<int> yPosDouble = {22, 33};
        std::vector<std::string> doubleLabels = {"Edit type:", "Value:"};

        int changeNoteTo = 35;
        NoteOnEvent::VARIATION_TYPE variationType =
            NoteOnEvent::VARIATION_TYPE::TUNE_0;
        int variationValue = 0;
        int editValue = 0;

        void checkThreeParameters();
        void checkFiveParameters();
        void checkNotes();

        void updateEditMultiple();
        void updateDouble();
        void setEditType(int i);

        void setVariationType(NoteOnEvent::VARIATION_TYPE type);
        void incrementVariationType(int i);
        void setVariationValue(int i);
        void setEditValue(int i);

        friend class mpc::lcdgui::screens::StepEditorScreen;
    };
} // namespace mpc::lcdgui::screens::window
