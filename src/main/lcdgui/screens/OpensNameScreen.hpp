#pragma once

namespace mpc::lcdgui::screens
{
    class OpensNameScreen
    {
    public:
        virtual ~OpensNameScreen() = default;
        virtual void openNameScreen() = 0;
    };
} // namespace mpc::lcdgui::screens
