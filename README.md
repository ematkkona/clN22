# clN22 - GPU accelerated '[zold-score/score_suffix](https://github.com/zold-io/zold-score)'-replacement using OpenCL
![slog](cln22.png)

#### clN22 is an experimental project for outsourcing CPU-intensive 'PoW'-workload running a [Zold](https://zold.io/)-node.

##### cln22
OpenCL 'zold-score' worker. 'Prefix' entered as input. Space separated, 'cln22' adds 6-character 'suffix' (a-z A-Z 0-9) and calculates sha256 hash of prefix+suffix. Starting with 'aaaaaa', proceeds to go through all possible suffices (ending with '999999') until the resulting hash ends with a set  number of trailing zeroes (8).
OpenCL kernel '[kernel22.cl](https://github.com/ematkkona/clN22/tree/master/src/kernel22.cl)' is built when program is launched for the first time. It is saved as '.clbin' -file, which is then used for all subsequent runs with the same device.
Work group sizes are calculated according to what's available in the device used.
Kernel is launched as 3-dimensional program. X, Y and Z are assigned to handle one character of 'suffix' each. Fourth character is resolved inside the kernel as well: If workgroup sizes are >= { 256, 256, 256 }, it is done in single execution. If not, by looping the kernel over as many times as needed. Remaining two characters are managed by host program. Time between kernel executions is measured and used to calculate performance, presented as 'MHash/s'.
###### cln22-zoldscore
Modified 'zold-score/ScoreSuffix.cpp'. While 'zoldscore' normally uses randomly generated suffices and hashes them with CPU, 'cln22-zoldscore' calls 'cln22' or 'cln22-remote' instead. If it fails, it reverts back to normal 'random suffix with CPU'-opreating mode.
##### '[cln22-remote](https://github.com/ematkkona/cln22-remote)'
Client for '[cln22-manager](https://github.com/ematkkona/clNManager-deno)' - fetch work items & return results.

#### Build:
With GNU toolchain:

`git clone https://github.com/ematkkona/clN22.git`

`cd clN22`

`make all`

#### Usage:
'cln22' is automatically launched from `cln22-remote` - or from `zold-score`, if ran locally with the zold node.
##### Manual use:

Help:

`./build/cln22`

Sanity testing & benchmarking:

`./build/cln22 runtests`

Process an example workload:

`./build/cln22 "InputStringWithCharactersAndNumbersABCabc123WithLengthOfSixtyToNinety" result.out log result.log`
