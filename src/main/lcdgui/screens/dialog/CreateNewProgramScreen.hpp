#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class CreateNewProgramScreen final : public ScreenComponent,
                                         public OpensNameScreen
    {
    public:
        void function(int i) override;

        CreateNewProgramScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void openNameScreen() override;

    private:
        std::string newName;
        int midiProgramChange = 0;

        void setMidiProgramChange(int i);

        void displayNewName() const;
        void displayMidiProgramChange() const;

        friend class NameScreen;
    };
} // namespace mpc::lcdgui::screens::dialog
