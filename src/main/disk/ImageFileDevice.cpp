#include "ImageFileDevice.hpp"
#include <mpc_fs.hpp>
#include <cerrno>
#include <stdexcept>
#include <algorithm>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <share.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#endif
using namespace mpc::disk;

namespace
{
    void closeDescriptor(int fd)
    {
#ifdef _WIN32
        _close(fd);
#else
        ::close(fd);
#endif
    }
} // namespace

std::shared_ptr<ImageFileDevice>
ImageFileDevice::open(const std::string &path, bool readOnly,
                      std::shared_ptr<void> access)
{
    int fd = -1;
#ifdef _WIN32
    _wsopen_s(&fd, mpc_fs::path(path).wstring().c_str(),
              (readOnly ? _O_RDONLY : _O_RDWR) | _O_BINARY,
              readOnly ? _SH_DENYWR : _SH_DENYRW, 0);
#else
    fd = ::open(path.c_str(), (readOnly ? O_RDONLY : O_RDWR) | O_CLOEXEC);
#endif
    if (fd < 0)
    {
        throw std::runtime_error("Cannot open image with requested access");
    }
    return adoptDescriptor(fd, readOnly, std::move(access));
}

std::shared_ptr<ImageFileDevice>
ImageFileDevice::adoptDescriptor(int fd, bool readOnly,
                                 std::shared_ptr<void> access)
{
    std::unique_ptr<ImageFileDevice> device;
    try
    {
        device.reset(new ImageFileDevice(fd, readOnly, std::move(access)));
    }
    catch (...)
    {
        if (fd >= 0)
        {
            closeDescriptor(fd);
        }
        throw;
    }
    return std::shared_ptr<ImageFileDevice>(std::move(device));
}

ImageFileDevice::ImageFileDevice(int fd, bool ro, std::shared_ptr<void> lease)
    : access(std::move(lease)), descriptor(fd), readOnly(ro)
{
#ifdef _WIN32
    size = _filelengthi64(fd);
#else
    struct stat info{};
    if (fstat(fd, &info) != 0 || !S_ISREG(info.st_mode) ||
        lseek(fd, 0, SEEK_SET) < 0)
    {
        throw std::runtime_error(
            "Image provider does not support direct random access");
    }
    size = info.st_size;
    if (flock(fd, (ro ? LOCK_SH : LOCK_EX) | LOCK_NB) != 0)
    {
        throw std::runtime_error("Image is in use or cannot be locked");
    }
#endif
    if (size < 512 || size % 512 != 0)
    {
        throw std::runtime_error(
            "Image must contain complete 512-byte sectors");
    }
}
ImageFileDevice::~ImageFileDevice()
{
    close();
}
bool ImageFileDevice::isClosed()
{
    return descriptor < 0;
}
bool ImageFileDevice::isReadOnly()
{
    return readOnly;
}
std::int64_t ImageFileDevice::getSize()
{
    return size;
}
std::int32_t ImageFileDevice::getSectorSize()
{
    return 512;
}
void ImageFileDevice::checkRange(std::int64_t offset, std::int64_t count)
{
    if (isClosed())
    {
        throw std::runtime_error("Image is closed");
    }
    if (offset < 0 || count < 0 || offset > size || count > size - offset)
    {
        throw std::runtime_error("Image access exceeds file bounds");
    }
}
void ImageFileDevice::read(std::int64_t offset, akaifat::ByteBuffer &buffer)
{
    auto remaining = buffer.remaining();
    checkRange(offset, remaining);
    while (remaining > 0)
    {
        const auto count = static_cast<unsigned>(
            std::min<std::int64_t>(remaining, 1024 * 1024));
        auto *data = buffer.getBuffer().data() + buffer.position();
#ifdef _WIN32
        if (_lseeki64(descriptor, offset, SEEK_SET) != offset)
        {
            throw std::runtime_error("Image seek failed");
        }
        const auto n = _read(descriptor, data, count);
#else
        const auto n = pread(descriptor, data, count, offset);
#endif
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            throw std::runtime_error("Image read failed or file was truncated");
        }
        buffer.position(buffer.position() + static_cast<int>(n));
        remaining -= n;
        offset += n;
    }
}
void ImageFileDevice::write(std::int64_t offset, akaifat::ByteBuffer &buffer)
{
    if (readOnly)
    {
        throw std::runtime_error("Image is read only");
    }
    auto remaining = buffer.remaining();
    checkRange(offset, remaining);
#ifdef _WIN32
    if (_filelengthi64(descriptor) != size)
#else
    struct stat info{};
    if (fstat(descriptor, &info) != 0 || info.st_size != size)
#endif
        throw std::runtime_error("Image size changed while mounted");

    while (remaining > 0)
    {
        const auto count = static_cast<unsigned>(
            std::min<std::int64_t>(remaining, 1024 * 1024));
        auto *data = buffer.getBuffer().data() + buffer.position();
#ifdef _WIN32
        if (_lseeki64(descriptor, offset, SEEK_SET) != offset)
        {
            throw std::runtime_error("Image seek failed");
        }
        const auto n = _write(descriptor, data, count);
#else
        const auto n = pwrite(descriptor, data, count, offset);
#endif
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            throw std::runtime_error("Image write failed");
        }
        buffer.position(buffer.position() + static_cast<int>(n));
        remaining -= n;
        offset += n;
    }
}
void ImageFileDevice::flush()
{
    checkRange(0, 0);
    if (readOnly)
    {
        return;
    }
#ifdef _WIN32
    const auto result = _commit(descriptor);
#else
    const auto result = fsync(descriptor);
#endif
    if (result != 0)
    {
        throw std::runtime_error("Image flush failed");
    }
}
void ImageFileDevice::close()
{
    if (!isClosed())
    {
        closeDescriptor(descriptor);
        descriptor = -1;
    }
    access.reset();
}
