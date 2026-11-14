#pragma once
#include "concurrency/FreeList.hpp"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <utility>

namespace mpc::sequencer
{
    class SysExPool;

    // Bytes are filled once, before the first reference is published.
    struct SysExPayload
    {
    private:
        uint8_t bytes[512];
        friend class SysExPool;

    public:
        SysExPayload(const uint8_t *source, size_t count, SysExPool *owner)
            : byteCount(static_cast<uint16_t>(count)), pool(owner)
        {
            std::memcpy(bytes, source, count);
        }
        const uint8_t *data() const { return bytes; }
        const uint16_t byteCount;
        SysExPool *const pool;
        std::atomic<unsigned> references{1};
        // Only the sequencer owner thread changes this count.
        unsigned storedReferences = 0;
    };

    class SysExReportBatch
    {
        inline static thread_local SysExReportBatch *current = nullptr;
        SysExReportBatch *previous = current;
        bool reported = false;

    public:
        SysExReportBatch() { current = this; }
        ~SysExReportBatch() { current = previous; }
        static bool allow()
        {
            auto *batch = current;
            if (!batch) return true;
            while (batch->previous) batch = batch->previous;
            if (batch->reported) return false;
            batch->reported = true;
            return true;
        }
    };

    class SysExPool : public std::enable_shared_from_this<SysExPool>
    {
        concurrency::FreeList<SysExPayload, 1024> slots;
        std::atomic<unsigned> persistent{0}, total{0};
        std::function<void()> notify;

    public:
        static constexpr size_t MaxBytes = 512;
        static constexpr unsigned PersistentLimit = 768;
        explicit SysExPool(std::function<void()> notify = {})
            : notify(std::move(notify)) {}

        void reportFull() const
        {
            if (SysExReportBatch::allow() && notify) notify();
        }

        SysExPayload *acquire(const uint8_t *bytes, size_t count)
        {
            if (count > MaxBytes)
            {
                reportFull();
                return nullptr;
            }
            if (count < 2 || !bytes) return nullptr;
            SysExPayload *p = nullptr;
            if (!slots.acquire(p))
            {
                reportFull();
                return nullptr;
            }
            new (p) SysExPayload(bytes, count, this);
            ++total;
            return p;
        }

        SysExPayload *acquireEdited(const SysExPayload *source, size_t index,
                                    uint8_t value)
        {
            if (!source || index >= source->byteCount) return nullptr;
            auto *copy = acquire(source->bytes, source->byteCount);
            if (copy) copy->bytes[index] = value;
            return copy;
        }

        void retain(const SysExPayload *p) const
        {
            if (p) const_cast<SysExPayload *>(p)->references.fetch_add(
                1, std::memory_order_relaxed);
        }

        void release(const SysExPayload *p)
        {
            if (!p) return;
            auto *slot = const_cast<SysExPayload *>(p);
            if (slot->references.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                --total;
                slot->~SysExPayload();
                slots.release(slot);
            }
        }

        // Only the sequencer owner thread calls this. The caller owns ordinary
        // references to both payloads and releases the old one after success.
        bool replaceStored(const SysExPayload *oldPayload,
                           const SysExPayload *newPayload)
        {
            if ((oldPayload && oldPayload->pool != this) ||
                (newPayload && newPayload->pool != this))
                return false;
            if (oldPayload == newPayload) return true;
            auto *oldSlot = const_cast<SysExPayload *>(oldPayload);
            auto *newSlot = const_cast<SysExPayload *>(newPayload);
            bool transferred = false;
            if (newSlot && newSlot->storedReferences == 0)
            {
                if (persistent.load(std::memory_order_relaxed) >= PersistentLimit)
                {
                    if (!oldSlot || oldSlot->storedReferences != 1)
                    {
                        reportFull();
                        return false;
                    }
                    transferred = true;
                }
                else
                {
                    ++persistent;
                }
            }
            if (newSlot) ++newSlot->storedReferences;
            if (oldSlot && --oldSlot->storedReferences == 0 && !transferred)
                --persistent;
            return true;
        }

        void releaseStored(const SysExPayload *p)
        {
            if (!p) return;
            auto *slot = const_cast<SysExPayload *>(p);
            if (--slot->storedReferences == 0) --persistent;
            release(p);
        }
        unsigned used() const { return total.load(); }
        unsigned persistentUsed() const { return persistent.load(); }
    };

    class SysExPayloadOwner
    {
        const SysExPayload *payload = nullptr;
        std::shared_ptr<SysExPool> pool;

    public:
        SysExPayloadOwner() = default;
        // Adopt the reference returned by acquire().
        explicit SysExPayloadOwner(SysExPayload *p)
            : payload(p), pool(p ? p->pool->shared_from_this() : nullptr) {}
        // Share an existing payload.
        explicit SysExPayloadOwner(const SysExPayload *p)
            : payload(p), pool(p ? p->pool->shared_from_this() : nullptr)
        {
            if (p) p->pool->retain(p);
        }
        SysExPayloadOwner(const SysExPayloadOwner &o)
            : payload(o.payload), pool(o.pool)
        {
            if (payload) pool->retain(payload);
        }
        SysExPayloadOwner(SysExPayloadOwner &&o) noexcept
            : payload(std::exchange(o.payload, nullptr)), pool(std::move(o.pool)) {}
        SysExPayloadOwner &operator=(SysExPayloadOwner o) noexcept
        {
            std::swap(payload, o.payload);
            pool.swap(o.pool);
            return *this;
        }
        ~SysExPayloadOwner() { if (payload) pool->release(payload); }
        const SysExPayload *get() const { return payload; }
    };
} // namespace mpc::sequencer
