
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

/*
 * 计算STOP命令从接收到处理的延迟。
 *
 * rx_tick: 收到STOP时的Tick
 * handle_tick: 开始处理STOP时的Tick
 * deadline_ms: 最大允许延迟
 * latency_ms: 输出实际延迟
 *
 * 返回true：未超时
 * 返回false：参数错误或超时
 */

bool stop_latency_check(
    uint32_t rx_tick,
    uint32_t handle_tick,
    uint32_t deadline_ms,
    uint32_t *latency_ms)
{
    if (latency_ms == NULL || deadline_ms == 0U) {
        return false;
    }

    uint32_t latency = handle_tick - rx_tick;
    *latency_ms = latency;
    return latency <= deadline_ms;
}


int main(void)
{
    uint32_t latency = 999;

    // 正常延迟：10ms
    assert(stop_latency_check(100, 110, 20, &latency));
    assert(latency == 10);

    // 刚好达到20ms
    assert(stop_latency_check(100, 120, 20, &latency));
    assert(latency == 20);

    // 超时25ms：仍须输出实际延迟
    assert(!stop_latency_check(100, 125, 20, &latency));
    assert(latency == 25);

    // Tick回绕：经过32ms
    assert(stop_latency_check(
        0xFFFFFFF0U, 0x00000010U, 40, &latency));
    assert(latency == 32);

    // 参数非法时不得修改输出
    latency = 999;
    assert(!stop_latency_check(100, 110, 0, &latency));
    assert(latency == 999);

    assert(!stop_latency_check(100, 110, 20, NULL));

    puts("Day 4 tests passed!");
    return 0;
}
