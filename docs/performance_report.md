# OBCDemo — performance & memory report

Command-line measurements of the `OBCDemo` executable.

- **Binary:** `build/x86-gcc/output-release/apps/OBCDemo/OBCDemo`
- **Configured / built with:**
  ```sh
  cmake --preset x86-gcc-release          # Release, no BUILD_TESTING
  cmake --build --preset x86-gcc-release --target OBCDemo
  ```

---

## 1. Binary size

Static memory footprint of the executable: machine code (`text`), initialised
globals (`data`), and zero-initialised globals (`bss`). No run required.

```sh
size build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
```

```
   text	   data	    bss	    dec	    hex	filename
  87987	   1992	    344	  90323	  160d3	build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
```

---

## 2. Wall-clock runtime

End-to-end time for one full run. 20 repetitions, `%R`
real seconds, sorted.

```sh
TIMEFORMAT="%3R"
for i in $(seq 1 20); do time build/x86-gcc/output-release/apps/OBCDemo/OBCDemo >/dev/null 2>&1; done | sort -n
```

```
0.001 0.001 0.001 0.001 0.001 0.001 0.002 0.002 0.002 0.002
0.002 0.002 0.003 0.003 0.003 0.003 0.004 0.004 0.004 0.005
```

total execution time = [1–5 ms]

---

## 3. System-call profile

Counts and aggregate kernel time per system call for the whole run.

```sh
strace -c -f build/x86-gcc/output-release/apps/OBCDemo/OBCDemo >/dev/null
```

```
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 32.61    0.000482          21        22           mmap
 30.65    0.000453         453         1           execve
  9.61    0.000142          28         5           openat
  7.85    0.000116          19         6           mprotect
  3.99    0.000059           9         6           fstat
  3.52    0.000052          10         5           close
  3.04    0.000045          11         4           read
  2.50    0.000037          37         1           munmap
  1.49    0.000022          11         2           pread64
  1.01    0.000015          15         1         1 access
  0.61    0.000009           9         1           arch_prctl
  0.54    0.000008           2         3           brk
  0.54    0.000008           8         1           futex
  0.54    0.000008           8         1           set_tid_address
  0.54    0.000008           8         1           rseq
  0.47    0.000007           7         1           set_robust_list
  0.47    0.000007           7         1           prlimit64
  0.00    0.000000           0         8           write
  0.00    0.000000           0         1         1 ioctl
  0.00    0.000000           0         1           getrandom
------ ----------- ----------- --------- --------- ----------------
100.00    0.001478          20        72         2 total
```

---

## 4. Heap totals & leak check

Total heap traffic over the run, blocks still allocated at exit, and any
invalid reads/writes, leaks, or mismatched `free`/`delete`.

```sh
valgrind --leak-check=full --show-leak-kinds=all \
  build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
```

```
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: 89 allocs, 89 frees, 82,301 bytes allocated

All heap blocks were freed -- no leaks are possible

ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

89 allocations for the whole process, all freed, no errors.

---

## 5. Peak heap over time

Heap size sampled across the run.

```sh
valgrind --tool=massif --massif-out-file=massif.out \
  build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
ms_print massif.out
```

```
    KB
76.12^                                     :#@::@::::@:@:@@::@::::::::::::@@@
     |                                     :#@::@::: @:@:@@::@:         : @
     |                                     :#@::@::: @:@:@@::@:  (flat — no growth)
   0 +----------------------------------------------------------------------->Mi
     0                                                                   3.645
Number of snapshots: 77

  n        time(i)         total(B)   useful-heap(B) extra-heap(B)
  4      2,061,000           77,912           77,878            34     <- peak

Peak breakdown:
  94.63% (73,728 B)  libstdc++ / ld.so start-up  (_dl_init -> call_init)
   5.26% ( 4,096 B)  libc stdio buffer for std::cout  (fwrite -> _IO_file_doallocate)
   ~0.1% (   ~54 B)  below threshold
```

Peak ≈ 78 KB, ~95% of it C++ runtime start-up and the rest the single 4 KB
stdio buffer `std::cout` allocates on first write. The trace is flat: nothing
accumulates during the simulation.

---

## 6. Allocation profile

Per-allocation view: block count, total vs. peak bytes, and how much the
blocks are read from / written to over their lifetime — spots allocation churn
even when nothing leaks.

```sh
valgrind --tool=dhat --dhat-out-file=dhat.out \
  build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
```

```
Total:     82,301 bytes in 89 blocks
At t-gmax: 77,900 bytes in 4 blocks
At t-end:  0 bytes in 0 blocks
Reads:     14,617 bytes
Writes:    14,724 bytes
```

Only 4 live blocks at the high-water mark. (Open `dhat.out` in
`/usr/libexec/valgrind/dh_view.html` for the per-callsite table.)

---

## 7. CPU: instructions, cache, branches

Exact instruction count, simulated I/D cache miss rates, and branch-prediction
rates for the whole run — a substitute for hardware `perf` counters and a
proxy for CPU cost and memory-access efficiency.

```sh
valgrind --tool=cachegrind --cache-sim=yes --branch-sim=yes \
  build/x86-gcc/output-release/apps/OBCDemo/OBCDemo
```

```
I refs:        3,734,299
I1  misses:        2,391
LLi misses:        2,347
I1  miss rate:      0.06%
LLi miss rate:      0.06%

D refs:        1,314,605  (947,973 rd   + 366,632 wr)
D1  misses:       16,412  ( 14,017 rd   +   2,395 wr)
LLd misses:        9,261  (  7,763 rd   +   1,498 wr)
D1  miss rate:       1.2% (    1.5%     +     0.7%  )
LLd miss rate:       0.7% (    0.8%     +     0.4%  )

LL refs:          18,803  ( 16,408 rd   +   2,395 wr)
LL misses:        11,608  ( 10,110 rd   +   1,498 wr)
LL miss rate:        0.2% (    0.2%     +     0.4%  )

Branches:        559,540  (546,527 cond +  13,013 ind)
Mispredicts:      25,294  ( 23,284 cond +   2,010 ind)
Mispred rate:        4.5% (    4.3%     +    15.4%   )
```

~3.7 M instructions for the entire demo, sub-1% instruction- and
data-cache miss rates. (The earlier coverage + syslog build was ~6.3 M.)
