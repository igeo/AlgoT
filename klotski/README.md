# Kloski (华容道) Solver
Currently my solver takes 11 ms to solver the classic Forget-me-not (横刀立马) opening (81/116 par) on a Google Compute Engine f1-micro instance. Callgrind show the solver used 27,873,279 instructions.

![forget-me-not](forget-me-not.png)

## Reference:
I found these research after I finished my codes:
  * Ruben Spaans, [Improving sliding-block puzzle solving using meta-level reasoning](
                  http://www.pvv.org/~spaans/master-cs.pdf), Master thesis, 2010
  * Meil Bickford, [Sliding Block Puzzles](https://nbickford.wordpress.com/2013/11/13/sliding-block-puzzles-part-4-of-3), blog
  * Yong Su, [The JavaScript algorithm for solving klotski game](https://github.com/jeantimex/klotski)
  * Memoryboxes, [Android Game](http://happy123.me/blog/2015/03/03/huarongdao)
  * [ Rumber Klotski game](https://cloud.tencent.com/developer/article/1036257)
