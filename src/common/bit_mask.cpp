#include "bit_mask.hpp"

void Bit_mask::resize(size_t new_size) {
    auto old_size = size();
    if (new_size <= old_size) {
        return;
    }
    if (new_size <= data_.size() * one_element_size_c) {
        size_ = new_size;
        return;
    }

    data_.resize(new_size / one_element_size_c + 1);
    for (size_t i{old_size / one_element_size_c + 1}; i < data_.size(); i++) {
        data_[i] = 0;
    }
    size_ = new_size;
}

void Bit_mask::set_bit(size_t i, bool value) {
    if (i >= size()) {
        resize(i + 1);
    }

    size_t   real_index      = i / one_element_size_c;
    size_t   bit_in_on_value = i % one_element_size_c;
    uint64_t work_value{0ul};
    if (value) {
        work_value |= (1ul << bit_in_on_value);
        data_[real_index] |= work_value;
    } else {
        work_value = ~work_value ^ (1ul << bit_in_on_value);
        data_[real_index] &= work_value;
    }
}

bool Bit_mask::check_bit(size_t i) const {
    if (i >= size()) {
        return false;
    }
    size_t   real_index      = i / one_element_size_c;
    size_t   bit_in_on_value = i % one_element_size_c;
    uint64_t work_value      = uint64_t{0ul} | (1ul << bit_in_on_value);
    return data_[real_index] & work_value;
}

size_t Bit_mask::ones() const {
    size_t result{0};
    for (size_t i{0}; i < data_.size(); ++i) {
        if (!data_[i]) {
            continue;
        }
        size_t base = i * one_element_size_c;
        for (size_t j = 0; (j < one_element_size_c); j++) {
            if (check_bit(base + j)) {
                ++result;
            }
        }
    }
    return result;
}

const std::vector<uint64_t>& Bit_mask::data() const {
    return data_;
}

std::vector<uint64_t>& Bit_mask::data() {
    return data_;
}

Bit_mask Bit_mask::operator&(const Bit_mask& other) const {
    size_t max_size, min_size;
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        min_size = data_.size();
        max_size = other.data_.size();
    }

    Bit_mask result(max_size * one_element_size_c);
    for (size_t i{0}; i < min_size; i++) {
        result.data_[i] = data_[i] & other.data_[i];
    }

    return result;
}

Bit_mask& Bit_mask::operator&=(const Bit_mask& other) {
    if (other.size() > size()) {
        resize(other.size());
    }

    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] &= other.data_[i];
    }

    return *this;
}

Bit_mask Bit_mask::operator|(const Bit_mask& other) const {
    size_t max_size, min_size;
    bool   other_greater{false};
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        other_greater = true;
        min_size      = data_.size();
        max_size      = other.data_.size();
    }

    Bit_mask result(max_size * one_element_size_c);
    for (size_t i{0}; i < min_size; i++) {
        result.data_[i] = data_[i] | other.data_[i];
    }

    if (other_greater) {
        for (size_t i{min_size}; i < max_size; i++) {
            result.data_[i] = other.data_[i];
        }
    } else {
        for (size_t i{min_size}; i < max_size; i++) {
            result.data_[i] = data_[i];
        }
    }

    return result;
}

Bit_mask& Bit_mask::operator|=(const Bit_mask& other) {
    if (other.size() > size()) {
        resize(other.size());
    }

    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] |= other.data_[i];
    }

    return *this;
}

Bit_mask Bit_mask::operator^(const Bit_mask& other) const {
    size_t max_size, min_size;
    bool   other_greater{false};
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        other_greater = true;
        min_size      = data_.size();
        max_size      = other.data_.size();
    }

    Bit_mask result(max_size * one_element_size_c);
    for (size_t i{0}; i < min_size; i++) {
        result.data_[i] = data_[i] ^ other.data_[i];
    }

    if (other_greater) {
        for (size_t i{min_size}; i < max_size; i++) {
            result.data_[i] = other.data_[i];
        }
    } else {
        for (size_t i{min_size}; i < max_size; i++) {
            result.data_[i] = data_[i];
        }
    }
    return result;
}

Bit_mask& Bit_mask::operator^=(const Bit_mask& other) {
    if (other.size() > size()) {
        resize(other.size());
    }

    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] ^= other.data_[i];
    }
    return *this;
}

Bit_mask Bit_mask::operator~() const {
    Bit_mask result(size_);
    for (size_t i{0}; i < data_.size(); i++) {
        result.data_[i] = ~data_[i];
    }
    return result;
}

Bit_mask& Bit_mask::invert() {
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] = ~data_[i];
    }

    return *this;
}

bool Bit_mask::operator==(const Bit_mask& other) const {
    size_t max_size, min_size;
    bool   other_greater{false};
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        other_greater = true;
        min_size      = data_.size();
        max_size      = other.data_.size();
    }

    Bit_mask result(max_size * one_element_size_c);
    for (size_t i{0}; i < min_size; i++) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }

    if (other_greater) {
        for (size_t i{min_size}; i < max_size; i++) {
            if (other.data_[i]) {
                return false;
            }
        }
    } else {
        for (size_t i{min_size}; i < max_size; i++) {
            if (data_[i]) {
                return false;
            }
        }
    }

    return true;
}

bool Bit_mask::operator<(const Bit_mask& other) const {
    size_t max_size, min_size;
    bool   other_greater{false};
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        other_greater = true;
        min_size      = data_.size();
        max_size      = other.data_.size();
    }

    for (size_t i{0}; i < min_size; i++) {
        if (data_[i] > other.data_[i]) {
            return false;
        } else if (data_[i] < other.data_[i]) {
            return true;
        }
    }

    if (other_greater) {
        for (size_t i{min_size}; i < max_size; i++) {
            if (other.data_[i]) {
                return true;
            }
        }
    } else {
        for (size_t i{min_size}; i < max_size; i++) {
            if (data_[i]) {
                return false;
            }
        }
    }
    return false;
}

bool Bit_mask::operator>(const Bit_mask& other) const {
    size_t max_size, min_size;
    bool   other_greater{false};
    if (data_.size() > other.data_.size()) {
        max_size = data_.size();
        min_size = other.data_.size();
    } else {
        other_greater = true;
        min_size      = data_.size();
        max_size      = other.data_.size();
    }

    for (size_t i{0}; i < min_size; i++) {
        if (data_[i] < other.data_[i]) {
            return false;
        } else if (data_[i] > other.data_[i]) {
            return true;
        }
    }

    if (other_greater) {
        for (size_t i{min_size}; i < max_size; i++) {
            if (other.data_[i]) {
                return false;
            }
        }
    } else {
        for (size_t i{min_size}; i < max_size; i++) {
            if (data_[i]) {
                return true;
            }
        }
    }
    return false;
}
