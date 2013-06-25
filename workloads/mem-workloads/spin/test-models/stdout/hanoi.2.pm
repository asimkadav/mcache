
(Spin Version 5.2.0 -- 2 May 2009)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 100 byte, depth reached 531447, errors: 0
   531443 states, stored
  1062880 states, matched
  1594323 transitions (= stored+matched)
        5 atomic steps
hash conflicts:      2017 (resolved)

Stats on memory usage (in Megabytes):
   64.873	equivalent memory usage for states (stored*(State-vector + overhead))
   46.899	actual memory usage for states (compression: 72.29%)
         	state-vector as stored = 65 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
   66.644	other (proc and chan stacks)
 1595.436	total actual memory usage

Successor counts:
  1	         7	(0.001317 % of total)
  2	         3	(0.0005645 % of total)
  3	    531438	(100 % of total)
mean 3 (without 0: 3)

pan: elapsed time 0.51 seconds
pan: rate 1042045.1 states/second
