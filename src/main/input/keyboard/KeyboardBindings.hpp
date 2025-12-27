#pragma once

#include <vector>

#include "input/keyboard/KeyBinding.hpp"

#include "hardware/ComponentId.hpp"

#include <nlohmann/json.hpp>

namespace mpc::input::keyboard
{
    inline constexpr long long CURRENT_KEYBOARD_BINDINGS_VERSION = 3;

    using KeyboardBindingsData = std::vector<KeyBinding>;

    class KeyboardBindings
    {
    public:
        KeyboardBindings();

        explicit KeyboardBindings(const KeyboardBindingsData &);

        void ensureCorrectAmountOfBindingsPerComponentId();
        void deduplicateBindings();

        bool isSameAs(const KeyboardBindingsData &other) const;

        bool hasNoDuplicateVmpcKeyCodes() const;

        bool hasNoDuplicateKeyBindings() const;

        bool hasNoDuplicates() const;

        std::vector<VmpcKeyCode>
            lookupComponentKeyCodes(hardware::ComponentId) const;

        std::vector<KeyBinding *> lookupKeyCodeBindings(VmpcKeyCode);
        KeyBinding *lookupFirstKeyCodeBinding(VmpcKeyCode);

        void updateBindingKeyCode(int bindingIndex, VmpcKeyCode);

        void initializeDefaults();

        const KeyboardBindingsData &getKeyboardBindingsData() const;

        KeyBinding *getByIndex(int);

        void setBindingsData(const KeyboardBindingsData &);

        int getBindingCount() const;

        void validateBindings();

    private:
        KeyboardBindingsData bindings;
        std::vector<VmpcKeyCode>
        lookupComponentKeyCodes(const std::string &) const;
        std::vector<KeyBinding *> lookupComponentBindings(const std::string &);
    };

} // namespace mpc::input::keyboard
