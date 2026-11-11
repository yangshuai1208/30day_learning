#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/* 与 retry_check.c 中的接口类型保持一致；被测函数由真实源文件提供。 */
typedef enum
{
    RETRY_INVALID = 0,
    RETRY_NO_PENDING,
    RETRY_WAIT,
    RETRY_SEND_AGAIN,
    RETRY_GIVE_UP
} RetryResult;

typedef struct
{
    uint32_t last_send_tick;
    uint8_t retry_count;
    bool awaiting_ack;
} RetryState;

RetryResult retry_check(
    const RetryState *state,
    uint32_t now_tick,
    uint32_t timeout_ms,
    uint8_t max_retries);

static void assert_state_unchanged(
    const RetryState *state,
    uint32_t last_send_tick,
    uint8_t retry_count,
    bool awaiting_ack)
{
    assert(state->last_send_tick == last_send_tick);
    assert(state->retry_count == retry_count);
    assert(state->awaiting_ack == awaiting_ack);
}

static void test_null_state(void)
{
    assert(retry_check(NULL, 100U, 10U, 3U) == RETRY_INVALID);
    puts("[通过] NULL 状态指针");
}

static void test_zero_timeout(void)
{
    const RetryState state = {100U, 0U, true};

    assert(retry_check(&state, 100U, 0U, 3U) == RETRY_INVALID);
    assert_state_unchanged(&state, 100U, 0U, true);
    puts("[通过] timeout_ms 为 0");
}

static void test_no_pending_ack(void)
{
    const RetryState state = {100U, 0U, false};

    assert(retry_check(&state, 1000U, 10U, 3U) == RETRY_NO_PENDING);
    assert_state_unchanged(&state, 100U, 0U, false);
    puts("[通过] awaiting_ack 为 false");
}

static void test_before_timeout(void)
{
    const RetryState state = {100U, 1U, true};

    assert(retry_check(&state, 109U, 10U, 3U) == RETRY_WAIT);
    assert_state_unchanged(&state, 100U, 1U, true);
    puts("[通过] 未达到超时阈值");
}

static void test_exact_timeout(void)
{
    const RetryState state = {100U, 1U, true};

    assert(retry_check(&state, 110U, 10U, 3U) == RETRY_SEND_AGAIN);
    assert_state_unchanged(&state, 100U, 1U, true);
    puts("[通过] 恰好达到超时阈值");
}

static void test_timeout_retry_allowed(void)
{
    const RetryState state = {100U, 1U, true};

    assert(retry_check(&state, 111U, 10U, 3U) == RETRY_SEND_AGAIN);
    assert_state_unchanged(&state, 100U, 1U, true);
    puts("[通过] 超时且仍允许重传");
}

static void test_retry_limit_reached_after_timeout(void)
{
    const RetryState state = {100U, 3U, true};

    assert(retry_check(&state, 110U, 10U, 3U) == RETRY_GIVE_UP);
    assert_state_unchanged(&state, 100U, 3U, true);
    puts("[通过] 超时且重试次数已达上限");
}

static void test_retry_limit_reached_before_timeout(void)
{
    const RetryState state = {100U, 3U, true};

    assert(retry_check(&state, 109U, 10U, 3U) == RETRY_WAIT);
    assert_state_unchanged(&state, 100U, 3U, true);
    puts("[通过] 重试次数已达上限但尚未超时");
}

static void test_zero_max_retries(void)
{
    const RetryState state = {100U, 0U, true};

    assert(retry_check(&state, 110U, 10U, 0U) == RETRY_GIVE_UP);
    assert_state_unchanged(&state, 100U, 0U, true);
    puts("[通过] max_retries 为 0");
}

static void test_tick_wraparound(void)
{
    const RetryState state = {UINT32_MAX - 4U, 0U, true};

    /* 从 UINT32_MAX-4 到 5 的无符号差值为 10，正好达到阈值。 */
    assert(retry_check(&state, 5U, 10U, 1U) == RETRY_SEND_AGAIN);
    assert_state_unchanged(&state, UINT32_MAX - 4U, 0U, true);
    puts("[通过] uint32_t Tick 回绕");
}

int main(void)
{
    test_null_state();
    test_zero_timeout();
    test_no_pending_ack();
    test_before_timeout();
    test_exact_timeout();
    test_timeout_retry_allowed();
    test_retry_limit_reached_after_timeout();
    test_retry_limit_reached_before_timeout();
    test_zero_max_retries();
    test_tick_wraparound();

    puts("全部 retry_check 单元测试通过。");
    return 0;
}
