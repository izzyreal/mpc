#pragma once

#include "Logger.hpp"
#include "mpc_fs.hpp"

#include <optional>
#include <string>

namespace mpc::file_io
{
    enum class FailurePolicy
    {
        Required,
        Recoverable,
        BestEffort,
    };

    inline std::string toString(const FailurePolicy policy)
    {
        switch (policy)
        {
            case FailurePolicy::Required:
                return "Required";
            case FailurePolicy::Recoverable:
                return "Recoverable";
            case FailurePolicy::BestEffort:
                return "Best-effort";
        }

        return "Unknown";
    }

    inline void logFailure(const FailurePolicy policy,
                           const std::string &context,
                           const mpc_fs::fs_error &error)
    {
        std::string msg =
            toString(policy) + " file I/O failed during " + context +
            " (operation=" + error.operation;

        if (!error.path1.empty())
        {
            msg += ", path=" + error.path1.string();
        }
        if (!error.path2.empty())
        {
            msg += ", path2=" + error.path2.string();
        }
        if (!error.message.empty())
        {
            msg += ", message=" + error.message;
        }

        msg += ")";
        MLOG(msg);
    }

    inline bool success(const mpc_fs::result<void> &result,
                        const FailurePolicy policy,
                        const std::string &context)
    {
        if (result)
        {
            return true;
        }

        logFailure(policy, context, result.error());
        return false;
    }

    inline bool success(const mpc_fs::result<bool> &result,
                        const FailurePolicy policy,
                        const std::string &context)
    {
        if (result)
        {
            return *result;
        }

        logFailure(policy, context, result.error());
        return false;
    }

    template <typename T>
    inline std::optional<T> value(const mpc_fs::result<T> &result,
                                  const FailurePolicy policy,
                                  const std::string &context)
    {
        if (result)
        {
            return *result;
        }

        logFailure(policy, context, result.error());
        return std::nullopt;
    }
} // namespace mpc::file_io
