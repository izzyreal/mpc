#pragma once

#include <string>

namespace mpc::engine
{
    static constexpr int VoiceStripCount = 32;
    static constexpr int PreviewSoundPlayerStripIndex = 33;
    static constexpr int SoundRecorderInputStripIndex = 34;
    static constexpr int QuickPreviewStripIndex = 35;
    static constexpr int MetronomeStripIndex = 36;
    static constexpr int TotalMixerStripCount = 36;

    const std::string PreviewSoundPlayerStrip = std::to_string(PreviewSoundPlayerStripIndex);
    const std::string SoundRecorderInputStrip = std::to_string(SoundRecorderInputStripIndex);
    const std::string QuickPreviewStrip = std::to_string(QuickPreviewStripIndex);
    const std::string MetronomeStrip = std::to_string(MetronomeStripIndex);
}
