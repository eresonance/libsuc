/**
 * Provides some common helper macros.
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

#endif //_SUC_MACROS_H_
