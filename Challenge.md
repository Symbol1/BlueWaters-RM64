
# Challenge the computation

**Blue Waters** is a petascale supercomputer
at the National Center for Supercomputing Applications (NCSA)
at the University of Illinois at Urbana-Champaign.

The [64, 22, 16]-Reed-Muller code is also known as RM(2, 6).
We compute its Tutte polynomial and more general invariants on Blue Waters.

The computation costs about 2,000 node-hours.
We divide the computation (not quite evenly) into about 48,520 subjobs.
All subjobs are feasible on descent laptops;
most of them costs less than 10 node-minuts.
Challengers are invited to double-check any subset of subjobs.

## Hypothesis testing

* **Null hypothesis:**
  We made a systematic mistake that affects 1% of subjobs.

* **Alternative hypothesis:**
  We did not make such mistake.

Assume a challenger randomly selects 70 out of 48,520 subjobs
and reruns them correctly (which should cost one laptop-night).
Then the probability that it does not spot a mistake is
> 99%^70 = 0.495.

If, instead, the challenger randomly selects
and reruns 300 subjobs (a laptop-week),
then the probability that it does not spot a mistake is
> 99%^300 = 0.049.

This is usually considered statistically significant.

## Data

https://drive.google.com/drive/folders/1zYv2R-oqepX1vJ_Fr5JBmrVNdle0mi9M?usp=sharing
