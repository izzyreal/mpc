#pragma once

#include <kaitai/kaitaistream.h>

#include <sstream>
#include <string>
#include <vector>

namespace mpc::file::kaitai
{
    template <typename T>
    inline std::vector<char> parseRewrite(const std::vector<char> &bytes)
    {
        std::istringstream parseStream(
            std::string(bytes.begin(), bytes.end()),
            std::ios::binary
        );
        ::kaitai::kstream parseIo(&parseStream);
        T parsed(&parseIo);
        parsed._read();

        std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
        ::kaitai::kstream writeIo(&writeStream);
        parsed._set_io(&writeIo);
        parsed._check();
        parsed._write();

        const auto written = writeStream.str();
        return std::vector<char>(written.begin(), written.end());
    }
}
