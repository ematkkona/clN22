# clN22 - OpenCL accelerated '[zold-score](https://github.com/zold-io/zold-score)'-replacement
![slog](res/smlogo.png)

Experimental project aiming to outsource the very CPU intensive 'PoW'-workload when running [Zold](https://zold.io/)-node.
Current operating model somewhat resembles traditional mining pools. The ocl-worker itself is highly portable and can be used for other kinds of implementations.

###### clN22-worker      - OpenCL-zold-score grinder
###### 		-wproxy 	 - Connect to clN22-manager, fetch work, return solutions. Launches clN22-worker for processing.
######		-wTest		 - Compatibility/benchmark test
###### clN22-manager     - Simple RESTful API to manage workitems, nodes and workers. Receive new workitems, assign to workers, return results.   
###### clN22-zoldscore   - Modified zold-score/ScoreSuffix.cpp. Interrupt program flow when requesting random nonce. Post prefix to clN22-manager and return valid result (or timeout).

![screenshot](res/scrshot.png)
