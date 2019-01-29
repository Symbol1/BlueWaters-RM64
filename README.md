# BlueWaters-baus
Computation of Reed-Muller codes of length 64 on BlueWaters

# Directories

* `rm34rref` 
   The place where the RREF-signature polynomial of RM32 is computed.
   The computation is basically enumerate all 2^32 subsets.

* `rm35rref`  
  The list of txt files of the form `rm35rref123.txt`.
  Each represents a subset of monomials of the RREF-signature polynomial of RM32.
  * `rm35rref000.txt`
  * `rm35rref001.txt`
  * etc
  * `rm35rref310.txt`

* `rm64square`, ..., `rm70square`, `rm70tutte`    
  Wrong computation.
  (Due to overflow of bitfiled.)

* `rm71square`  
  The place where the RREF-signature polynomial of RM32
  becomes the pivot-signature polynomial of RM64.
  But the computation is *distributed*.
  See the @@@@@@ part.

* `rm72square`  
  The place where the pivot-signature polynomial of RM64
  across 48,516 files are gathered together and stored in this file.
  * `rm64pivotall.txt`

* `rm73square`  
  Wrong zip files.

* `rm74square`  
  The list of zip files of the form `123.zip`
