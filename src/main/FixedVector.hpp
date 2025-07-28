#pragma once
#include <array>

#include <stdexcept>

template <typename T, std::size_t Capacity>
class FixedVector {
    std::array<T, Capacity> data_;
    std::size_t size_ = 0;

public:
    FixedVector() = default;

    FixedVector(const FixedVector& other)
        : data_(other.data_), size_(other.size_) {}

    FixedVector& operator=(const FixedVector& other) {
        if (this != &other) {
            data_ = other.data_;
            size_ = other.size_;
        }
        return *this;
    }
    void push_back(const T& value) {
        if (size_ < Capacity) data_[size_++] = value;
        else throw std::overflow_error("FixedVector capacity exceeded");
    }

    bool contains(const T& value) {
        for (std::size_t i = 0; i < size_; ++i)
            if (data_[i] == value) return true;
        return false;
    }

    void clear() { size_ = 0; }
    std::size_t size() const { return size_; }
    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    bool operator==(const FixedVector& other) const {
        if (size_ != other.size_) return false;
        for (std::size_t i = 0; i < size_; ++i)
            if (!(data_[i] == other.data_[i])) return false;
        return true;
    }
};
