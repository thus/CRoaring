/**
* The purpose of this test is to check that we can call CRoaring from C++
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <roaring/roaring.h>
#include "roaring.hh"


void roaring_iterator_sumall(uint32_t value, void *param) {
    *(uint32_t *)param += value;
}

void test_example(bool copy_on_write) {

    // create a new empty bitmap
    roaring_bitmap_t *r1 = roaring_bitmap_create();
    r1->copy_on_write = copy_on_write;
    assert(r1 != NULL);

    // then we can add values
    for (uint32_t i = 100; i < 1000; i++) {
        roaring_bitmap_add(r1, i);
    }

    // check whether a value is contained
    assert(roaring_bitmap_contains(r1, 500));

    // compute how many bits there are:
    uint32_t cardinality = roaring_bitmap_get_cardinality(r1);
    printf("Cardinality = %d \n", cardinality);

    // if your bitmaps have long runs, you can compress them by calling
    // run_optimize
    uint32_t size = roaring_bitmap_portable_size_in_bytes(r1);
    roaring_bitmap_run_optimize(r1);
    uint32_t compact_size = roaring_bitmap_portable_size_in_bytes(r1);

    printf("size before run optimize %d bytes, and after %d bytes\n", size,
           compact_size);

    // create a new bitmap with varargs
    roaring_bitmap_t *r2 = roaring_bitmap_of(5, 1, 2, 3, 5, 6);
    assert(r2 != NULL);

    roaring_bitmap_printf(r2);
    printf("\n");


    // we can also create a bitmap from a pointer to 32-bit integers
    const uint32_t values[] = {2, 3, 4};
    roaring_bitmap_t *r3 = roaring_bitmap_of_ptr(3, values);
    r3->copy_on_write = copy_on_write;

    // we can also go in reverse and go from arrays to bitmaps
    uint64_t card1 = roaring_bitmap_get_cardinality(r1);
    uint32_t *arr1 = new uint32_t[card1];
    assert(arr1  != NULL);
    roaring_bitmap_to_uint32_array(r1, arr1);

    roaring_bitmap_t *r1f = roaring_bitmap_of_ptr(card1, arr1);
    delete[] arr1;
    assert(r1f != NULL);

    // bitmaps shall be equal
    assert(roaring_bitmap_equals(r1, r1f));
    roaring_bitmap_free(r1f);

    // we can copy and compare bitmaps
    roaring_bitmap_t *z = roaring_bitmap_copy(r3);
    z->copy_on_write = copy_on_write;
    assert(roaring_bitmap_equals(r3, z));

    roaring_bitmap_free(z);

    // we can compute union two-by-two
    roaring_bitmap_t *r1_2_3 = roaring_bitmap_or(r1, r2);
    r1_2_3->copy_on_write = copy_on_write;
    roaring_bitmap_or_inplace(r1_2_3, r3);

    // we can compute a big union
    const roaring_bitmap_t *allmybitmaps[] = {r1, r2, r3};
    roaring_bitmap_t *bigunion = roaring_bitmap_or_many(3, allmybitmaps);
    assert(roaring_bitmap_equals(r1_2_3, bigunion));
    roaring_bitmap_t *bigunionheap =
        roaring_bitmap_or_many_heap(3, allmybitmaps);
    assert(roaring_bitmap_equals(r1_2_3, bigunionheap));
    roaring_bitmap_free(r1_2_3);
    roaring_bitmap_free(bigunion);
    roaring_bitmap_free(bigunionheap);

    // we can compute intersection two-by-two
    roaring_bitmap_t *i1_2 = roaring_bitmap_and(r1, r2);
    roaring_bitmap_free(i1_2);

    // we can write a bitmap to a pointer and recover it later
    uint32_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    char *serializedbytes = (char *) malloc(expectedsize);
    roaring_bitmap_portable_serialize(r1, serializedbytes);
    roaring_bitmap_t *t = roaring_bitmap_portable_deserialize(serializedbytes);
    assert(roaring_bitmap_equals(r1, t));
    roaring_bitmap_free(t);
    free(serializedbytes);

    // we can iterate over all values using custom functions
    uint32_t counter = 0;
    roaring_iterate(r1, roaring_iterator_sumall, &counter);
    /**
     * void roaring_iterator_sumall(uint32_t value, void *param) {
     *        *(uint32_t *) param += value;
     *  }
     *
     */

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    roaring_bitmap_free(r3);
}

void test_example_cpp(bool copy_on_write) {
    // create a new empty bitmap
    Roaring r1;
    r1.setCopyOnWrite(copy_on_write);
    // then we can add values
    for (uint32_t i = 100; i < 1000; i++) {
        r1.add(i);
    }

    // check whether a value is contained
    assert(r1.contains(500));

    // compute how many bits there are:
    uint32_t cardinality = r1.cardinality();
    std::cout << "Cardinality = " << cardinality << std::endl;

    // if your bitmaps have long runs, you can compress them by calling
    // run_optimize
    uint32_t size = r1.getSizeInBytes();
    r1.runOptimize();
    uint32_t compact_size = r1.getSizeInBytes();

    std::cout << "size before run optimize " << size << " bytes, and after "
    		<<  compact_size << " bytes." << std::endl;


    // create a new bitmap with varargs
    Roaring r2 = Roaring::bitmapOf(5, 1, 2, 3, 5, 6);

    r2.printf();
    printf("\n");

    // we can also create a bitmap from a pointer to 32-bit integers
    const uint32_t values[] = {2, 3, 4};
    Roaring r3 = Roaring::fromUint32Array(3, values);
    r3.setCopyOnWrite(copy_on_write);

    // we can also go in reverse and go from arrays to bitmaps
    uint64_t card1 = r1.cardinality();
    uint32_t *arr1 = new uint32_t[card1];
    assert(arr1  != NULL);
    r1.toUint32Array(arr1);
    Roaring r1f = Roaring::fromUint32Array(card1, arr1);
    delete[] arr1;

    // bitmaps shall be equal
    assert(r1 == r1f);

    // we can copy and compare bitmaps
    Roaring z (r3);
    z.setCopyOnWrite(copy_on_write);
    assert(r3 == z);

    // we can compute union two-by-two
    Roaring r1_2_3 = r1 | r2;
    r1_2_3.setCopyOnWrite(copy_on_write);
    r1_2_3 |= r3;

    // we can compute a big union
    const Roaring *allmybitmaps[] = {&r1, &r2, &r3};
    Roaring bigunion = Roaring::fastunion(3, allmybitmaps);
    assert(r1_2_3 == bigunion);

    // we can compute intersection two-by-two
    Roaring i1_2 = r1 & r2;

    // we can write a bitmap to a pointer and recover it later

    uint32_t expectedsize = r1.getSizeInBytes();
    char *serializedbytes = new char [expectedsize];
    r1.write(serializedbytes);
    Roaring t = Roaring::read(serializedbytes);
    assert(r1 == t);
    delete[] serializedbytes;

    // we can iterate over all values using custom functions
    uint32_t counter = 0;
    r1.iterate(roaring_iterator_sumall, &counter);
    /**
     * void roaring_iterator_sumall(uint32_t value, void *param) {
     *        *(uint32_t *) param += value;
     *  }
     *
     */

}



int main() {
  test_example(true);
  test_example(false);
  test_example_cpp(true);
  test_example_cpp(false);

  return EXIT_SUCCESS;
}
