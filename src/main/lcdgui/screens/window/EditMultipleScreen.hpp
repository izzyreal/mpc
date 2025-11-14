#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include <sequencer/NoteEvent.hpp>

using mpc::sequencer::NoteOnEvent;

namespace mpc::lcdgui::screens::window
{
    class EditMultipleScreen final : public ScreenComponent
    {
    public:
        EditMultipleScreen(Mpc &mpc, int layerIndex);

        void function(int i) override;
        void turnWheel(int increment) override;
        void open() override;
        void close() override;

        void setChangeNoteTo(NoteNumber);
        void setVariationType(NoteOnEvent::VARIATION_TYPE type);
        void setVariationValue(int i);
        void setEditValue(int i);

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

        NoteNumber changeNoteTo = MinDrumNoteNumber;
        NoteOnEvent::VARIATION_TYPE variationType =
            NoteOnEvent::VARIATION_TYPE::TUNE_0;
        int variationValue = 0;
        int editValue = 0;

        void checkThreeParameters() const;
        void checkFiveParameters();
        void checkNotes() const;

        void updateEditMultiple() const;
        void updateDouble() const;
        void setEditType(int i);

        void incrementVariationType(int i);
    };
} // namespace mpc::lcdgui::screens::window
