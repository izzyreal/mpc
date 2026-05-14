#pragma once

#include <string>

namespace mpc::engine
{
    static constexpr int PreviewSoundPlayerStripIndex = 65;
    static constexpr int SoundRecorderInputStripIndex = 66;
    static constexpr int QuickPreviewStripIndex = 67;
    static constexpr int MetronomeStripIndex = 68;
    static constexpr int TotalMixerStripCount = 68;

    const std::string PreviewSoundPlayerStrip = std::to_string(PreviewSoundPlayerStripIndex);
    const std::string SoundRecorderInputStrip = std::to_string(SoundRecorderInputStripIndex);
    const std::string QuickPreviewStrip = std::to_string(QuickPreviewStripIndex);
    const std::string MetronomeStrip = std::to_string(MetronomeStripIndex);
}