#pragma once
#include <disk/Volume.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

namespace akaifat
{
    class BlockDevice;
}
namespace mpc::platform
{
    struct FileSelection
    {
        std::optional<disk::Volume>
            image; // Empty with no error means cancelled.
        std::string error;
    };
    struct PickerRequest
    {
        std::atomic<bool> cancelled{false};
        std::mutex mutex;
        std::optional<FileSelection> result;
        std::function<void()> dismiss;
        std::thread worker;
        void finish(FileSelection);
        void setDismiss(std::function<void()>);
        void cancel();
        std::optional<FileSelection> takeResult();
    };
    // Native presentation is optional on desktop. No UI or platform access is
    // initialized until request() is called. Methods belong to the UI owner.
    class NativeFilePicker
    {
    public:
        NativeFilePicker() = default;
        ~NativeFilePicker();
        NativeFilePicker(const NativeFilePicker &) = delete;
        NativeFilePicker &operator=(const NativeFilePicker &) = delete;
        void setParent(void *nativeParent, std::string portalParent = {});
        bool request();
        bool pending() const;
        void cancel();
        std::optional<FileSelection> poll();

    private:
        void *parent = nullptr; // HWND, NSWindow*, or UIView* on iOS.
        std::string portalParent;
        std::shared_ptr<PickerRequest> current;
    };
    disk::Volume selectedImage(std::string path, std::string label = {},
                               std::string access = {});
    void showNativeFilePicker(std::shared_ptr<PickerRequest>, void *,
                              const std::string &);
    std::shared_ptr<akaifat::BlockDevice> openNativeImage(const disk::Volume &,
                                                          bool readOnly);
} // namespace mpc::platform
