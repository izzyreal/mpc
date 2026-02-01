#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SaveAllFileScreen final : public ScreenComponent,
                                    public OpensNameScreen
    {

    public:
        SaveAllFileScreen(Mpc &mpc, int layerIndex);

        void function(int i) override;
        void openNameScreen() override;
        void open() override;

        std::string getFileName() const;

        void setFileName(const std::string &);

    private:
        void displayFile() const;
        std::string fileName = "ALL_SEQ_SONG1";
    };
} // namespace mpc::lcdgui::screens::window
