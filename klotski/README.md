# Kloski Solver
Currently my solver takes 11 ms to solver the classic Forget-me-not opening (81/116 par) on a Google Compute Engine f1-micro instance. Callgrind show the solver used 27,873,279 instructions.

![forget-me-not](forget-me-not.png)

## Reference:
I found these research after I finished my codes:
  * Ruben Spaans, [Improving sliding-block puzzle solving using meta-level reasoning](
                  http://www.pvv.org/~spaans/master-cs.pdf), Master thesis, 2010
  * Meil Bickford, [Sliding Block Puzzles](https://nbickford.wordpress.com/2013/11/13/sliding-block-puzzles-part-4-of-3), blog
