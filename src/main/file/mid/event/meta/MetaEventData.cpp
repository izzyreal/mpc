#include "file/mid/event/meta/MetaEventData.hpp"

using namespace mpc::file::mid::event::meta;

MetaEventData::MetaEventData(std::stringstream &in)
{
    type = in.get();

    length = mpc::file::mid::util::VariableLengthInt(in);

    data = std::vector<char>(length.getValue());

    if (length.getValue() > 0)
    {
        in.read(&data[0], data.size());
    }
}
