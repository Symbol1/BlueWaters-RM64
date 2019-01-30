# BlueWaters-baus

Computation of Reed-Muller codes of length 64 on BlueWaters



## Directories on Blue Waters


* `helloworld`, `rm16test`,`rm17pivot`,`rm32pivot`  
  Testing.

* `rm32rref`, `rm33rref`  
  Trial computation.
  Debug (overflow), optimize the code (pass by reference), 
  and check against the pivot-signatutre etc.

* `rm34rref`  
  The RREF-signature polynomial of RM32 is computed here.
  The computation is basically enumerating all 2^32 subsets.
  The resulting file, `rm32rref.txt`, has 17,818,745 lines
  (i.e. 17,818,745 monomials in a polynomial).
  After compressing, `rm32rref.zip` is 65.5Mib.
  * `rm32rref.txt`
  * `rm32rref.zip`

* `rm35rref`  
  Since `rm32rref.txt` has 17,818,745 lines,
  we divide then evenly into 311 files of the form `rm35rref123.txt`.
  Each file represents a subset of 57,295 monomials of the polynomial.
  * `rm35rref000.txt`
  * `rm35rref001.txt`
  * etc
  * `rm35rref310.txt`

* `rm64square`, ... , `rm70square`, `rm70tutte`    
  Wrong computation.
  (Due to overflow of bitfiled.)

* `rm71square`  
  From RREF-signature polynomial of RM32
  we want to the pivot-signature polynomial of RM64.
  The computation is like computing the square of the polynomial.
  Since we devide the polynomial into 311 *sub*polynomials,
  it suffices to compute the product of any two *sub*polynomials.
  The resulting files are called `rm64pivot123x234.txt` or so.
  See the @@@@@@ part.
  * `rm64pivot000x000.txt`
  * `rm64pivot000x001.txt`
  * vdots
  * `rm64pivot000x155.txt`
  * (Note that `000x156` does not exist because it is named `156x000`)
  * (Note that `001x000` does not exist because `000x001` does.)
  * `rm64pivot001x001.txt`
  * `rm64pivot001x002.txt`
  * vdots
  * `rm64pivot001x156.txt`
  * (Note that `001x157` does not exist because it is named `157x001`)
  * (Note that `002x000` does not exist because `000x002` does.)
  * (Note that `002x001` does not exist because `001x002` does.)
  * `rm64pivot002x002.txt`
  * `rm64pivot002x003.txt`
  * vdots
  * `rm64pivot002x157.txt`
  * VDOTS
  * `rm64pivot310x310.txt`
  * `rm64pivot310x000.txt`
  * vdots
  * `rm64pivot310x154.txt`

* `rm72square`  
  Recall that the pivot-signature polynomial of RM64
  is computed and stored arcoss 48,516 files.
  We gather them and store in this file.
  * `rm64pivotall.txt`

* `rm73square`  
  Wrong zip files.

* `rm74square`  
  The list of zip files of the form `123.zip` @@@
