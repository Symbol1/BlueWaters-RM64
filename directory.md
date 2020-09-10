# Directories on Blue Waters

Read [process overview](process.md) before this.
This file explains necessary conventions
to decipher the filenames on Google Drive
<https://drive.google.com/drive/folders/1zYv2R-oqepX1vJ_Fr5JBmrVNdle0mi9M>.

The project ID of our computation is `baus`.
The following is a note on what is inside the directory
`/projects/sciteam/baus`.

* > `helloworld`, `rm16test`, `rm17pivot`, `rm32pivot`
  
  Testing by repeating the computation of something easy.
  These jobs can be done, and had been done, on my laptop.

* > `rm32rref`, `rm33rref`
  
  Trial computation.
  Debug (overflow), optimize the code (pass by reference),
  and check against the pivot-signatutre polynomial.
  The main reason we give up these folders is pure stupidity
  `#define five 4`.

* > `rm34rref`
  
  The RREF-signature polynomial of RM32 is computed here.
  The computation is basically enumerating all 2^32 subsets.
  That resulting polynomial has 17,818,745 monomials.
  Each monomial is encoded as a line
  so the resulting file, `rm32rref.txt`, has 17,818,745 lines.
  After compressing, `rm32rref.zip` is 68.7 MB.
  * > `rm32rref.txt`
  * > `rm32rref.zip`

* > `rm35rref`
  
  Since `rm32rref.txt` has 17,818,745 lines,
  we divide them evenly into 311 files of the form
  `rm35rref123.txt` or `rm35rref${i}.txt` in general .
  Each file has 57,295 lines,
  which represents 57,295 monomials out of 17,818,745 monomials.
  * > `rm35rref000.txt`
  * > `rm35rref001.txt`
  * vdots
  * > `rm35rref310.txt`

* > `rm64square`, `rm65square`, `rm66square`, `rm67square`,
    `rm68square`, `rm69square`, `rm70square`, `rm70tutte`
  
  Wrong computation.
  (Due to overflow of bitfiled.)

* > `rm71square`
  
  From the RREF-signature polynomial of RM32
  we want to compute the pivot-signature polynomial of RM64.
  The computation is like computing the square of the polynomial `rm32rref.txt`.
  Since we had divided the polynomial `rm32rref.txt` into 311 *sub*polynomials,
  it suffices to compute the product of any two *sub*polynomials.
  The resulting files are called
  `rm64pivot123x234.txt` or `rm64pivot${i}x$(( ($i+$j)%310 )).txt` in general.
  Here `$i` runs from `000` to `310`; and `$j` runs from `000` to `155`.
  * > `rm64pivot000x000.txt`
  * > `rm64pivot000x001.txt`
  * vdots
  * > `rm64pivot000x155.txt`
  * (Note that `000x156` does not exist because it is named `156x000`)
  * (Note that `001x000` does not exist because `000x001` does.)
  * > `rm64pivot001x001.txt`
  * > `rm64pivot001x002.txt`
  * vdots
  * > `rm64pivot001x156.txt`
  * (Note that `001x157` does not exist because it is named `157x001`)
  * (Note that `002x000` does not exist because `000x002` does.)
  * (Note that `002x001` does not exist because `001x002` does.)
  * > `rm64pivot002x002.txt`
  * > `rm64pivot002x003.txt`
  * vdots
  * > `rm64pivot002x157.txt`
  * VDOTS
  * > `rm64pivot310x310.txt`
  * > `rm64pivot310x000.txt`
  * vdots
  * > `rm64pivot310x154.txt`

* > `rm72square`
  
  Recall that the pivot-signature polynomial of RM64
  is computed distributedly and stored across 48,516 files.
  (48,516 = 1 + 2 + ... + 311.)
  We gather them and store the final result in this file.
  It has 7,828,354 lines/monomials; and is 274 MB.
  After compressing 42.7 MB.
  
  * > `rm64pivotall.txt`

* > `rm72tutte`
  
  From the pivot-signature polynomial of RM64
  we can compute the Tutte polynomials.
  In particular that of the \[64,22,16\]-Reed-Muller codes.
  * > `rm72tutte.txt`

* > `rm73square`
  
  Wrong zip files.

* > `rm74square`
  
  The list of zip files of the form `123.zip`.
  A file like `123.zip` is the compression of files `rm64pivot123*`.
  That is, each zip file contains 156 txt files,
  starting from `$i` to `$(( ($i+155)%310 ))`.
  
  * > `000.zip`
  * > `001.zip`
  * vdots
  * > `310.zip`
