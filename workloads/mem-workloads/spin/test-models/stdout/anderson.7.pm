pan: invalid end state (at depth 24023)
Depth=  995439 States=    1e+06 Transitions= 1.05e+06 Memory=  1574.830	t=   55.3 R=   2e+04
Interrupted

(Spin Version 5.2.0 -- 2 May 2009)
Warning: Search not completed
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 68 byte, depth reached 1692921, errors: 165
  1701785 states, stored
    85841 states, matched
  1787626 transitions (= stored+matched)
        5 atomic steps
hash conflicts:       319 (resolved)

Stats on memory usage (in Megabytes):
  155.803	equivalent memory usage for states (stored*(State-vector + overhead))
   93.744	actual memory usage for states (compression: 60.17%)
         	state-vector as stored = 30 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
   64.338	other (proc and chan stacks)
 1639.967	total actual memory usage

Successor counts:
  0	       165	(1.861 % of total)
  1	      1284	(14.48 % of total)
  2	      3109	(35.06 % of total)
  3	      3047	(34.36 % of total)
  4	      1136	(12.81 % of total)
  5	       127	(1.432 % of total)
mean 2.461 (without 0: 2.507)

pan: elapsed time 103 seconds
pan: rate 16510.963 states/second
