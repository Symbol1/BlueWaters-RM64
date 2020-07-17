
# Squaring Comment

## Data structure

Use versatile type names to indicate purpose.

```C++
typedef u_int16_t matrixrow10_t; // a 10-bit row
typedef u_int32_t matrixrow20_t; // a 20-bit row
typedef u_int64_t pattern64_t;   // pivot-pattern for RM64
```

---

Define the main data structure that holds RREF-patterns

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

This is the same structure for RM32 RREF-patterns
as in the [rm32rref computation](../rm34rref).

---

We are to read in the RREF-signature polynomial of RM32
compute the pivot-signature polynomial of RM64.
These are the dedicated type for polynomials.

```C++
// our beloved pivot/rref signature polynomial is an std::map
typedef struct rref_coef_pair_t{
    rref32_t rref;
    coef64_t coef;
}rref_coef_pair_t;
typedef rref_coef_pair_t Poly32_t[fiveseventwoninefive];
#include<map>
typedef map<pattern64_t,coef64_t> Poly64_t;
```

---

Define addition.

```C++
#include<map>
typedef map<pattern64_t,coef64_t> Poly64_t;

// declare how to add polynomial in C++
Poly64_t&Poly64_add(Poly64_t&left,Poly64_t&right){
    for(const auto&piv_coe_pair:right){
        left[piv_coe_pair.first]+=piv_coe_pair.second;
    }
    right.clear();
    return left;
}
// declare how to add polynomial again for openmp
#pragma omp declare reduction(omp_poly64_add:Poly64_t:omp_out=Poly64_add(omp_out,omp_in))
```

Same as before, we rely on the openmp magic
to sum up local polynomials from all threads.

---

The Gaussian elimination.

```C++
// Very Efficient Gaussian Elimination
pattern64_t GaussianElimination(matrixrow20_t*WorkingMatrix,int height,int width){
    //                                       ^                 ^the dimension of the matrix
    //                                       ^                  need to be boundary-safe
    //                                       ^the working space as a pointer
    //                                        resulting RREF will be here
```

On my laptop, this function is called about 5 millions times per second.

## int main

Initialize MPI magic.

```C++
    // MPI has highest priority
    MPI_Init(NULL,NULL);
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD ,&comm_size);
    int comm_rank;
    MPI_Comm_rank(MPI_COMM_WORLD ,&comm_rank);
    printf("MPI rank %d / size %d \n",comm_rank,comm_size);
```

## Outer MPI loop

The outer loop is for MPI magic.
We rent 50--200 nodes at a time and each node becomes one MPI unit.
(Within node, openmp works to distribute loads to its threads.)

```C++
    // the loop for different pair of subtxt
    for(int subtxt_no=comm_rank;subtxt_no<1;subtxt_no+=comm_size){
        int subtxt_no_TTT=(24+subtxt_no/onefivesix)%threeoneone;
        int subtxt_no_SSS=(subtxt_no_TTT+subtxt_no%onefivesix)%threeoneone;
```

This is an implicit forking.
Here, `comm_rank` is the id of the current node;
and `comm_size` is the total number of nodes.
Thus the for loops distributes loads across all nodes, evenly.

---

My laptop has 8 threads; Blue Waters has 32.
The program later uses that to detect
if it is running on my laptop or Blue Waters.

```C++
        // set up omp, will use this to determine where am I
        int max_threads=omp_get_max_threads();
        omp_set_num_threads(max_threads);
```

---

Read the sub-polynomial from the file numbered `subtxt_no_TTT`.

```C++
        // read subtxt, the ttt term
        Poly32_t RREF_Signature_TTT;
        coef64_t coefficient_ttt_acc=0;
```

---

Read the sub-polynomial from the file numbered `subtxt_no_SSS`.

```C++
        // read subtxt, the sss serm
        Poly32_t RREF_Signature_SSS;
        coef64_t coefficient_sss_acc=0;
```

## Inner openmp loop

Iterate through all monomials from the first file;
inside that, iterate through all monomials from the second file.

```C++
        // start the rref-pair loop
        Poly64_t Pivot_Signature_Counter; // our purpose is to compute this
        // exhaus all ordered pair
        #pragma omp parallel for reduction(omp_poly64_add:Pivot_Signature_Counter)
        for(int term=0;term<fiveseventwoninefive;term+=1){
            for(int serm=0;serm<fiveseventwoninefive;serm+=1){
                rref32byblock_t TERM=RREF_Signature_TTT[term].rref.block;
                rref32byblock_t SERM=RREF_Signature_SSS[serm].rref.block;
```

---

Perform RREF.
Doing so block-wise is faster.

```C++
                pattern64_t pivot_pattern=0;
                for(int block=0;block<=five;block++){
                    matrixrow20_t PlotkinBlock[2*binomial5choose[block]]; // claim working space
                    // no need to initialize because we will copy data and keep track of size
                    int rank_acc=0;
                    for(int subrow=0;subrow<binomial5choose[block]&&TERM[block][subrow];subrow++){
                        PlotkinBlock[rank_acc]=TERM[block][subrow]*(1<<binomial5choose[block]);
                        rank_acc++;
                    }
                    for(int subrow=0;subrow<binomial5choose[block]&&SERM[block][subrow];subrow++){
                        PlotkinBlock[rank_acc]=SERM[block][subrow]*(1<<binomial5choose[block]|1);
                        rank_acc++;
                    }
                    pattern64_t pivot_subpat=GaussianElimination(PlotkinBlock,rank_acc,2*binomial5choose[block]);
                    pivot_pattern|=pivot_subpat<<(2*sumafter5choose[block]);
                }
```

---

opemmp magic happens here.

```C++
                Pivot_Signature_Counter[pivot_pattern]+=RREF_Signature_TTT[term].coef*RREF_Signature_SSS[serm].coef;
            }
        }
```

## Pickling

The rest is to output the result.
For any pair of input files, we output one file.
For instance, if the inputs are `rm35rref123.txt` and `rm35rref456.txt`,
respectively, the output will be `rm64pivot123x456.txt`.

We later merge these files into one `rm64pivotall.txt`.
It is important that we do not delete the intermediate files
(`rm64pivot123x456.txt` and its friends).
So you, or anyone else, can double check our result by just picking two numbers
(e.g., `121` and `343`) and generate their `rm64pivot121x343.txt`.
