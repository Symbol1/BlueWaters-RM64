
# Challenge the Computation

Our computation assumes some mathematical shortcuts
and exploits bitwise operations.
Chances are that we made mistakes that lead to a wrong result.
There are several scientific ways to challenge our computation, including
[checking against weight enumerator](#Weight-enumerator),
[naïve sampling](#Naïve-sampling), and
[hypothesis testing](#Hypothesis-testing).

## Weight enumerator

The *[weight enumerator polynomial]* (sometimes *weight enumerator*)
of a code is an invariant that is fairly easy to compute.
For RM(2, 6), our target code, the weight enumerator
can be computed by [Magma], a mathematical software.
Algorithms in Magma are based on descent coding theory and are trustworthy.

The *[Tutte--Whitney polynomial]* (sometimes *Tutte polynomial*)
of a code uniquely determines its weight enumerator.
To support that our Tutte polynomial is computed correctly,
we reduce our Tutte polynomial to a weight distribution.
The latter is checked against Magma and passes the test.

For the record, the weight enumerator is

```python
       x^64      +    2604*x^48*y^16 + 291648*x^40*y^24 +
888832*x^36*y^28 + 1828134*x^32*y^32 + 888832*x^28*y^36 +
291648*x^24*y^40 +    2604*x^16*y^48 +             y^64
```

## Naïve sampling

Recall that the Tutte polynomial
is defined as

```python
sum(
    x^(corank of A) * y^(nullity of A) for A in AccessPatterns
)
```

where `AccessPatterns` contains all subsets
of the columns of a generator matrix.
For a code of length 64---for instance RM(2, 6)---there are 2^64 subsets.
Enumerating all 2^64 possibilities is not feasible
for computers today (2020 CE).
So this is not the way we computed the exact Tutte polynomial.

On the other hand, modern laptops can sample,
say, one million subsets per second.
So a challenger can sample sufficiently many subsets
and check if the relative frequency of x^c*y^n for any c, n.

Note that the Tutte polynomial has at most 989 monomials.
Therefore, sampling billions of subsets
should give a good approximation of the true distribution.

## Hypothesis testing

The full computation costs 2,000 node-hours and is costly to redo.
To make the computation falsifiable, we divide it into 48,520 sub-jobs;
most of them cost less than 3 node-minutes.
That makes each sub-job replicable on modern laptops.
Challengers are invited to rerun any subset of sub-jobs.
Here is a possible setup.

* **Null hypothesis:**
  We made a systematic mistake that breaks 1% of sub-jobs or more.

* **Alternative hypothesis:**
  Either we made no mistake or less than 1% of sub-jobs are broken.

Assume a challenger randomly selects 70 out of 48,520 sub-jobs
and reruns them (which should cost one laptop-day).
Then the probability that it does not spot a mistake is
> 99% ^ 70 ≈ 49.5%.

If, instead, the challenger randomly selects and reruns 300 sub-jobs
(a laptop-week), then the probability that it does not spot a mistake is
> 99% ^ 300 ≈ 4.9%.

This is usually considered statistically significant.

The sub-jobs data are available on my [Google Drive].
See [process overview](process.md) for detailed organization of sub-jobs.

### Remark

The hypothesis testing idea is widely applicable.
See, e.g., [Miller--Rabin primality test].

[weight enumerator polynomial]: https://en.wikipedia.org/wiki/Enumerator_polynomial
[Magma]: https://en.wikipedia.org/wiki/Magma_(computer_algebra_system)
[Tutte--Whitney polynomial]: https://en.wikipedia.org/wiki/Tutte_polynomial
[Miller--Rabin primality test]: https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
[Google Drive]: https://drive.google.com/drive/folders/1zYv2R-oqepX1vJ_Fr5JBmrVNdle0mi9M
