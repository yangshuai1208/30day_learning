
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>


#define SERVO_COUNT 5

typedef struct
{
    uint8_t angles[SERVO_COUNT];
    uint8_t updated_mask;
} ServoState;

bool servo_set_angle(
    ServoState *state,
    uint8_t channel,
    uint8_t angle)
{
    // 1. 检查空指针
    if(state==NULL)
    {
        return false;
    }
    // 2. 检查通道和角度是否合法
    if(channel>=SERVO_COUNT||angle>180U)
    {
        return false;
    }
    // 3. 更新对应通道角度
    state->angles[channel]=angle;
    // 4. 将updated_mask对应位置1
    state->updated_mask |= (uint8_t)(1U << channel);
    // 5. 返回true
    return true;
}



int main(void)
{
    ServoState state = {0};

    // 测试1：空指针
    assert(servo_set_angle(NULL, 0, 90) == false);

    // 测试2：通道越界
    assert(servo_set_angle(&state, 5, 90) == false);

    // 测试3：角度越界
    assert(servo_set_angle(&state, 0, 181) == false);

    // 测试4：正常更新角度
    assert(servo_set_angle(&state, 2, 90) == true);
    assert(state.angles[2] == 90);
    assert(state.updated_mask == 0x04);

    // 测试5：更新其他通道，保留已有标志位
    assert(servo_set_angle(&state, 0, 180) == true);
    assert(state.angles[0] == 180);
    assert(state.updated_mask == 0x05);

    puts("All Day 2 tests passed!");
    return 0;
}
