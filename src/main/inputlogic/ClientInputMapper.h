#pragma once

#include <functional>
#include <array>
#include <unordered_map>
#include <string>
#include <optional>

namespace mpc::inputlogic {

struct ClientInput;

class ClientInputMapper {
public:
    using Callback = std::function<void(const ClientInput&)>;

    // Pad bindings
    void bindPadPress(int index, Callback callback);
    void bindPadRelease(int index, Callback callback);
    void bindPadAftertouch(int index, Callback callback);

    // Buttons
    void bindButtonPress(const std::string& label, Callback callback);
    void bindButtonRelease(const std::string& label, Callback callback);

    // Other controls
    void bindDataWheel(Callback callback); // single handler for data wheel turns
    void bindSlider(Callback callback);
    void bindPot(Callback callback);

    // Trigger an action (called by hardware components)
    void trigger(const ClientInput& action) const;

private:
    static constexpr int MAX_PADS = 64; // allow more than 16 for banks, but Initializer may only bind 16

    std::vector<std::optional<Callback>> padPressBindings;
    std::vector<std::optional<Callback>> padReleaseBindings;
    std::vector<std::optional<Callback>> padAftertouchBindings;

    std::unordered_map<std::string, Callback> buttonPressBindings;
    std::unordered_map<std::string, Callback> buttonReleaseBindings;

    std::optional<Callback> dataWheelBinding;
    std::optional<Callback> sliderBinding;
    std::optional<Callback> potBinding;
};

} // namespace mpc::inputlogic

