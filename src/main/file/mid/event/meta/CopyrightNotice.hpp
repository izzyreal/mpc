#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class CopyrightNotice : public TextualMetaEvent
    {

    public:
        void setNotice(std::string t);
        std::string getNotice();

        CopyrightNotice(int tick, int delta, std::string text);
    };
} // namespace mpc::file::mid::event::meta
