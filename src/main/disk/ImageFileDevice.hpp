#pragma once
#include <BlockDevice.hpp>
#include <memory>
#include <string>

namespace mpc::disk
{
    // Seekable, locked access to an existing image. Never creates, truncates,
    // copies, or grows the selected file. The lease retains platform
    // permission.
    class ImageFileDevice final : public akaifat::BlockDevice
    {
    public:
        static std::shared_ptr<ImageFileDevice>
        open(const std::string &, bool, std::shared_ptr<void> = {});
        static std::shared_ptr<ImageFileDevice>
        adoptDescriptor(int, bool, std::shared_ptr<void> = {});
        ~ImageFileDevice();
        bool isClosed() override;
        bool isReadOnly() override;
        std::int64_t getSize() override;
        std::int32_t getSectorSize() override;
        void read(std::int64_t, akaifat::ByteBuffer &) override;
        void write(std::int64_t, akaifat::ByteBuffer &) override;
        void flush() override;
        void close() override;

    private:
        ImageFileDevice(int, bool, std::shared_ptr<void>);
        void checkRange(std::int64_t, std::int64_t);
        std::shared_ptr<void> access;
        int descriptor;
        bool readOnly;
        std::int64_t size = 0;
    };
} // namespace mpc::disk
