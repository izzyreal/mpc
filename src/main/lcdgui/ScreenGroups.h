#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace mpc::lcdgui::screengroups {

// ---- Screen group definitions ----

inline const std::vector<std::string> screensThatOnlyAllowPlay{
    "song",
    "track-mute",
    "next-seq",
    "next-seq-pad",
    "vmpc-recording-finished"
};

inline const std::vector<std::string> allowCentralNoteAndPadUpdateScreens{
    "program-assign",
    "program-params",
    "velocity-modulation",
    "velo-env-filter",
    "velo-pitch",
    "mute-assign",
    "assignment-view",
    "keep-or-retry",
    "load-a-sound"
};

inline const std::vector<std::string> screensThatAllowPlayAndRecord{
    "sequencer",
    "select-drum",
    "select-mixer-drum",
    "program-assign",
    "program-params",
    "drum",
    "purge",
    "program",
    "create-new-program",
    "name",
    "delete-program",
    "delete-all-programs",
    "assignment-view",
    "initialize-pad-assign",
    "copy-note-parameters",
    "velocity-modulation",
    "velo-env-filter",
    "velo-pitch",
    "mute-assign",
    "trans",
    "mixer",
    "mixer-setup",
    "channel-settings"
};

inline const std::vector<std::string> samplerScreens{
    "create-new-program",
    "assignment-view",
    "auto-chromatic-assignment",
    "copy-note-parameters",
    "edit-sound",
    "end-fine",
    "init-pad-assign",
    "keep-or-retry",
    "loop-end-fine",
    "loop-to-fine",
    "mute-assign",
    "program",
    "start-fine",
    "velo-env-filter",
    "velo-pitch",
    "velocity-modulation",
    "zone-end-fine",
    "zone-start-fine",
    "drum",
    "loop",
    "mixer",
    "mixer-setup",
    "channel-settings",
    "program-assign",
    "program-params",
    "select-drum",
    "trim",
    "zone",
    "load-a-sound"
};

inline const std::vector<std::string> soundScreens{
    // layer 1
    "trim",
    "loop",
    "zone",
    "params",

    // layer 2
    "sound",
    "start-fine",
    "end-fine",
    "loop-to-fine",
    "loop-end-fine",
    "zone-start-fine",
    "zone-end-fine",
    "number-of-zones",
    "edit-sound",

    // layer 3
    "delete-sound",
    "delete-all-sound",
    "convert-sound",
    "stereo-to-mono",
    "mono-to-stereo",
    "resample"
};

// ---- Convenience helpers ----

inline bool isPlayOnlyScreen(const std::string& screenName) {
    return std::find(screensThatOnlyAllowPlay.begin(), screensThatOnlyAllowPlay.end(), screenName)
           != screensThatOnlyAllowPlay.end();
}

inline bool isCentralNoteAndPadUpdateScreen(const std::string& screenName) {
    return std::find(allowCentralNoteAndPadUpdateScreens.begin(), allowCentralNoteAndPadUpdateScreens.end(), screenName)
           != allowCentralNoteAndPadUpdateScreens.end();
}

inline bool isPlayAndRecordScreen(const std::string& screenName) {
    return std::find(screensThatAllowPlayAndRecord.begin(), screensThatAllowPlayAndRecord.end(), screenName)
           != screensThatAllowPlayAndRecord.end();
}

inline bool isPlayScreen(const std::string& screenName) {
    return isPlayOnlyScreen(screenName) || isPlayAndRecordScreen(screenName);
}

inline bool isSamplerScreen(const std::string& screenName) {
    return std::find(samplerScreens.begin(), samplerScreens.end(), screenName)
           != samplerScreens.end();
}

inline bool isSoundScreen(const std::string& screenName) {
    return std::find(soundScreens.begin(), soundScreens.end(), screenName)
           != soundScreens.end();
}

} // namespace mpc::lcdgui::screengroups
