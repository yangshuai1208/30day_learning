
#include <assert.h>
#include <stdio.h>

#include "register_bits.c"

int main(void)
{
    uint8_t reg = 0xA5;

    // 正常修改
    assert(register_update_bits(&reg, 0x24, 0x04));
    assert(reg == 0x85);

    // 非法参数
    assert(!register_update_bits(NULL, 0x04, 0x04));
    assert(!register_update_bits(&reg, 0x00, 0x00));

    // value超出mask范围
    assert(!register_update_bits(&reg, 0x04, 0x80));
    assert(reg == 0x85);

    // 允许设置多个指定bit
    assert(register_update_bits(&reg, 0x0F, 0x0A));
    assert(reg == 0x8A);

    puts("DAY10_REGISTER_TEST_PASS");
    return 0;
}
