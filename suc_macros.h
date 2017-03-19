/* libsuc - Simple utilities for C
 *
 * Provides some common helper macros.
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

#ifndef _SUC_MACROS_H_
#define _SUC_MACROS_H_

// Concatenates two words
#define _SUC_CAT2(x, y) x##y
#define SUC_CAT(x, y) _SUC_CAT2(x, y)

// get number of arguments
// http://stackoverflow.com/a/26408195/780110
#define _SUC_NARG_(...)  _SUC_NARG_I_(__VA_ARGS__,_SUC_RSEQ_N())
#define _SUC_NARG_I_(...) _SUC_ARG_N(__VA_ARGS__)
#define _SUC_ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, _9, N,...) N
#define _SUC_RSEQ_N() 9,8,7,6,5,4,3,2,1,0

// virtual fn that will eval to the correct one with '_*' added to the end
#define SUC_VFUNC(func, ...) SUC_CAT(func, _SUC_NARG_(__VA_ARGS__))(__VA_ARGS__)

// why these aren't in stdlib I have no idea...
#define SUC_MAX(a,b) ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b; })
#define SUC_MIN(a,b) ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b; })

#define SUC_LEN(x) (sizeof(x)/sizeof(x[0]))

#endif //_SUC_MACROS_H_
