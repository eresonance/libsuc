/* libsuc - Simple utilities for C
 *
 * Range utilities to emulate python behaviour.
 *
 * Copyright (c) 2017 - Devin Linnington
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 * Range meta-data for use with for_daft_in
 */
typedef struct _suc_range_meta {
    const suc_range *_r;
    int _i;
    int _end;
    bool _d;
} suc_range_meta;

/**
 * Helper to make a range object, same prototype as in python.
 * 
 * range(stop)
 * range(start, stop[, step])
 */
#define range(...) SUC_VFUNC(_range_obj_, __VA_ARGS__)

/**
 * Same as range(...) but as a const expression for initializing
 */
#define range_init(...) SUC_VFUNC(_range_const_, __VA_ARGS__)

/**
 * Loop int var over a pointer to a suc_range object.
 * var has only inner scope.
 *
 * ex:
 * for_in(i, &range(1, 10, 1)) {
 *   printf("i = %d\n", i);
 * }
 */
#define for_in(var, range) for(const suc_range *_r=(range); _r; _r=NULL) \
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
#define for_ex_in(var, range) for(const suc_range *_r=(range); _r; _r=NULL) \
                              for(int _i=0, _end=abs((_r->stop - _r->start)/_r->step), _d=1; _d; _d=0) \
                              for((var)=_r->start; _i < _end; (var) += _r->step, _i++)

/**
 * Loop over a range using a struct to hold the meta-info.
 * Useful for using these ranges in a daft function.
 *
 * for_daft_in(stack->i, stack->range_meta, cfg->loop_range) {
 *     ...
 *     daft_yield(stack->ctx);
 *     ...
 * }
 */
#define for_daft_in(var, meta, range) \
    /* init the meta struct */ \
    for((meta)->_r=(range), (meta)->_i=0, (meta)->_end=abs(((meta)->_r->stop-(meta)->_r->start)/(meta)->_r->step), \
            /* loop once, just for scope */ \
            (meta)->_d=false; !((meta)->_d); (meta)->_d=true) \
    /* the actual loop */ \
    for((var)=(meta)->_r->start; (meta)->_i < (meta)->_end; (var) += (meta)->_r->step, (meta)->_i++)

// Internal stuff below, don't use these directly
#define _range_obj_1(astop)                (suc_range)_range_const_1(astop)
#define _range_obj_2(astart, astop)        (suc_range)_range_const_2(astart, astop)
#define _range_obj_3(astart, astop, astep) (suc_range)_range_const_3(astart, astop, astep)
#define _range_const_1(astop)                {.start=0, .stop=(astop), .step=1}
#define _range_const_2(astart, astop)        {.start=(astart), .stop=(astop), .step=1}
#define _range_const_3(astart, astop, astep) {.start=(astart), .stop=(astop), .step=(astep)}

#endif //_SUC_RANGE_H_
