
# RM32 RREF Comment

Read [process overview](../process.md) before this.

## Data structure

The correct way to declare `int array32[32];`
(we need to specify the length for later usage).

```C++
#include<array>
typedef array<uint16_t,thirtytwo> rref32asarray_t;
```

---

The data structure that holds an RREF-pattern.
See [data-format](../format.md) for big pictures.

```C++
typedef struct{
    u_int16_t block1_____[1];
    u_int16_t block_5____[5];
    u_int16_t block__a___[10];
    u_int16_t block___a__[10];
    u_int16_t block____5_[5];
    u_int16_t block_____1[1];
    u_int16_t*operator[](int block){
        switch(block){
            case 0:return block1_____;
            case 1:return block_5____;
            case 2:return block__a___;
            case 3:return block___a__;
            case 4:return block____5_;
            case 5:return block_____1;
            default:exit(56672);
        }
    };
}rref32byblock_t;
```

I override the `[]` operator so I can
iterate the blocks (of various sizes) using simple for-loops.
So for instance, `rref_holder[0][0]` is the upper left scaler;
`rref_holder[1][0]`, ..., `rref_holder[1][4]` are the next block;
`rref_holder[2][0]`, ..., `rref_holder[1][9]` are the next block;
and so on.

I forgot why I must use an indirect tree-structure
in place of a 2D array `u_int16_t blocks[6][10]`.
It was either to reduce the size to fit it into memory/cache,
or to accelerate the in/equality comparison through  not having too many zeros.

There is a good reason for not using bit field here---some compiler
will attempt to align a block at the default size of int.
In general, that incurs unpredictable (implementation-dependent) size.

---

Now we can unify the array and the tailor-made structure.

```C++
union rref32_t{
    rref32asarray_t array;
    rref32byblock_t block;
};
````

So the type `rref32_t` can be accessed from two sides.
On one side, it is a vector (an array of fixed length).
A vector can be the key to sorting and hash table.
On the other, it bears the block-wise decomposition.

---

The RREF-signature polynomial is implemented as an `std::map`.
An `std::map` is essentially python dictionary.

```C++
// our beloved pivot/rref signature polynomial is an std::map
#include<map>
typedef map<rref32asarray_t,uint32_t> Poly32_t;
```

The key is the RREF-pattern;
note that we use the array-side of the union type because built-in `operator<`.
The value is the multiplicity.

---

We want to specify how to add polynomials---by
matching the keys and adding the values.

```C++
// declare how to add polynomial in C++
Poly32_t&Poly32_add(Poly32_t&left,Poly32_t&right){
    for(const auto&piv_coe_pair:right){
        left[piv_coe_pair.first]+=piv_coe_pair.second;
    }
    right.clear();
    return left;
}
```

The for loop is equivalent to python `for piv_coe_pair in right:`,
where `left` is a polynomial.
So each `piv_coe_pair` is a monomial `<RREF-pattern,multiplicity>`.
Use `.first` and `.second` to access the components;
and add that term to the `left`.

---

The defined polynomial addition is passed to openmp.

```
// declare how to add polynomial again for openmp
#pragma omp declare reduction(omp_poly32_add:Poly32_t:omp_out=Poly32_add(omp_out,omp_in))
```

The design is, each thread will deal with
a small portion of access patterns.
So each thread will end up with a partial RREF-signature polynomial.
We want to add those polynomials at the openmp level to obtain the full.

## Gauss–Jordan elimination

We define Gauss–Jordan elimination.
(The return value is unused.)

```C++
// Very Efficient Gaussian Elimination
pattern32_t GaussianElimination(uint32_t*WorkingMatrix,int height,int width){
    //                                   ^                 ^the dimension of the matrix
    //                                   ^                  need to be boundary-safe
    //                                   ^the working space as a pointer
    //                                    resulting RREF will be here
```

Each row of the matrix is stored as an `uint32_t`.
So the whole matrix is passed as an array, by reference.
We are to perform elimination on-site.

## int main

Prepare the matrix.
Columns with more `1` are moved to the left.
But we just specify the final result.

```C++
    // prepare matrix
    int monomials_in_order[thirtytwo]={31,30,29,27,23,15,28,26,22,
        14,25,21,13,19,11,7,24,20,12,18,10,6,17,9,5,3,16,8,4,2,1,0};
    int ReedMullerMother[thirtytwo]={};
    for(int row=0;row<thirtytwo;row++){
        for(int col=thirtytwo-1;col>=0;col--){ // col for column
            int mon=monomials_in_order[col];
            ReedMullerMother[row]+=int((row&mon)==0)<<col;
        }
    }
```

---

We are about to start the main loop that go over all access patterns.

```C++
    //          start the rref loop
    printf("    start the rref loop \n");
    printf("    ");
    Poly32_t RREF_Signature_Counter; // our purpose is to compute this
    // exhaus all erasure pattern
    #pragma omp parallel for reduction(omp_poly32_add:RREF_Signature_Counter)
```

The loop will compute the RREF-pattern for each pattern
and insert into `RREF_Signature_Counter`.
Note that openmp will distribute the loop to multiple threads evenly.
Each thread will then maintain its local copy of `RREF_Signature_Counter`.
At the end, all `RREF_Signature_Counter` are added together
using the addition function above.

## The main loop

```C++
    for(pattern64_t row_era_pat=(1L<<thirtytwo)-1;row_era_pat>=0;row_era_pat-=1234){ // era for erasure pattern
        if(row_era_pat%(1<<20)==0)printf("%lld ",row_era_pat>>20);
        if(row_era_pat%(1<<26)==0)printf("\n    ");
```

Erasure pattern is the complement of the access pattern.
This program is my local version, so `row_era_pat-=1234`.
On Blue Waters, it is `-=1`.
The `printf` are to give feedback to stdout.

---

keep or delete rows according to the erasure pattern.

```C++
        // copy the preserved matrix to working space
        int cardinality_acc=0; // tracking the number of rows that are preserved when erasing (i.e. cardinality in matroid language)
        uint32_t RMpreserved[thirtytwo+1]={}; // preserved rows go to here, the working space.
        //+1 because when compressing, need an empty room
        for(int row=0;row<thirtytwo;row++){ // check row by row
            if((row_era_pat&(1<<row))==0){ // if row-th row is preserved (i.e. if not erased)
                RMpreserved[cardinality_acc]=ReedMullerMother[row];
                cardinality_acc+=1;
            }
        }
```

`RMpreserved` holds the submatrix.

---

```C++
        GaussianElimination(RMpreserved,cardinality_acc,thirtytwo);
```

Now the elimination is done.
The result is in `RMpreserved`.

---

Transcribe the result in `RMpreserved` to the tailor-made structure.
And add that pattern to the polynomial.

```C++
        // new a rref holder to hold the result
        rref32_t rref_holder;
        for(int i=0;i<thirtytwo;i++){
            rref_holder.array[i]=0;
        }
        // compress rows into blocks
        for(int row=0,block=0;block<=five;block++){ // operate per block
            for(int subrow=0;RMpreserved[row]>>sumafter5choose[block];subrow++,row++){
                rref_holder.block[block][subrow]=RMpreserved[row]>>sumafter5choose[block];
                assert(subrow<binomial5choose[block]);
                assert(row<thirtytwo);
            }
        }
        // push rref_result into the map
        RREF_Signature_Counter[rref_holder.array]+=1;
```

`row` is the overall row index;
`subrow` is the row index within a block.
Notice how we use `.array` and `.block` interchangeably.

---

openmp magic (synthesizing the polynomial) happens here.

```C++
    }
```

## Pickling

The rest is to printf the polynomial to a file.
And other programs will pick up the work.
See [data format](../format.md) and [directories on BW](../directory.md)
for exactly how the intermediate data is transmitted.

For the record, the output is called `rm32rref.txt`.
It contains 17,818,745 lines.
It begins with:

```text
000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000    1
001 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
001 001 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   10
001 002 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   10
001 002 001 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
...
```

The first line is the monomial corresponding to the all-erased pattern.
The second line is 32 ways to choose a single row.

At 7% (line 1,411,180).

```
001 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 010 008 004 002 000 000   10
001 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 010 008 004 002 001 000   20
001 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 010 008 004 002 001 001    1
001 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 010 008 004 003 000 000   10
001 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 200 100 080 040 020 010 008 004 002 001 010 008 005 000 000 000   20
```

Entries in the middle (50%, line 8,909,373) have higher multiplicities.

```text
...
001 010 008 004 002 001 200 104 084 042 020 013 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   40
001 010 008 004 002 001 200 104 084 042 020 013 008 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000  120
001 010 008 004 002 001 200 104 084 042 020 013 008 000 000 000 084 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
001 010 008 004 002 001 200 104 084 042 020 013 009 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000  120
001 010 008 004 002 001 200 104 084 042 020 013 009 000 000 000 095 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
...
```

The file `rm32rref.txt` ends with:

```text
...
001 01d 003 000 000 000 0df 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000    8
001 01e 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   10
001 01e 001 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
001 01e 001 000 000 000 04b 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000    8
001 01f 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   10
...
```

The last line does not correspond to the RREF of the full generator matrix.
The full RREF-pattern is at 7% (see above).
