1. rh and rt didn't parse remove target, thus we can't check remove node or not in queue view.
-> not need, it will check the option length is work or not.

2. option malloc enlarge space may not work?
-> due to strdup also need malloc a new space, thus this could failed
-> strdup also wrap by the test_strdup in harness.c

Note. 
1. strncpy is slow than memcpy

queue_merge_sort.c.failed
> swap cloud distroy ordering