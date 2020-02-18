# clN22 - GPU accelerated '[zold-score/score_suffix](https://github.com/zold-io/zold-score)'-replacement using OpenCL
![slog](misc/smlogo.png)

#### clN22 is a project with the goal for outsourcing the very CPU-intensive 'PoW'-workload from running a [Zold](https://zold.io/)-node.

###### Why on earth...? Well, mostly for self learning tech that was totally new to me: OpenCL (clN22-worker) and the world of Node.js, with the seemingly endless amount of crap available for it (clN22-manager). And of course, while most other PoW-based cryptocurrencies already had their wide variety of fancy mining software available, nothing like this existed for Zold.

While the heart and soul here is the OpenCL worker, the implementation to Zold can be done in many different ways.
In current model all workers and nodes connect to a centralized service, the 'clN22-manager'. It is responsible for relaying all the work (prefixes) from nodes to workers and return valid solutions (suffices) back to nodes once they are found.
This 'clN22-manager'-centered approach can be loosely compared to how a crypto mining pool would operate (one major difference is that every worker attached to 'clN22-manager' is given a different problem to solve). 
Standalone version is also planned.
'clN22-worker' has been tested under [x86-64] Windows 7&10, and few Linux-distributions.
It has also been succesfully tested with Raspberry Pi, running Raspbian & VC4CL. While that works, it performs too slowly to be viable / useful. At least this was the case with RPi 3B+.

#### Performance: Using 'AMD Radeon RX Vega 64' as an example. Speed (hashes / second) gets up to around 300 MH/s. Majority of given inputs yields a valid solution within 30 seconds and calculating hash for every possible suffix takes about 4 minutes. This is in orders of magnitude faster compared to the vanilla CPU-worker.

##### clN22-worker        - OpenCL 'zold-score' worker. Prefix entered as input. Space separated, automatically adds 6-character suffix (a-z A-Z 0-9) and calculates sha256 hash of prefix+suffix. Goes through all possible suffices, until the resulting hash ends with a set  number (8) of trailing zeroes.
##### clN22-remoteclient  - Connect to clN22-manager, fetch work & return results.
##### clN22-manager       - Simple RESTful API to manage workitems, nodes and workers. Accept new workitems, assign to workers and return results.
##### clN22-zoldscore     - Modified zold-score/ScoreSuffix.cpp. Interrupt normal program flow when random suffix is called for. Post prefix to clN22-manager and return valid result (or timeout).

#### Building:
Proper guide is indeed planned..
##### clN22-worker: Just link with OpenCL-library.
##### clN22-remoteclient & clN22-zoldscore: The 'cpr' & 'nlohmann/json' -libraries are required.

###### Feel free to drop me mail at: eetu [at] kkona.xyz

![screenshot](misc/scrshot.png)
