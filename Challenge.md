
# Challenge the computation

Our computation assumes some mathematical shortcuts
and exploits bitwise operations.
Chances are that we made mistakes that lead to a wrong result.
There are several scientific ways to challenge our computation, including
[checking against weight distribution](#Weight-ditribution),
[direct sampling](#Direct-sampling), and
[hypothesis testning](#Hypothesis-testing).

## Weight distribution

The [weight distribution](https://en.wikipedia.org/wiki/Enumerator_polynomial)
of a code is an invariant that is easier to compute.
For RM(2, 6) in our case, the weight distribution is given by
[Magma](https://en.wikipedia.org/wiki/Magma_(computer_algebra_system)).
It is backed up by some coding theory and is trust worthy.

The Tutte polynomial uniquely determines the weight distribution.
So it suffices to reduce our Tutte polynomial to weight distribution
and see if it coincides with that given by Magma.

## Direct Sampling

Recall that the Tutte polynomial is defined as
```python
sum(
    x^(corank of E) * y^(nullity of E)
             for E in ErasurePatterns
)
```
where `ErasurePatterns` contains all 2^64 subsets
of the 64 columns of the generator matrix.

Enumerating all 2^64 possibilities is not currently feasible.
But a challenger can sample, say, one million subsets per second.
Note that the Tutte polynomial has at most 989 monomials.
Sampling billions of subsets should give us a good approximation.
And then we compare the approximation to our result.

## Hypothesis testing

The computation costs 2,000 node-hours.
We divide the computation into 48,520 subjobs.
All subjobs are feasible on descent laptops;
most of them costs less than 3 node-minuts.
Challengers are invited to rerun any subset of subjobs.

* **Null hypothesis:**
  We made a systematic mistake that affects 1% of subjobs or more.

* **Alternative hypothesis:**
  Less than 1% of subjobs are broken.

Assume a challenger randomly selects 70 out of 48,520 subjobs
and reruns them correctly (which should cost one laptop-day).
Then the probability that it does not spot a mistake is
> 99%^70 = 0.495.

If, instead, the challenger randomly selects
and reruns 300 subjobs (a laptop-week),
then the probability that it does not spot a mistake is
> 99%^300 = 0.049.

This is usually considered statistically significant.

The data is uploaded to Google Drive  
https://drive.google.com/drive/folders/1zYv2R-oqepX1vJ_Fr5JBmrVNdle0mi9M?usp=sharing




