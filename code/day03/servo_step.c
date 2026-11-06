
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    uint8_t current_angle;
    uint8_t target_angle;
    uint32_t last_tick;
} ServoMotion;

bool servo_step(
    ServoMotion *servo,
    uint32_t now_tick,
    uint32_t interval_ms)
{
    // 1. 检查空指针与时间间隔
    if (!servo || interval_ms == 0)
        return false;

    // 2. 判断是否已经到达目标角度
    if (servo->current_angle == servo->target_angle)
        return false;

    // 3. 使用无符号减法判断时间是否到达
    if ((now_tick - servo->last_tick) < interval_ms)
        return false;
    // 4. 当前角度向目标方向移动1度
    if (servo->current_angle < servo->target_angle)
        servo->current_angle++;
    else
        servo->current_angle--;
    // 5. 更新时间并返回true
    servo->last_tick = now_tick;
    return true;
}
