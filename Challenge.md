
# Challenge the computation

There are several scientific ways to double check our computation,
[hypothesis testning](#Hypothesis-testing) is one of them.

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
