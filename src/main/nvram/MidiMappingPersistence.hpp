#pragma once

#include "Mpc.hpp"

#include <string>
#include <vector>

namespace mpc::nvram {

    class MidiMappingPersistence
    {
    public:
        static void load(mpc::Mpc& mpc);
        static void save(mpc::Mpc& mpc);

        struct Row {

            std::string label;
            bool isNote;
            char channel;
            char value;

            std::vector<char> toBytes() {
                std::vector<char> result;

                for (int i = 0; i < label.size(); i++)
                {
                    result.push_back(label[i]);
                }

                result.push_back(' ');

                result.push_back(isNote ? 1 : 0);
                result.push_back(channel);
                result.push_back(value);
                return result;
            }
        };
    };

}