
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 * 修改8位寄存器中指定的位。
 *
 * reg：寄存器值的地址
 * mask：需要修改的位，1表示允许修改
 * value：修改后的位值
 *
 * 返回true表示成功，false表示参数非法。
 * 失败时不得修改原寄存器值。
 */

bool register_update_bits(
    uint8_t *reg,
    uint8_t mask,
    uint8_t value)
{
    // 1. 检查指针和掩码
    if (reg == NULL || mask == 0U)
    {
        return false;
    }

    // 2. 检查value是否超出mask范围
    if ((value & ~mask) != 0U)
    {
        return false;
    }

    // 3. 清除指定位置，再写入新值
    *reg = (uint8_t)((*reg & ~mask) | value);

    return true;
}
