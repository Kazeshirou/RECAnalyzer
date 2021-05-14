#include <gtest/gtest.h>

#include "bit_mask.hpp"

namespace tests {

TEST(bit_mask_tests, bit_mask) {
    size_t   size{66ul};
    Bit_mask mask(size);
    ASSERT_EQ(mask.size(), size);
    ASSERT_EQ(mask.data()[0], 0);
    ASSERT_EQ(mask.data()[1], 0);

    ASSERT_EQ(mask.ones(), 0);
    ASSERT_FALSE(mask.check_bit(0));
    mask.set_bit(0);
    ASSERT_EQ(mask.ones(), 1);
    ASSERT_FALSE(mask.check_bit(63));
    mask.set_bit(63);
    ASSERT_EQ(mask.ones(), 2);
    ASSERT_FALSE(mask.check_bit(65));
    mask.set_bit(65);
    ASSERT_EQ(mask.ones(), 3);
    ASSERT_TRUE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_EQ(mask.size(), size);
    ASSERT_EQ(mask.data()[0], 1ul | (1ul << 63));
    ASSERT_EQ(mask.data()[1], 1ul << 1);

    ASSERT_FALSE(mask.check_bit(68));
    mask.set_bit(68);
    ASSERT_EQ(mask.ones(), 4);
    ASSERT_TRUE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_TRUE(mask.check_bit(68));
    ASSERT_EQ(mask.size(), 69);
    ASSERT_EQ(mask.data()[0], 1ul | (1ul << 63));
    ASSERT_EQ(mask.data()[1], (1ul << 1) | (1ul << 4));

    ASSERT_FALSE(mask.check_bit(128));
    mask.set_bit(128);
    ASSERT_EQ(mask.ones(), 5);
    ASSERT_TRUE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_TRUE(mask.check_bit(68));
    ASSERT_TRUE(mask.check_bit(128));
    ASSERT_EQ(mask.size(), 129);
    ASSERT_EQ(mask.data()[0], 1ul | (1ul << 63));
    ASSERT_EQ(mask.data()[1], (1ul << 1) | (1ul << 4));
    ASSERT_EQ(mask.data()[2], 1ul);

    mask.set_bit(128, false);
    ASSERT_EQ(mask.ones(), 4);
    ASSERT_TRUE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_TRUE(mask.check_bit(68));
    ASSERT_FALSE(mask.check_bit(128));
    ASSERT_EQ(mask.size(), 129);
    ASSERT_EQ(mask.data()[0], 1ul | (1ul << 63));
    ASSERT_EQ(mask.data()[1], (1ul << 1) | (1ul << 4));
    ASSERT_EQ(mask.data()[2], 0ul);

    mask.set_bit(68, false);
    ASSERT_EQ(mask.ones(), 3);
    ASSERT_TRUE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_FALSE(mask.check_bit(68));
    ASSERT_FALSE(mask.check_bit(128));
    ASSERT_EQ(mask.size(), 129);
    ASSERT_EQ(mask.data()[0], 1ul | (1ul << 63));
    ASSERT_EQ(mask.data()[1], (1ul << 1));
    ASSERT_EQ(mask.data()[2], 0ul);

    mask.set_bit(0, false);
    ASSERT_EQ(mask.ones(), 2);
    ASSERT_FALSE(mask.check_bit(0));
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_TRUE(mask.check_bit(65));
    ASSERT_FALSE(mask.check_bit(68));
    ASSERT_FALSE(mask.check_bit(128));
    ASSERT_EQ(mask.size(), 129);
    ASSERT_EQ(mask.data()[0], (1ul << 63));
    ASSERT_EQ(mask.data()[1], (1ul << 1));
    ASSERT_EQ(mask.data()[2], 0ul);

    Bit_mask another_mask = mask;
    ASSERT_EQ(another_mask.ones(), 2);
    ASSERT_FALSE(another_mask.check_bit(0));
    ASSERT_TRUE(another_mask.check_bit(63));
    ASSERT_TRUE(another_mask.check_bit(65));
    ASSERT_FALSE(another_mask.check_bit(68));
    ASSERT_FALSE(another_mask.check_bit(128));
    ASSERT_EQ(another_mask.size(), 129);
    ASSERT_EQ(another_mask.data()[0], (1ul << 63));
    ASSERT_EQ(another_mask.data()[1], (1ul << 1));
    ASSERT_EQ(another_mask.data()[2], 0ul);

    another_mask.set_bit(63, false);
    ASSERT_TRUE(mask.check_bit(63));
    ASSERT_FALSE(another_mask.check_bit(63));
}

TEST(bit_mask_tests, bit_mask_operators) {
    size_t   size{68ul};
    Bit_mask mask(size);
    mask.data()[0] = {255ul};
    mask.data()[1] = {5ul};
    size_t   another_size{63ul};
    Bit_mask another_mask(another_size);
    another_mask.data()[0] = {9ul};

    auto and_result = mask & another_mask;
    ASSERT_EQ(and_result.data()[0], 255ul & 9ul);
    ASSERT_EQ(and_result.data()[1], 0ul);

    auto or_result = mask | another_mask;
    ASSERT_EQ(or_result.data()[0], 255ul | 9ul);
    ASSERT_EQ(or_result.data()[1], 5ul);

    auto xor_result = mask ^ another_mask;
    ASSERT_EQ(xor_result.data()[0], 255ul ^ 9ul);
    ASSERT_EQ(xor_result.data()[1], 5ul);

    auto not_result = ~mask;
    ASSERT_EQ(not_result.data()[0], ~uint64_t{255ul});
    ASSERT_EQ(not_result.data()[1], ~uint64_t{5ul});

    auto and_eq_result = mask;
    and_eq_result &= another_mask;
    ASSERT_EQ(and_eq_result, and_result);

    auto or_eq_result = mask;
    or_eq_result |= another_mask;
    ASSERT_EQ(or_eq_result, or_result);

    auto xor_eq_result = mask;
    xor_eq_result ^= another_mask;
    ASSERT_EQ(xor_eq_result, xor_result);

    auto invert_result = mask;
    invert_result.invert();
    ASSERT_EQ(invert_result, not_result);

    and_eq_result = another_mask;
    and_eq_result &= mask;
    ASSERT_EQ(and_eq_result, and_result);

    or_eq_result = another_mask;
    or_eq_result |= mask;
    ASSERT_EQ(or_eq_result, or_result);

    xor_eq_result = another_mask;
    xor_eq_result ^= mask;
    ASSERT_EQ(xor_eq_result, xor_result);
}

}  // namespace tests