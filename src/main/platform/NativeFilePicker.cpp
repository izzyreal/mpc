#include "NativeFilePicker.hpp"
#include <disk/ImageFileDevice.hpp>
#include <mpc_fs.hpp>
#include <random>
#include <sstream>
#include <utility>

using namespace mpc::platform;
void PickerRequest::finish(FileSelection value)
{
    std::lock_guard lock(mutex);
    if (!cancelled && !result)
    {
        result = std::move(value);
    }
    dismiss = {};
}
void PickerRequest::setDismiss(std::function<void()> action)
{
    bool stop;
    {
        std::lock_guard lock(mutex);
        stop = cancelled;
        if (!stop)
        {
            dismiss = action;
        }
    }
    if (stop)
    {
        action();
    }
}
void PickerRequest::cancel()
{
    cancelled = true;
    std::function<void()> action;
    {
        std::lock_guard lock(mutex);
        action = std::exchange(dismiss, {});
    }
    if (action)
    {
        action();
    }
    if (worker.joinable())
    {
        worker.join();
    }
}
std::optional<FileSelection> PickerRequest::takeResult()
{
    std::lock_guard lock(mutex);
    auto value = std::move(result);
    result.reset();
    return value;
}
NativeFilePicker::~NativeFilePicker()
{
    cancel();
}
void NativeFilePicker::setParent(void *p, std::string portal)
{
    if (p == parent && portal == portalParent)
    {
        return;
    }
    cancel();
    parent = p;
    portalParent = std::move(portal);
}
bool NativeFilePicker::request()
{
    if (current)
    {
        return false;
    }
    current = std::make_shared<PickerRequest>();
    try
    {
        showNativeFilePicker(current, parent, portalParent);
    }
    catch (const std::exception &e)
    {
        current->finish({{}, e.what()});
    }
    return true;
}
bool NativeFilePicker::pending() const
{
    return current != nullptr;
}
void NativeFilePicker::cancel()
{
    if (current)
    {
        current->cancel();
    }
    current.reset();
}
std::optional<FileSelection> NativeFilePicker::poll()
{
    if (!current)
    {
        return {};
    }
    auto value = current->takeResult();
    if (value)
    {
        current->cancel();
        current.reset();
    }
    return value;
}
mpc::disk::Volume mpc::platform::selectedImage(std::string path,
                                               std::string label,
                                               std::string access)
{
    disk::Volume v;
    v.type = disk::DISK_IMAGE;
    v.mode = disk::READ_ONLY;
    std::random_device random;
    std::ostringstream id;
    id << "image-" << std::hex << random() << '-' << random() << '-' << random()
       << '-' << random();
    v.volumeUUID = id.str();
    v.label = label.empty() ? mpc_fs::path(path).filename().string()
                            : std::move(label);
    v.diskImagePath = std::move(path);
    v.diskImageAccessToken = std::move(access);
    return v;
}
#if !defined(__APPLE__) && !defined(__ANDROID__)
std::shared_ptr<akaifat::BlockDevice>
mpc::platform::openNativeImage(const disk::Volume &v, bool ro)
{
    return disk::ImageFileDevice::open(v.diskImagePath, ro);
}
#endif
