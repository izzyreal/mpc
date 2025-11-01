#include "file/mid/event/meta/CopyrightNotice.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

CopyrightNotice::CopyrightNotice(int tick, int delta, std::string text)
    : TextualMetaEvent(tick, delta, MetaEvent::COPYRIGHT_NOTICE, text)
{
}

void CopyrightNotice::setNotice(std::string t)
{
    setText(t);
}

std::string CopyrightNotice::getNotice()
{
    return getText();
}
