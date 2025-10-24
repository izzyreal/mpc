#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

#include <string>

namespace mpc
{
    namespace midi
    {
        namespace event
        {
            namespace meta
            {

                class Marker
                    : public TextualMetaEvent
                {

                public:
                    void setMarkerName(std::string name);
                    std::string getMarkerName();

                    Marker(int tick, int delta, std::string marker);
                };
            } // namespace meta
        } // namespace event
    } // namespace midi
} // namespace mpc
