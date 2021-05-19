#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class Bit_mask {
public:
    Bit_mask() : data_(1, uint64_t{0ul}) {}
    Bit_mask(size_t size)
            : size_{size},
              data_(size / one_element_size_c + 1, uint64_t{0ul}) {}
    Bit_mask(const Bit_mask&) = default;
    Bit_mask(Bit_mask&&)      = default;

    Bit_mask& operator=(const Bit_mask&) = default;
    Bit_mask& operator=(Bit_mask&&) = default;

    ~Bit_mask() = default;

    size_t size() const {
        return size_;
    }
    void resize(size_t new_size);

    void set_bit(size_t i, bool value = true);
    void reset_bit(size_t i) {
        set_bit(i, false);
    }
    bool   check_bit(size_t i) const;
    size_t ones() const;

    const std::vector<uint64_t>& data() const;
    std::vector<uint64_t>&       data();

    Bit_mask  operator&(const Bit_mask& other) const;
    Bit_mask& operator&=(const Bit_mask& other);
    Bit_mask  operator|(const Bit_mask& other) const;
    Bit_mask& operator|=(const Bit_mask& other);
    Bit_mask  operator^(const Bit_mask& other) const;
    Bit_mask& operator^=(const Bit_mask& other);
    Bit_mask  operator~() const;
    Bit_mask& invert();

    bool operator==(const Bit_mask& other) const;
    bool operator!=(const Bit_mask& other) const {
        return !(*this == other);
    }
    bool operator<(const Bit_mask& other) const {
        return ones() < other.ones();
    }
    bool operator>(const Bit_mask& other) const {
        return ones() > other.ones();
    }
    bool operator<=(const Bit_mask& other) const {
        return !(*this > other);
    }
    bool operator>=(const Bit_mask& other) const {
        return !(*this < other);
    }

private:
    static constexpr size_t one_element_size_c{sizeof(uint64_t) * 8};
    size_t                  size_{0};
    std::vector<uint64_t>   data_;
};
