
(Spin Version 5.2.0 -- 2 May 2009)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 92 byte, depth reached 6567, errors: 0
     6563 states, stored
    13120 states, matched
    19683 transitions (= stored+matched)
        5 atomic steps
hash conflicts:         2 (resolved)

Stats on memory usage (in Megabytes):
    0.751	equivalent memory usage for states (stored*(State-vector + overhead))
    0.684	actual memory usage for states (compression: 91.07%)
         	state-vector as stored = 81 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
 1483.131	total actual memory usage

Successor counts:
  1	         7	(0.1066 % of total)
  2	         3	(0.04568 % of total)
  3	      6558	(99.85 % of total)
mean 2.997 (without 0: 2.997)

pan: elapsed time 0.01 seconds
