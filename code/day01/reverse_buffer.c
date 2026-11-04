
#include <stdint.h>
#include <stddef.h>

void reverse_buffer(uint8_t *buf, size_t len)
{
  
    if(buf==NULL||len<2)
    {
        return;
    }
    uint8_t *left = buf;
    uint8_t *right = buf + len - 1;

    while(left < right)
    {
        uint8_t temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
}

#include <assert.h>
#include <stdio.h>

int main(void)
{
    uint8_t a[] = {1, 2, 3, 4, 5};
    reverse_buffer(a, 5);

    assert(a[0] == 5);
    assert(a[1] == 4);
    assert(a[2] == 3);
    assert(a[3] == 2);
    assert(a[4] == 1);

    uint8_t b[] = {1, 2, 3, 4};
    reverse_buffer(b, 4);

    assert(b[0] == 4);
    assert(b[3] == 1);

    uint8_t c[] = {7};
    reverse_buffer(c, 1);
    assert(c[0] == 7);

    reverse_buffer(NULL, 0);

    puts("All tests passed!");
    return 0;
}
