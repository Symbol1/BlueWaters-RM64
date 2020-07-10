
# Project Overview

The entire project of computing the Tutte polynomial of `RM(2, 6)`
can be summarized as follows.
 
* Compute the RREF-signature polynomial of RM32 from scratch.
* Compute the pivot-signature polynomial of RM64 from the former.
* Specialize the former to the Tutte polynomial

This does not answers any question at all, if not posing new.
Let me explain some terminology and concept.

## Reed--Muller codes

Reed--Muller code is a series of linear block codes
that possess symmetric and recursive structures.
In the usual notation, `RM(0, 5)` is the repetition code;
`RM(1, 5)` consists of `RM(0, 5)` plus linear monomials (x₁, x₂, etc);
`RM(2, 5)` consists of `RM(1, 5)` plus quadratic monomials (x₁x₂, x₁x₃, etc).

More figuratively, the recursive structure is a *Pascal's rule*.

```python
                                                RM(6, 6)
                                        RM(5, 5)
                                RM(4, 4)        RM(5, 6)
                        RM(3, 3)        RM(4, 5)
                RM(2, 2)        RM(3, 4)        RM(4, 6)
        RM(1, 1)        RM(2, 3)        RM(3, 5)
RM(0, 0)        RM(1, 2)        RM(2, 4)        RM(3, 6)
        RM(0, 1)        RM(1, 3)        RM(2, 5)
RM(-1,0)        RM(0, 2)        RM(1, 4)        RM(2, 6)
        RM(-1,1)        RM(0, 3)        RM(1, 5)
                RM(-1,2)        RM(0, 4)        RM(1, 6)
                        RM(-1,3)        RM(0, 5)
                                RM(-1,4)        RM(0, 6)
                                        RM(-1,5)
                                                RM(-1,6)
```

For any vertical pair, the upper `RM(r-1, m)` contains the lower `RM(r, m)`.
For any triangle

```python
RM( r , m)
          RM(r, m+1)
RM(r-1, m)
```

the direct sum of the left two is the right one.

It is usually desired to focus on the *increments*:
`RM(r, m)` = `RM(r-1, m)` ⊕ `rm(r, m)`

```python
                                                rm(6, 6)
                                        rm(5, 5)
                                rm(4, 4)        rm(5, 6)
                        rm(3, 3)        rm(4, 5)
                rm(2, 2)        rm(3, 4)        rm(4, 6)
        rm(1, 1)        rm(2, 3)        rm(3, 5)
rm(0, 0)        rm(1, 2)        rm(2, 4)        rm(3, 6)
        rm(0, 1)        rm(1, 3)        rm(2, 5)
                rm(0, 2)        rm(1, 4)        rm(2, 6)
                        rm(0, 3)        rm(1, 5)
                                rm(0, 4)        rm(1, 6)
                                        rm(0, 5)
                                                rm(0, 6)
```

Then `rm(r, m)` has dimension m choose r.
And the left two of any triangle direct sum to the right one.

## Pivot signature polynomial

Fix a code, say `RM(2, 6)`.
Choose a generator matrix such that its last row generates `RM(0, 6)`,
and its last six rows generate `RM(1, 6)`.
In other words, we are stacking the generator matrices
of `rm(2, 6)`, `rm(1, 6)`, and `rm(0, 6)`.

Let `AccessPattern` be the power set of the columns of the generator matrix.
That is, an element `A` of `AccessPattern` is a subset of columns.
`A` is treated as a standalone matrix; compute its RREF.
The pivot signature polynomial is the collection of the positions of the pivots.
In detail, the pivot signature polynomial is

```python
sum(
    product(
        z_p for p in PivotIndicesOf(A)
    )
    for A in AccessPattern
)
```