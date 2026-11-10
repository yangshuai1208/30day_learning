#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

bool dma_copy_delta(
    const uint8_t *dma_buf,
    size_t size,
    size_t last_pos,
    size_t new_pos,
    uint8_t *out,
    size_t out_cap,
    size_t *copied);

#define GUARD_VALUE 0xA5U
#define COPIED_SENTINEL ((size_t)0x5AU)

typedef struct
{
    uint8_t before;
    uint8_t data[8];
    uint8_t after;
} guarded_output_t;

static void fill_output(guarded_output_t *output, uint8_t value)
{
    memset(output, value, sizeof(*output));
}

static void assert_output_unchanged(const guarded_output_t *output,
                                    uint8_t value)
{
    const uint8_t *bytes = (const uint8_t *)output;

    for (size_t i = 0U; i < sizeof(*output); ++i)
    {
        assert(bytes[i] == value);
    }
}

static void test_normal_incremental_copy(void)
{
    const uint8_t dma[] = {10U, 11U, 12U, 13U, 14U, 15U};
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(dma_copy_delta(dma, sizeof(dma), 1U, 4U,
                          output.data, sizeof(output.data), &copied));
    assert(copied == 3U);
    assert(output.data[0] == 11U);
    assert(output.data[1] == 12U);
    assert(output.data[2] == 13U);
    assert(output.before == GUARD_VALUE);
    assert(output.after == GUARD_VALUE);
}

static void test_circular_wrap_copy(void)
{
    const uint8_t dma[] = {20U, 21U, 22U, 23U, 24U, 25U};
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(dma_copy_delta(dma, sizeof(dma), 4U, 2U,
                          output.data, sizeof(output.data), &copied));
    assert(copied == 4U);
    assert(output.data[0] == 24U);
    assert(output.data[1] == 25U);
    assert(output.data[2] == 20U);
    assert(output.data[3] == 21U);
    assert(output.before == GUARD_VALUE);
    assert(output.after == GUARD_VALUE);
}

static void test_equal_positions_copy_zero_bytes(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(dma_copy_delta(dma, sizeof(dma), 2U, 2U,
                          output.data, sizeof(output.data), &copied));
    assert(copied == 0U);
    assert_output_unchanged(&output, GUARD_VALUE);
}

static void test_exact_output_capacity(void)
{
    const uint8_t dma[] = {30U, 31U, 32U, 33U, 34U};
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(dma_copy_delta(dma, sizeof(dma), 1U, 4U,
                          output.data, 3U, &copied));
    assert(copied == 3U);
    assert(output.data[0] == 31U);
    assert(output.data[1] == 32U);
    assert(output.data[2] == 33U);
    assert(output.data[3] == GUARD_VALUE);
    assert(output.before == GUARD_VALUE);
    assert(output.after == GUARD_VALUE);
}

static void assert_failure_without_writes(const uint8_t *dma,
                                          size_t size,
                                          size_t last_pos,
                                          size_t new_pos,
                                          size_t out_cap)
{
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(!dma_copy_delta(dma, size, last_pos, new_pos,
                           output.data, out_cap, &copied));
    assert(copied == COPIED_SENTINEL);
    assert_output_unchanged(&output, GUARD_VALUE);
}

static void test_output_capacity_too_small(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U, 5U};
    assert_failure_without_writes(dma, sizeof(dma), 1U, 4U, 2U);
}

static void test_null_input_pointer(void)
{
    guarded_output_t output;
    size_t copied = COPIED_SENTINEL;
    fill_output(&output, GUARD_VALUE);

    assert(!dma_copy_delta(NULL, 4U, 0U, 1U,
                           output.data, sizeof(output.data), &copied));
    assert(copied == COPIED_SENTINEL);
    assert_output_unchanged(&output, GUARD_VALUE);
}

static void test_null_output_pointer(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    size_t copied = COPIED_SENTINEL;

    assert(!dma_copy_delta(dma, sizeof(dma), 0U, 2U,
                           NULL, 4U, &copied));
    assert(copied == COPIED_SENTINEL);
}

static void test_null_copied_pointer(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    guarded_output_t output;
    fill_output(&output, GUARD_VALUE);

    assert(!dma_copy_delta(dma, sizeof(dma), 0U, 2U,
                           output.data, sizeof(output.data), NULL));
    assert_output_unchanged(&output, GUARD_VALUE);
}

static void test_zero_size(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    assert_failure_without_writes(dma, 0U, 0U, 0U, 0U);
}

static void test_last_position_out_of_range(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    assert_failure_without_writes(dma, sizeof(dma), sizeof(dma), 0U, 4U);
}

static void test_new_position_out_of_range(void)
{
    const uint8_t dma[] = {1U, 2U, 3U, 4U};
    assert_failure_without_writes(dma, sizeof(dma), 0U, sizeof(dma), 4U);
}

int main(void)
{
    test_normal_incremental_copy();
    test_circular_wrap_copy();
    test_equal_positions_copy_zero_bytes();
    test_exact_output_capacity();
    test_output_capacity_too_small();
    test_null_input_pointer();
    test_null_output_pointer();
    test_null_copied_pointer();
    test_zero_size();
    test_last_position_out_of_range();
    test_new_position_out_of_range();

    return 0;
}
