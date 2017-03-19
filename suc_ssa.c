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
    SSA_ASSERT_INIT(array);
    struct ssa_attr *attr = SSA_HDR(array);
    char *buf = array;
    if(attr->len == new_length) {
        return;
    }
    if(attr->len < new_length) {
        //make sure there is room
        new_length = SUC_MIN(attr->alloc/attr->esz, new_length);
        //need to zero out the new mem
        size_t count = (new_length-attr->len)*attr->esz;
        memset(buf+attr->len*attr->esz, 0, count);
    }
    attr->len = new_length;
}

//copy the contents of other into ssa array at index i
void ssa_cpy(void *array, size_t i, const void *other, size_t other_size)
{
    SSA_ASSERT_INIT(array);
    struct ssa_attr *attr = SSA_HDR(array);
    char *buf = array;
    //if i is too big, other/other_size aren't set
    if((i > attr->alloc/attr->esz) || !other || !other_size) {
        return;
    }
    //if i is beyond the length of the array, zero out the difference
    if(i > attr->len) {
        size_t count = (i-attr->len)*attr->esz;
        memset(buf+attr->len*attr->esz, 0, count);
    }
    char *start = buf+i*attr->esz;
    //end of the allocated array or the end of other
    char *end = SUC_MIN(buf+attr->alloc, start+other_size);
    //copy as many bytes from other as we can
    memcpy(start, other, end-start);
    //only set len if it grew
    size_t new_length = (end-buf)/attr->esz;
    if(new_length > attr->len) {
        attr->len = new_length;
    }
}

//copies a portion of ssa array, from start to the element before end, into ssa slice, replacing its contents
void ssa_slice(const void *array, size_t start, size_t end, void *slice)
{
    SSA_ASSERT_INIT(array);
    SSA_ASSERT_INIT(slice);
    const struct ssa_attr *attr = SSA_HDR(array);
    const char *buf = array;
    struct ssa_attr *oattr = SSA_HDR(slice);
    assert(attr->esz == oattr->esz);
    //end has to be > start, no support for -ve indexes right now
    if(end < start) return;
    size_t slice_sz = (end-start)*attr->esz;
    //indexes must be in bounds, slice length must be <= slice's allocated size
    if((start >= attr->len) || (end >= attr->len) || (slice_sz > oattr->alloc)) {
        return;
    }
    const char *start_ptr = buf+start*attr->esz;
    ssa_cpy(slice, 0, start_ptr, slice_sz);
    //we already checked that slice_sz can fit in slice, so set its new len
    oattr->len = (end-start);
}

//helper method
void* _ssa_new(struct ssa_attr *attr, void *array, size_t alloc, size_t esz, const void *init, size_t init_sz)
{
    //we must init an even number of elements
    assert(init_sz%esz==0);
    assert(alloc%esz==0);
    char *buf = array;
    
    //store the array-ssa_attr in the padding and/or the _pdiff member
    *SSA_PDIFF(array) = buf-(char*)attr;
    //magic number used for kinda meh error checking
    *SSA_PMAGIC(array) = SSA_MAGIC;
    
    attr->alloc = alloc;
    attr->esz = esz;
    
    if(init && init_sz) {
        size_t count = SUC_MIN(alloc, init_sz);
        memcpy(buf, init, count);
        attr->len = count/esz;
    }
    else {
        attr->len = 0;
    }
    //a bit meaningless, but return a ptr to buf
    return array;
}


/*** TEST stuff *****/
#if defined(SUC_TEST_MAIN)
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

struct test_uint{
    struct ssa_attr attr;
    uint32_t array[20];
    uint32_t padding[4];
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
    printf("  esz   %u\n", attr->esz);
}

int main(void) {
    struct test_uint t1, t2;
    memset(t1.padding, 0x5a, sizeof(t1.padding));
    uint32_t *a1, *a2;
    const uint32_t d1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i, j;
    
    puts("\nTest empty ssa creation");
    a1 = ssa_new_empty(&t1.attr, t1.array);
    
    #if 0
    for(char* c=(char*)&t1; c<(char*)t1.array; c++) {
        printf(" t1 %p = %"PRIu8"\n", c, (uint8_t)*c);
    }
    #endif
    printf("*SSA_PDIFF(a1) = %u\n", *SSA_PDIFF(a1));
    
    assert(a1 == t1.array);
    assert(SSA_PDIFF(a1) == SSA_PDIFF(t1.array));
    assert(SSA_HDR(a1) == &t1.attr);
    
    print_ssa_attr(SSA_HDR(a1));
    
    assert(ssa_length(a1) == 0);
    assert(ssa_avail(a1) == SUC_LEN(t1.array));
    
    
    puts("\nTest ssa creation and init");
    a1 = ssa_new(&t1.attr, t1.array, d1, sizeof(d1));
    
    print_ssa_attr(SSA_HDR(a1));
    
    assert(ssa_length(a1) == SUC_LEN(d1));
    assert(ssa_avail(a1) == SUC_LEN(t1.array)-SUC_LEN(d1));
    assert(ssa_size(a1) == sizeof(d1));
    
    puts("\nTest set/get");
    for(i=0; i<ssa_length(a1); i++) {
        printf("a1[%d] = %u = %u\n", i, a1[i], ssa_get(a1, i));
        assert(a1[i] == d1[i]);
        assert(ssa_get(a1, i) == d1[i]);
    }
    //write outside bounds
    a1[i] = 0xdeadbeef;
    assert(ssa_get(a1, i) == 0);
    ssa_set(a1, i, 0xcafebabe);
    assert(a1[i] == 0xdeadbeef);
    
    puts("\nTest resize");
    ssa_resize(a1, ssa_length(a1)+3);
    //make sure the 0xdeadbeef got zeroed out
    assert(ssa_get(a1, i) == 0);
    assert(a1[i] == 0);
    assert(ssa_length(a1) > i);
    ssa_set(a1, i, 0xcafebabe);
    assert(ssa_get(a1, i) == 0xcafebabe);
    
    puts("\nTest clear");
    ssa_clear(a1);
    assert(ssa_length(a1) == 0);
    
    puts("\nTest cpy/cat");
    ssa_cat(a1, d1, sizeof(d1));
    ssa_cat(a1, d1, sizeof(d1));
    assert(ssa_length(a1) == SUC_LEN(d1)*2);
    i = SUC_LEN(d1)+2;
    assert(ssa_get(a1, i) == d1[i%SUC_LEN(d1)]);
    
    //array should only be d1*2 in length
    assert(ssa_avail(a1) == 0);
    
    //this is an illegal access, just to make sure we don't access the padding member
    i = SUC_LEN(t1.array);
    printf("a1[i] too big = 0x%08x\n", a1[i]);
    assert(a1+i == t1.padding);
    assert(a1[i] == t1.padding[0]);
    assert(ssa_get(a1, i) == 0);
    ssa_set(a1, i, 0xcafebabe);
    assert(t1.padding[0] != 0xcafebabe);
    
    
    puts("\nTest cat_ssa/replace");
    //accessing an ssa that hasn't been initialized is illegal, the following should throw an error
    //ssa_cat_ssa(t2.array, a1);
    a2 = ssa_new(&t2.attr, t2.array, a1, ssa_size(a1));
    assert(ssa_length(a2) == ssa_length(a1));
    
    //this won't do anything because array is too small to hold all the values
    i = ssa_length(a2);
    ssa_cat_ssa(a2, a1);
    assert(i == ssa_length(a2));
    
    //clear both, resize a1 and fill with d1, then replace/copy a1 to a2
    ssa_clear(a1);
    ssa_clear(a2);
    memset(a2, 0, SSA_HDR(a2)->alloc);
    ssa_resize(a1, SUC_LEN(d1));
    //should be zeroed
    assert(a1[5] == 0);
    
    ssa_cpy(a1, 0, d1, sizeof(d1));
    assert(ssa_length(a1) == SUC_LEN(d1));
    assert(a1[5] == d1[5]);
    
    ssa_replace(a2, a1);
    assert(ssa_length(a1) == ssa_length(a2));
    for(i = 0; i<ssa_length(a1); i++) {
        assert(ssa_get(a1, i) == ssa_get(a2, i));
    }
    
    //should be room now
    i = ssa_length(a1);
    ssa_cat_ssa(a2, a1);
    assert(ssa_length(a2) == i*2);
    //make sure ssa_new/cat didn't copy that padding
    assert(ssa_get(a2, i) == a1[0]);
    
    
    puts("\nTest slice");
    i = 1;
    j = 5;
    ssa_slice(a2, i, j, a1);
    assert(ssa_length(a1) == j-i);
    assert(a1[0] == ssa_get(a2, i));
    assert(a1[1] == d1[i+1]);
    
    for(i=0; i<ssa_length(a2); i++) {
        printf("a2[%d] = %u\n", i, ssa_get(a2, i));
    }
    for(i=0; i<ssa_length(a1); i++) {
        printf("a1[%d] = %u = %u\n", i, a1[i], ssa_get(a1, i));
    }
    
    
    
    return 0;
}
#endif
