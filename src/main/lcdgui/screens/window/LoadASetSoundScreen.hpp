#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <optional>
#include <vector>

namespace mpc::lcdgui::screens::window
{
    class LoadASetSoundScreen final : public ScreenComponent
    {
    public:
        LoadASetSoundScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        std::vector<size_t> assignedMpc60PadIndexes;
        size_t selectedAssignedPadIndex = 0;

        void displayMpc60Pad() const;
        void displayFile() const;
        std::optional<size_t> getSelectedSoundDirectoryEntryIndex() const;
    };
} // namespace mpc::lcdgui::screens::window
