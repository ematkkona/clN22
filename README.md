# clN22 - GPU accelerated '[zold-score/score_suffix](https://github.com/zold-io/zold-score)'-replacement using OpenCL
![slog](misc/smlogo.png)

#### clN22 is an experimental project for outsourcing CPU-intensive 'PoW'-workload running a [Zold](https://zold.io/)-node.

##### cln22
OpenCL 'zold-score' worker. 'Prefix' entered as input. Space separated, 'cln22' adds 6-character 'suffix' (a-z A-Z 0-9) and calculates sha256 hash of prefix+suffix. Starting with 'aaaaaa', proceeds to go through all possible suffices (ending with '999999') until the resulting hash ends with a set  number of trailing zeroes (8).
OpenCL kernel 'kernel22.cl' is built when launched for the first time. It is saved as '.clbin' -file, and is then used for all subsequent runs with the same device.
Work group sizes are calculated according to the hardware used, takes advantage with full multiples of '62', if possible, or divides the work to smaller chunks if needed.
Kernel is set up and launched as 3-dimensional OCL-program. X, Y and Z are assigned to one character of prefix each. Two characters are managed by host program. Remaining single character is resolved in kernel. If workgroup size >= { 256, 256, 256 }), it is done in single execution. If smaller, by looping the kernel over as many times as needed.
###### cln22-zoldscore
Modified 'zold-score/ScoreSuffix.cpp'. While 'zoldscore' normally uses randomly generated suffices and hashes them with CPU, 'cln22-zoldscore' calls 'cln22' or 'cln22-remote' instead. If it fails, it reverts back to normal 'random suffix with CPU'-opreating mode.
##### '[cln22-remote](https://github.com/ematkkona/cln22-remote)'
Client for '[cln22-manager](https://github.com/ematkkona/clNManager-deno)' - fetch work items & return results.

#### Build:
With GNU toolchain:

`git clone https://github.com/ematkkona/clN22.git`

`cd clN22/src`

`make`

#### Usage:
In normal use 'cln22' is automatically launched from `cln22-remote` or `zold-score` if ran locally with the zold node.
##### Manual use:

Help:

`./cln22`

Sanity testing & benchmarking:

`./cln22 runtests`

Process an example workload:

`./cln22 "InputStringWithCharactersAndNumbersABCabc123WithLengthOfSixtyToNinety" result.out log result.log`
