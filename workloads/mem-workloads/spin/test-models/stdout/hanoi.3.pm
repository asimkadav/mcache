Depth= 1000003 States=    1e+06 Transitions= 1.58e+06 Memory=  1718.580	t=   0.48 R=   2e+06
Depth= 2000003 States=    2e+06 Transitions= 3.17e+06 Memory=  1955.201	t=   0.97 R=   2e+06
Depth= 3000003 States=    3e+06 Transitions= 4.75e+06 Memory=  2191.822	t=   1.45 R=   2e+06
Depth= 4000003 States=    4e+06 Transitions= 6.33e+06 Memory=  2428.541	t=   1.93 R=   2e+06
Depth= 5000003 States=    5e+06 Transitions= 7.92e+06 Memory=  2665.162	t=   2.42 R=   2e+06
Depth= 6000003 States=    6e+06 Transitions=  9.5e+06 Memory=  2901.783	t=    2.9 R=   2e+06
Depth= 7000003 States=    7e+06 Transitions= 1.11e+07 Memory=  3138.404	t=   3.39 R=   2e+06
Depth= 8000003 States=    8e+06 Transitions= 1.27e+07 Memory=  3375.123	t=   3.88 R=   2e+06
Depth= 9000003 States=    9e+06 Transitions= 1.43e+07 Memory=  3611.744	t=   4.38 R=   2e+06
error: max search depth too small
Depth= 9999999 States=    1e+07 Transitions= 1.58e+07 Memory=  3848.365	t=   4.88 R=   2e+06
Depth= 9999999 States=  1.1e+07 Transitions= 2.93e+07 Memory=  3939.967	t=   9.62 R=   1e+06
Depth= 9999999 States=  1.2e+07 Transitions= 3.11e+07 Memory=  4031.568	t=   10.2 R=   1e+06
Depth= 9999999 States=  1.3e+07 Transitions= 3.27e+07 Memory=  4123.170	t=   10.7 R=   1e+06
Depth= 9999999 States=  1.4e+07 Transitions= 3.73e+07 Memory=  4214.772	t=   12.2 R=   1e+06

(Spin Version 5.2.0 -- 2 May 2009)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	cycle checks       	- (disabled by -DSAFETY)
	invalid end states	+

State-vector 116 byte, depth reached 9999999, errors: 0
 14321541 states, stored
 28643076 states, matched
 42964617 transitions (= stored+matched)
        5 atomic steps
hash conflicts:   1349519 (resolved)

Stats on memory usage (in Megabytes):
 1966.764	equivalent memory usage for states (stored*(State-vector + overhead))
 1352.759	actual memory usage for states (compression: 68.78%)
         	state-vector as stored = 71 byte + 28 byte overhead
 1024.000	memory used for hash table (-w27)
  457.764	memory used for DFS stack (-m10000000)
 1408.046	other (proc and chan stacks)
    1.696	memory lost to fragmentation
 4244.264	total actual memory usage

Successor counts:
  1	         7	(4.888e-05 % of total)
  2	         3	(2.095e-05 % of total)
  3	  14321536	(100 % of total)
mean 3 (without 0: 3)

pan: elapsed time 14.2 seconds
pan: rate 1007849.5 states/second
