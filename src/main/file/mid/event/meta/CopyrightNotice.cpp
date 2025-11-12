#include "file/mid/event/meta/CopyrightNotice.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

CopyrightNotice::CopyrightNotice(int tick, int delta, const std::string &text)
    : TextualMetaEvent(tick, delta, COPYRIGHT_NOTICE, text)
{
}

void CopyrightNotice::setNotice(const std::string &t)
{
    setText(t);
}

std::string CopyrightNotice::getNotice()
{
    return getText();
}
