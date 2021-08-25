#pragma once

#include <tl/expected.hpp>

#include <memory>

namespace mpc::disk {

class MpcFile;

struct mpc_io_error {
    std::string msg;
};

}

using file_or_error  = tl::expected<std::shared_ptr<mpc::disk::MpcFile>, mpc::disk::mpc_io_error>;
using void_or_error  = tl::expected<void, mpc::disk::mpc_io_error>;
using file_operation = std::function<file_or_error()>;
