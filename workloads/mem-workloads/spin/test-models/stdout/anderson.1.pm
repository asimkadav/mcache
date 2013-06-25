
(Spin Version 5.2.0 -- 2 May 2009)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 36 byte, depth reached 2639, errors: 0
   352666 states, stored
   351639 states, matched
   704305 transitions (= stored+matched)
        1 atomic steps
hash conflicts:       207 (resolved)

Stats on memory usage (in Megabytes):
   21.525	equivalent memory usage for states (stored*(State-vector + overhead))
   13.695	actual memory usage for states (compression: 63.63%)
         	state-vector as stored = 13 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
 1495.436	total actual memory usage

Successor counts:
  1	      1029	(0.2918 % of total)
  2	    351638	(99.71 % of total)
mean 1.997 (without 0: 1.997)

pan: elapsed time 0.11 seconds
pan: rate 3206054.5 states/second
