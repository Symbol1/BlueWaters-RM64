
# Data Format

Read [process overview](process.md) before this.
This file explains necessary conventions to decipher the data on Google Drive
<https://drive.google.com/drive/folders/1zYv2R-oqepX1vJ_Fr5JBmrVNdle0mi9M>.

## Pivot-pattern

As we demonstrate in the [process overview](process.md),
a pivot-pattern can be represented by a binary number.
For pivot-patterns of RM64, there are 64 bits, hence `int64_t` suffices.

Similarly, the multiplicity of a pivot-pattern (the coefficient of the monomial)
is (way) less than the number of subsets of rows, 2^64.
Thus the multiplicity/coefficient can be held by a `u_int64_t`.

In summary, the file looks like the following.
Pattern on the left; multiplicity on the right.

```text
fff0ff0001400000         26dd8000
fff0ff0001800000         b43f0000
fff0ff0001c00000          e5ec000
fff0ff0002800000         a0c2c000
fff0ff0002c00000         161f4000
...
```

## RREF-pattern

RREF-pattern is more complicated.
In general, an RREF-pattern of RM32 looks like

```text
[A]
  ⌈B B B B B⌉
  |  B B B B|
  |    B B B|
  |      B B|
  ⌊        B⌋
            ⌈C C C C C C C C C C⌉
            |  C C C C C C C C C|
            |    C C C C C C C C|
            |      C C C C C C C|
            |        C C C C C C|
            |          C C C C C|
            |            C C C C|
            |              C C C|
            |                C C|
            ⌊                  C⌋
                                ⌈D D D D D D D D D D⌉
                                |  D D D D D D D D D|
                                |    D D D D D D D D|
                                |      D D D D D D D|
                                |        D D D D D D|
                                |          D D D D D|
                                |            D D D D|
                                |              D D D|
                                |                D D|
                                ⌊                  D⌋
                                                    ⌈E E E E E⌉
                                                    |  E E E E|
                                                    |    E E E|
                                                    |      E E|
                                                    ⌊        E⌋
                                                              [F]
```

In short:
The `A`-part is either a scaler or empty.
The `B`-part is at most five rows, each of length five.
The `C`-part is at most ten rows, each of length ten.
The rest is symmetric.

To store, say, the `C`-part without too much encoding burdens,
we treat a row as an 10-bit integer.
A `u_int16_t` holds an 10-bit integer.
So the `C` part along requires an array of 10 integers.

Since compiler and cache does not like mixing data types,
all parts are stored by `u_int16_t` for uniformity.
So, we have a total of 32 integers, plus the multiplicity.
THe first 32 integers is no greater than 2^10 = 1024, so `%03x` is good enough.
The multiplicity uses `%4x`.

## Example

For example, the is around line 8,909,373 of the file `rm32rref.txt`:

```text
...
001 010 008 004 002 001 200 104 084 042 020 013 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   40
001 010 008 004 002 001 200 104 084 042 020 013 008 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000  120
001 010 008 004 002 001 200 104 084 042 020 013 008 000 000 000 084 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
001 010 008 004 002 001 200 104 084 042 020 013 009 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000  120
001 010 008 004 002 001 200 104 084 042 020 013 009 000 000 000 095 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000   20
...
```

The first three-digit number is the `A`-part.
the next five groups of three-digits are the `B`-part
(regardless the height of the B-part).
And so on.
The number at the end is the multiplicity.

As an example, the middle line is translated into:

```text
[001]
    ⌈010⌉
    |008|
    |004|
    |002|
    ⌊001⌋
        ⌈200⌉
        |104|
        |084|
        |042|
        |020|
        |013|
        |008|
        |000|
        |000|
        ⌊000⌋
            ⌈084⌉
            |000|
            |000|
            |000|
            |000|
            |000|
            |000|
            |000|
            |000|
            ⌊000⌋
                ⌈000⌉
                |000|
                |000|
                |000|
                ⌊000⌋
                    [000]
```

with multiplicity 32.

We expand the hexadecimal number:

```text
[1]
  ⌈1 0 0 0 0⌉
  |  1 0 0 0|
  |    1 0 0|
  |      1 0|
  ⌊        1⌋
            ⌈1 0 0 0 0 0 0 0 0 0⌉
            |  1 0 0 0 0 0 1 0 0|
            |    1 0 0 0 0 1 0 0|
            |      1 0 0 0 0 1 0|
            |        1 0 0 0 0 0|
            |          1 0 0 1 1|
            |            1 0 0 0|
            |              0 0 0|
            |                0 0|
            ⌊                  0⌋
                                ⌈0 0 1 0 0 0 0 1 0 0⌉
                                |  0 0 0 0 0 0 0 0 0|
                                |    0 0 0 0 0 0 0 0|
                                |      0 0 0 0 0 0 0|
                                |        0 0 0 0 0 0|
                                |          0 0 0 0 0|
                                |            0 0 0 0|
                                |              0 0 0|
                                |                0 0|
                                ⌊                  0⌋
                                                    ⌈0 0 0 0 0⌉
                                                    |  0 0 0 0|
                                                    |    0 0 0|
                                                    |      0 0|
                                                    ⌊        0⌋
                                                              [0]
```

Finally we omit all-zero rows.

```text
[1]
  ⌈1 0 0 0 0⌉
  |  1 0 0 0|
  |    1 0 0|
  |      1 0|
  ⌊        1⌋
            ⌈1 0 0 0 0 0 0 0 0 0⌉
            |  1 0 0 0 0 0 1 0 0|
            |    1 0 0 0 0 1 0 0|
            |      1 0 0 0 0 1 0|
            |        1 0 0 0 0 0|
            |          1 0 0 1 1|
            ⌊            1 0 0 0⌋
                                [0 0 1 0 0 0 0 1 0 0]
```
