# clN22 - OpenCL accelerated '[zold-score/score_suffix](https://github.com/zold-io/zold-score)'-replacement
![slog](misc/smlogo.png)

#### clN22 is a project with the goal of outsourcing CPU-intensive 'PoW'-workload from running a [Zold](https://zold.io/)-node.
Current 'clN22-manager'-centered approach can be somewhat compared to how a crypto mining pool operates (a major difference being that every worker attached to 'clN22-manager' is given a different problem to solve). Stand-alone version is planned. The 'clN22-worker' is highly portable; [x86-64] It has been tested under Windows 7&10 and various Linux-distributions. OS X should work as well, though this is untested. In addition, it has been succesfully tested with Raspberry Pi, running Raspbian & VC4CL. While it works, it performs too slow to be a viable option. 

#### Performance: Using 'AMD Radeon RX Vega 64' as an example. Speed (hashes / second) gets up to around 300 MH/s. Majority of given inputs yields a valid solution within 20 seconds, and calculating hash with every possible suffix takes ~4 minutes.

##### clN22-worker        - OpenCL 'zold-score' worker. Prefix entered as input. Space separated, automatically adds 6-character suffix (a-z A-Z 0-9) and calculates sha256 hash of prefix+suffix. Goes through all possible suffices, until the resulting hash ends with a set  number (8) of trailing zeroes.
##### clN22-remoteclient  - Connect to clN22-manager, fetch work & return results.
##### clN22-manager       - Simple RESTful API to manage workitems, nodes and workers. Accept new workitems, assign to workers and return results.
##### clN22-zoldscore     - Modified zold-score/ScoreSuffix.cpp. Interrupt normal program flow when random suffix is called for. Post prefix to clN22-manager and return valid result (or timeout).


#### Building:
Proper guide is indeed planned..

##### clN22-worker: Just link with OpenCL-library.
##### clN22-remoteclient & clN22-zoldscore: The 'cpr' & 'nlohmann/json' -libraries are required.

![screenshot](misc/scrshot.png)
