
# Process Overview

The entire process of computing the Tutte polynomial of `RM(2, 6)`
can be summarized as follows.

* Compute the RREF-signature polynomial of RM32 from scratch (hard).
* Compute the pivot-signature polynomial of RM64 from the former
    (VERY DIFFICULT).
* Reduce the former to the Tutte polynomial (very easy).

This does not answers any question at all, if not posing new.
Let me explain some terminology and concept.

## Reed--Muller codes

Reed--Muller code is a series of linear block codes
that possess symmetric and recursive structures.
In the usual notation, `RM(0, 5)` is the repetition code;
`RM(1, 5)` consists of `RM(0, 5)` plus linear monomials (z₁, z₂, etc);
`RM(2, 5)` consists of `RM(1, 5)` plus quadratic monomials (z₁z₂, z₁z₃, etc).

More figuratively, the recursive structure is a *Pascal's rule*.

```text
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

```text
RM( r , m)
          RM(r, m+1)
RM(r-1, m)
```

the direct sum of the left two is the right one.

It is usually desired to focus on the *increments*:
`RM(r, m)` = `RM(r-1, m)` ⊕ `rm(r, m)`

```text
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

## Pivot-signature polynomial

Fix a code length, say 2^6 = 64.
Choose a generator matrix such that

* it generates `RM(6, 6)`,
* its last 57 columns generates `RM(5, 6)`,
* ...
* its last seven columns generate `RM(1, 6)`, and
* its last column generates `RM(0, 6)`.

In other words, we are augmenting the generator matrices
of `rm(6, 6)`, `rm(5, 6)`, ..., and `rm(0, 6)`.

Let `AccessPattern` be the power set of the rows of the generator matrix.
That is, an element `A` of `AccessPattern` is a subset of rows.
`A` is treated as a standalone matrix; compute its RREF.
The pivot-signature polynomial is the collection of the positions of the pivots.
In detail, the pivot-signature polynomial is

```python
sum(
    product(
        z_p for p in PivotIndicesOf(A)
    )
    for A in AccessPattern
)
```

where z₁, z₂, ... are variables.

This polynomial encodes the info we want to know,
How many pivots lies in the `rm(r, 6)` region for each r?
And how does the pattern of the pivots behave?

Note that there is an easy way to represent the pivot pattern.
Simply write a binary number `1110010...0` to represent the case
where the first three columns have pivots, but not the next two,
yet the next is a pivot, and the rest are not.

Therefore, the pivot-signature polynomial can be encoded
by a file with many lines, each line containing a monomial (i.e., pivot pattern)
and the associated coefficient (i.e., multiplicity).
For example,

```text
fff0ff0001400000         26dd8000
fff0ff0001800000         b43f0000
fff0ff0001c00000          e5ec000
fff0ff0002800000         a0c2c000
fff0ff0002c00000         161f4000
...
```

Of course hexadecimal saves resources;
and there is no need to put the prefix `0x` if it is the default.

## RREF-signature polynomial

One could go one step further and ask, instead of the patterns,
How does the RREF itself distribute?
This is not necessary a wise question to ask because the RREFs are all distinct;
there is no *statistics* if all samples are unique in its own.

However, there is a relief---instead of the full RREF,
we are interested in RREFs restricted to certain blocks.
For instance, consider a pivot pattern `11100101000...`.
Instead of an RREF of the form

```text
1 0 0 a d 0 g 0 k p u . . .
  1 0 b e 0 h 0 l q v . . .
    1 c f 0 i 0 m r w . . .
          1 j 0 n s x . . .
              1 o t y . . .
                      . . .
```

we are interested the RREF w.r.t. the block decomposition `1 + 4 + 6 + 4 + 1`:

```text
[1]
   [1 0 b e]
   [  1 c f]
            [1 j 0 n s x]
            [    1 o t y]
                         [. . . .]
                                  [.]
```

What, you ask, is all of these for?
Well, the answer is that, if we define the RREF-signature polynomial properly,
then the RREF-signatures of RM32 determine the pivot-signatures of RM64.
Notationally,

```text
Pivot_Sign( RM64 ) = Simplify( RREF_Sign( RM32 )^2 )
```

## The squaring process

One might ask, Why the pivot-signatures of RM64
is related to the RREF-signatures of RM32?
Essentially, this is because
we can define RM64 using RM32 following the recursive rule.

In fact, we see RREF-signatures as whatever that is (necessary and) sufficient
to understand the pivot-signatures of the next level.
We obtain a proper definition of RREF-signatures by
reverse-engineering the recursive structure of Reed--Muller codes.

Note that `RREF_Sign( RM32 )` does not determine `RREF_Sign( RM64 )`;
this method is not inductive.
As a result, if we want to understand `Pivot_Sign( RM128 )`,
we will have to build the understanding of `RREF_Sign( RM64 )` from scratch.
