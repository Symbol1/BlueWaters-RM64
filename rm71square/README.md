
# Squaring Comment

## Data structure

Use versatile names to indicate purpose.

```C++
typedef u_int16_t matrixrow10_t; // a 10-bit row
typedef u_int32_t matrixrow20_t; // a 20-bit row
typedef u_int64_t pattern64_t;   // pivot-pattern for RM64
```

---

Define the main data structure that holds RREF-

```C++
// rm32 rref comes in two forms
// first the array form
using namespace std;
#include<array>
typedef array<matrixrow10_t,thirtytwo> rref32asarray_t;
// second the block form
typedef struct rref32byblock_t{
    matrixrow10_t block1_____[1];
    matrixrow10_t block_5____[5];
    matrixrow10_t block__a___[10];
    matrixrow10_t block___a__[10];
    matrixrow10_t block____5_[5];
    matrixrow10_t block_____1[1];
    matrixrow10_t*operator[](int block){
        switch(block){
                case 0:return block1_____;
                case 1:return block_5____;
                case 2:return block__a___;
                case 3:return block___a__;
                case 4:return block____5_;
                case 5:return block_____1;
            default:exit(48442);
        }
    };
}rref32byblock_t;
// unify them
union rref32_t{
    rref32asarray_t array;
    rref32byblock_t block;
};
```