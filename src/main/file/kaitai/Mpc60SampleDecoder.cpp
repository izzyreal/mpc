#include "file/kaitai/Mpc60SampleDecoder.hpp"

int16_t mpc::file::kaitai::Mpc60SampleDecoder::decodeImportedPcm(
    uint16_t, bool)
{
    return 0;
}

float mpc::file::kaitai::Mpc60SampleDecoder::decodeImportedFloat(
    uint16_t, bool)
{
    return 0.0f;
}
