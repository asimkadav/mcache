pan: invalid end state (at depth 6786)

(Spin Version 5.2.0 -- 2 May 2009)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 40 byte, depth reached 24156, errors: 142
   151290 states, stored
   244243 states, matched
   395533 transitions (= stored+matched)
        0 atomic steps
hash conflicts:        74 (resolved)

Stats on memory usage (in Megabytes):
    9.811	equivalent memory usage for states (stored*(State-vector + overhead))
    7.174	actual memory usage for states (compression: 73.13%)
         	state-vector as stored = 22 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
 1489.869	total actual memory usage

Successor counts:
  0	       142	(0.09386 % of total)
  1	      6127	(4.05 % of total)
  2	     45658	(30.18 % of total)
  3	     99363	(65.68 % of total)
mean 2.614 (without 0: 2.617)

pan: elapsed time 8.05 seconds
pan: rate 18793.789 states/second
