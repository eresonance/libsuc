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

#include "suc_ssa.h"
#include "suc_macros.h"

//resize length, zeroing new elements if expanding
void ssa_resize(void *array, size_t new_length)
{
    struct ssa_attr *attr = SSA_HDR(array);
    if(attr->len == new_length) {
        return;
    }
    if(attr->len < new_length) {
        //make sure there is room
        new_length = SUC_MIN(attr->alloc/attr->esz, new_length);
        //need to zero out the new mem
        size_t count = (new_length-attr->len)*attr->esz;
        memset(attr->buf+attr->len*attr->esz, 0, count);
    }
    attr->len = new_length;
}

//copy the contents of other into ssa array at index i
void ssa_cpy(void *array, size_t i, const void *other, size_t other_size)
{
    struct ssa_attr *attr = SSA_HDR(array);
    //if i is too big, other/other_size aren't set
    if((i > attr->alloc/attr->esz) || !other || !other_size) {
        return;
    }
    //if i is beyond the length of the array, zero out the difference
    if(i > attr->len) {
        size_t count = (i-attr->len)*attr->esz;
        memset(attr->buf+attr->len*attr->esz, 0, count);
    }
    char *start = attr->buf+i*attr->esz;
    //end of the allocated array or the end of other
    char *end = SUC_MIN(attr->buf+attr->alloc, start+other_size);
    //copy as many bytes from other as we can
    memcpy(start, other, end-start);
    //only set len if it grew
    size_t new_length = (end-attr->buf)/attr->esz;
    if(new_length > attr->len) {
        attr->len = new_length;
    }
}

//copies a portion of ssa array to ssa slice, replacing its contents
void ssa_slice(const void *array, size_t start, size_t end, void *slice)
{
    const struct ssa_attr *attr = SSA_HDR(array);
    struct ssa_attr *oattr = SSA_HDR(slice);
    assert(attr->esz == oattr->esz);
    //end has to be > start, no support for -ve indexes right now
    if(end < start) return;
    size_t slice_sz = (end-start)*attr->esz;
    //indexes must be in bounds, slice length must be <= slice's allocated size
    if((start >= attr->len) || (end >= attr->len) || (slice_sz > oattr->alloc)) {
        return;
    }
    const char *start_ptr = attr->buf+start*attr->esz;
    ssa_cpy(slice, 0, start_ptr, slice_sz);
    //we already checked that slice_sz can fit in slice, so set its new len
    oattr->len = (end-start);
}

//helper method
void* _ssa_new(struct ssa_attr *attr, size_t alloc, size_t esz, const void *init, size_t init_sz)
{
    //we must init an even number of elements
    assert(init_sz%esz==0);
    assert(alloc%esz==0);
    
    attr->alloc = alloc;
    attr->esz = esz;
    
    if(init && init_sz) {
        size_t count = SUC_MIN(alloc, init_sz);
        memcpy(attr->buf, init, count);
        attr->len = count/esz;
    }
    else {
        attr->len = 0;
    }
    //a bit meaningless, but return a ptr to buf
    return attr->buf;
}


/*** TEST stuff *****/
#if defined(SUC_TEST_MAIN)
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

struct test_uint{
    struct ssa_attr does_not_matter;
    uint32_t array[100];
};

struct node {
    int x;
    int y;
};

struct test_node{
    struct ssa_attr does_not_matter;
    struct node array[100];
};

void print_ssa_attr(struct ssa_attr *attr)
{
    printf("ptr     %p\n", attr);
    printf("  alloc %zu\n", attr->alloc);
    printf("  len   %zu\n", attr->len);
    printf("  esz   %zu\n", attr->esz);
}

int main(void) {
    struct test_uint t1;
    uint32_t *a1 = ssa_new_empty(&t1, array);
    print_ssa_attr(SSA_HDR(a1));
    assert(ssa_length(a1) == 0);
    assert(ssa_avail(a1) == sizeof(t1.array)/sizeof(t1.array[0]));
}
#endif
