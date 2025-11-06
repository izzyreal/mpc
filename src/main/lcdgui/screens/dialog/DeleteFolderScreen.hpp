#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteFolderScreen final : public ScreenComponent
    {
        std::thread deleteFolderThread{};
        static void static_deleteFolder(void *this_p);
        void deleteFolder();

    public:
        void function(int i) override;

        DeleteFolderScreen(Mpc &mpc, int layerIndex);
        ~DeleteFolderScreen() override;
    };
} // namespace mpc::lcdgui::screens::dialog
