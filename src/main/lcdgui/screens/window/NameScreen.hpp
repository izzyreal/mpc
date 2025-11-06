#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <functional>

namespace mpc::lcdgui
{
    class Underline;
}

namespace mpc::lcdgui::screens::window
{
    class NameScreen final : public ScreenComponent
    {

    public:
        NameScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void left() override;
        void right() override;
        void turnWheel(int j) override;
        void function(int i) override;
        void pressEnter() override;
        void numpad() const {} // Block ways to leave the screen inadvertently

        void typeCharacter(char c);
        void backSpace();

        void initialize(
            const std::string &nameToEdit, unsigned char nameLimit,
            const std::function<void(std::string &)> &enterAction,
            const std::string &cancelScreen,
            const std::function<void()> &mainScreenAction = [] {});

        std::string getNameWithoutSpaces() const;
        void setEditing(bool b);

        void setNameToEdit(const std::string &newNameToEdit);

        std::function<void()> mainScreenAction = []() {};

    private:
        std::function<void(std::string &)> enterAction = [](std::string &) {};
        std::string cancelScreen;
        void drawUnderline();
        void initEditColors();
        void displayName() const;
        std::weak_ptr<Underline> findUnderline();

        void setNameLimit(int i);
        void setNameToEdit(const std::string &str, int i);

        void changeNameCharacter(int i, bool up);
        std::string nameToEdit;
        bool editing = false;
        int nameLimit = 0;
    };
} // namespace mpc::lcdgui::screens::window
