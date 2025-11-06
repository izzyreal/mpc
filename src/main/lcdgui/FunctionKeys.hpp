#pragma once

#include "lcdgui/Component.hpp"

#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class FunctionKey : public Component
    {

    public:
        FunctionKey(mpc::Mpc &mpc, const std::string &name, int xPos);
        void Draw(std::vector<std::vector<bool>> *pixels) override;
        int type = -1;
        void setText(const std::string &text);
        void setType(int newType);
    };

    class FunctionKeys : public Component
    {

    private:
        const std::vector<int> xPoses{2, 43, 84, 125, 166, 207};
        int activeArrangement = -1;
        std::vector<std::vector<std::string>> texts;
        std::vector<std::vector<int>> types;

    public:
        void setActiveArrangement(int i);
        FunctionKeys(mpc::Mpc &mpc, const std::string &name,
                     const std::vector<std::vector<std::string>> &texts,
                     const std::vector<std::vector<int>> &types);
    };
} // namespace mpc::lcdgui
