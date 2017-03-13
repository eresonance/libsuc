/**
 * Range utils to be a bit like python.
 */

#ifndef _SUC_RANGE_H_
#define _SUC_RANGE_H_

//for abs
#include <stdlib.h>

#include "suc_macros.h"

/**
 * Range type to use for the for loops
 */
typedef struct _suc_range {
    int start;
    int stop;
    int step;
} suc_range;

/**
 * Helper to make a range object, same prototype as in python.
 * 
 * range(stop)
 * range(start, stop[, step])
 */
#define range(...) SUC_VFUNC(_range_obj_, __VA_ARGS__)

/**
 * Loop int var over a pointer to a suc_range object.
 * var has only inner scope.
 *
 * ex:
 * for_in(i, &range(1, 10, 1)) {
 *   printf("i = %d\n", i);
 * }
 */
#define for_in(var, range) for(suc_range *_r=(range); _r; _r=NULL) \
                           for(int _i=0, _end=abs((_r->stop - _r->start)/_r->step), _d=1; _d; _d=0) \
                           for(int var=_r->start; _i < _end; var += _r->step, _i++)

/**
 * Loop an existing var over a pointer to suc_range object.
 * This just accepts a var from an outer scope.
 *
 * ex:
 * int i;
 * int timeout=100;
 * for_ex_in(i, &range(timeout)) {
 *    ...
 * }
 * if(i < timeout) <error>
 */
#define for_ex_in(var, range) for(suc_range *_r=(range); _r; _r=NULL) \
                              for(int _i=0, _end=abs((_r->stop - _r->start)/_r->step), _d=1; _d; _d=0) \
                              for(var=_r->start; _i < _end; var += _r->step, _i++)

// Internal stuff below, don't use these directly
#define _range_obj_1(astop)                (suc_range){.start=0, .stop=(astop), .step=1}
#define _range_obj_2(astart, astop)        (suc_range){.start=(astart), .stop=(astop), .step=1}
#define _range_obj_3(astart, astop, astep) (suc_range){.start=(astart), .stop=(astop), .step=(astep)}

#endif //_SUC_RANGE_H_
