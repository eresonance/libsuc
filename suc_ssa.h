/* libsuc - Simple utilities for C
 *
 * Simple static arrays.
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

#ifndef _SUC_SSA_H_
#define _SUC_SSA_H_

#include "suc_macros.h"
#include <stdlib.h>

#ifndef SSA_SIZE_TYPE
#define SSA_SIZE_TYPE size_t
#endif

struct ssa_attr {
    //num bytes allocated
    SSA_SIZE_TYPE alloc;
    //num of used elements
    SSA_SIZE_TYPE len;
    //size of each element
    SSA_SIZE_TYPE esz;
    //buf ptr
    char buf[];
};

//type can be called whatever you like
struct some_type {
    //and the attributes can be called whatever you like...
    struct ssa_attr whatever;
    //but must appear directly above your array
    uint32_t stuff[100];
};

//get the ssa header used to keep attributes from an array
#define SSA_HDR(array) ((struct ssa_attr*)(((char *)array)-(sizeof(struct ssa_attr*))))

/** initializes a new array of len 0, returning a pointer to the array
 * container: pointer to the struct containing the array and the ssa_attr
 * aname: name of the array element in container
 * returns: Pointer to the array, which you don't have to use
 */
#define ssa_new_empty(container, aname) ({ \
    __typeof__(container) tc = &(*container); /*container must be a ptr*/ \
    (__typeof__(tc->aname))_ssa_new(SSA_HDR(tc->aname), sizeof(tc->aname), sizeof(tc->aname[0]), NULL, 0); \
    })

//makes a new array based on an existing one
#define ssa_new(container, aname, init, init_size) ({ \
    __typeof__(container) tc = &(*container); /*container must be a ptr*/ \
    (__typeof__(tc->aname))_ssa_new(SSA_HDR(tc->aname), sizeof(tc->aname), sizeof(tc->aname[0]), (init), (init_size)); \
    })

//length of array in use
static inline size_t ssa_length(const void *array)
{
    return SSA_HDR(array)->length;
}

//size of the array that's been used, len*sizeof(array[0])
static inline size_t ssa_size(const void *array)
{
    struct ssa_attr *attr = SSA_HDR(array);
    return attr->len*attr->esz;
}

//available number of elements in array that are not in use, alloc/sizeof(array[0])-len
static inline size_t ssa_avail(const void *array)
{
    struct ssa_attr *attr = SSA_HDR(array);
    return attr->alloc/attr->esz - attr->len;
}

//clear an array, setting length=0
static inline void ssa_clear(void *array)
{
    SSA_HDR(array)->len = 0;
}

//resize length, zeroing new elements if expanding
void ssa_resize(void *array, size_t new_length);

//copy the contents of other into ssa array at index i
void ssa_cpy(void *array, size_t i, void *other, size_t other_size);

//copy the contents of other onto the end of ssa array
static inline void ssa_cat(void *array, void *other, size_t other_size)
{
    ssa_cpy(array, SSA_HDR(array)->len, other, other_size);
}

//copy the contents of ssa other_ssa onto the end of ssa array
static inline void ssa_cat_ssa(void *array, void *other_ssa)
{
    struct ssa_attr *attr = SSA_HDR(array);
    struct ssa_attr *oattr = SSA_HDR(other_ssa);
    assert(attr->e_sz == oattr->e_sz);
    //won't do anything if other_ssa is too big
    ssa_cpy(array, attr->len, other_ssa, oattr->len*oattr->esz);
}

//copy the contents of ssa other_ssa over the contents of ssa array, setting ssa array's size to that of ssa other_ssa
static inline void ssa_replace(void *array, void *other_ssa)
{
    struct ssa_attr *attr = SSA_HDR(array);
    struct ssa_attr *oattr = SSA_HDR(other_ssa);
    assert(attr->e_sz == oattr->e_sz);
    ssa_cpy(array, 0, other_ssa, ssa_size(other_ssa));
    //ssa_cpy will already set attr->length if it grew, so set it here only if it shrunk
    if(oattr->len < attr->len) {
        attr->len = oattr->len;
    }
}

//copies a portion of ssa array to ssa slice, replacing its contents
void ssa_slice(void *array, size_t start, size_t end, void *slice);

//safely push/append value to the end of an ssa array, if there is room
//value and typeof(array) must be an assignable type, otherwise use ssa_cat
#define ssa_push(array, value) do { \
    struct ssa_attr *attr = SSA_HDR(array); \
    if(attr->len < attr->alloc*attr->sz) { \
        (array)[attr->len] = (value); \
        attr->len++; \
    } }while(0)

//pop an item off the end of an ssa array, if there is one
#define ssa_pop(array) ({ \
    struct ssa_attr *attr = SSA_HDR(array); \
    attr->len ? (array)[--attr->len] : 0; \
    })

//safely get an element of ssa array at an index
#define ssa_get(array, i) ({ \
    const struct ssa_attr *attr = SSA_HDR(array); \
    const size_t ti = (i); \
    ti < attr->len ? (array)[ti] : 0; \
    })

//safely set an element of ssa array at an index
// value must be assignable, otherwise use ssa_cpy
#define ssa_set(array, i, value) do { \
    const struct ssa_attr *attr = SSA_HDR(array); \
    const size_t ti = (i); \
    if(ti < attr->len) { (array)[ti] = (value); } \
    } while(0)


/************** Internal stuff *************/

//helper method
void* _ssa_new(struct ssa_attr *attr, size_t a_sz, size_t e_sz, const void *init, size_t init_sz);

#endif //_SUC_SSA_H_
