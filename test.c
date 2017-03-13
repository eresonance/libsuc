
#include <assert.h>
#include <stdio.h>

#include "suc_range.h"

int main(void)
{
    suc_range tmp = range(8);
    assert(tmp.start==0);
    assert(tmp.stop==8);
    assert(tmp.step==1);
    
    for_in(index, &tmp) {
        printf("index = %d\n", index);
    }
    
    suc_range t2;
    t2 = range(1, 9);
    assert(t2.start==1);
    assert(t2.stop==9);
    assert(t2.step==1);
    
    suc_range t3 = range(5, -6, -1);
    assert(t3.start==5);
    assert(t3.stop==-6);
    assert(t3.step==-1);
    
    int i;
    for_out_in(i, &t3) {
        printf("i = %d\n", i);
    }
    printf("end i=%d\n", i);
    assert(i == t3.stop);
    
    return 0;
}
