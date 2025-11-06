#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class CopyrightNotice : public TextualMetaEvent
    {

    public:
        void setNotice(const std::string &t);
        std::string getNotice();

        CopyrightNotice(int tick, int delta, const std::string &text);
    };
} // namespace mpc::file::mid::event::meta
