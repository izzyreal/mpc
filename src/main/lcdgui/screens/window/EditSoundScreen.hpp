#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/OpensNameScreen.hpp>

namespace mpc::lcdgui::screens::window
{

    class EditSoundScreen : public mpc::lcdgui::ScreenComponent,
                            public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        EditSoundScreen(mpc::Mpc &mpc, int layerIndex);
        void turnWheel(int j) override;
        void function(int j) override;
        void open() override;
        void right() override;

        void setNewName(const std::string &s);

        void setReturnToScreenName(const std::string &s);
        std::string getReturnToScreenName();

    private:
        const std::vector<std::string> editNames{
            "DISCARD",
            "LOOP FROM ST TO END",
            u8"SECTION \u00C4 NEW SOUND",
            u8"INSERT SOUND \u00C4 SECTION START",
            "DELETE SECTION",
            "SILENCE SECTION",
            "REVERSE SECTION",
            "TIME STRETCH",
            "NORMALIZE SECTION",
            "SLICE SOUND"};
        std::vector<std::string> timeStretchPresetNames{
            "FEM VOX",      "MALE VOX",     "LOW MALE VOX", "VOCAL",
            "HFREQ RHYTHM", "MFREQ RHYTHM", "LFREQ RHYTHM", "PERCUSSION",
            "LFREQ PERC.",  "STACCATO",     "LFREQ SLOW",   "MUSIC 1",
            "MUSIC 2",      "MUSIC 3",      "SOFT PERC.",   "HFREQ ORCH.",
            "LFREQ ORCH.",  "SLOW ORCH."};

        void displayEdit();
        void displayCreateNewProgram();
        void displayEndMargin();
        void displayVariable();
        void openNameScreen() override;

        int edit = 0;
        int insertSoundIndex = 0;
        int timeStretchRatio = 10000;
        int timeStretchPresetIndex = 0;
        int timeStretchAdjust = 0;
        int endMargin = 30;
        bool createNewProgram = true;
        std::string newName;
        std::string returnToScreenName;

        void setEdit(int i);
        void setInsertSndNr(int i, int soundCount);
        void setTimeStretchRatio(int i);
        void setTimeStretchPresetNumber(int i);
        void setTimeStretchAdjust(int i);
        void setCreateNewProgram(bool b);
        void setEndMargin(int i);
    };
} // namespace mpc::lcdgui::screens::window
