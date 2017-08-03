#include "suc_range.h"

int _suc_calc_end(const suc_range *r)
{
    if((r->step > 0) && (r->start < r->stop)) {
        //going up
        return 1 + (r->stop - 1 - r->start)/r->step;
    }
    else if((r->step < 0) && (r->start > r->stop)) {
        //going down
        return 1 + (r->start - 1 - r->stop)/(0-r->step);
    }
    else if(r->step == 0) {
        //invalid
        return -1;
    }
    //otherwise no steps
    return 0;
}

#ifdef SUC_TEST_MAIN
#include <assert.h>
#include <stdio.h>

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
    for_ex_in(i, &t3) {
        printf("i = %d\n", i);
    }
    printf("end i=%d\n", i);
    assert(i == t3.stop);
    
    const int start = 1, stop = 2;
    const suc_range *t4 = &range(start, stop);
    for_in(idx, t4) {
        printf("idx = %d\n", idx);
    }

    return 0;
}
#endif

