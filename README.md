*This project has been created as part of the 42 curriculum by lminasia.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. It is a
variant of the dining philosophers problem.

A number of coders sit in a circle around a shared Quantum Compiler. There is
one USB dongle between each pair of neighbours, so there are as many dongles as
coders. To compile, a coder needs both dongles next to them at the same time.
Each coder repeats the same cycle: **compile → debug → refactor**. A coder
burns out if `time_to_burnout` milliseconds pass after the start of their last
compile (or after the start of the simulation) without them starting a new
compile.

The goal is to share the dongles so that:

- two coders never hold the same dongle at the same time,
- the program never deadlocks,
- the dongles are handed out fairly, according to a scheduling policy
  (`fifo` or `edf`),
- a released dongle stays unavailable for `dongle_cooldown` milliseconds,
- a burnout is detected and printed within 10 ms,
- log lines never mix with each other.

The simulation stops when a coder burns out, or when every coder has compiled
at least `number_of_compiles_required` times.

## Instructions

### Build

```sh
make        # builds ./codexion
make clean  # removes object files
make fclean # removes object files and the binary
make re     # rebuilds from scratch
```

The code is compiled with `cc -Wall -Wextra -Werror -pthread`.

### Run

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders and of dongles (≥ 1) |
| `time_to_burnout` | ms allowed between the starts of two compiles (≥ 1) |
| `time_to_compile` | ms spent compiling while holding two dongles (≥ 1) |
| `time_to_debug` | ms spent debugging (≥ 0) |
| `time_to_refactor` | ms spent refactoring (≥ 0) |
| `number_of_compiles_required` | the simulation stops once every coder has compiled this many times (≥ 0) |
| `dongle_cooldown` | ms a dongle stays unavailable after it is released (≥ 0) |
| `scheduler` | `fifo` or `edf` |

All arguments are required. Numbers must contain digits only (no sign, no
decimal point) and must not be larger than `INT_MAX`. Any invalid input makes
the program print an error to stderr and exit with status 1.

### Examples

```sh
./codexion 4 800 200 200 200 5 0 fifo    # nobody should burn out
./codexion 5 800 200 200 200 5 50 edf    # nobody should burn out, with cooldown
./codexion 1 400 200 100 100 3 0 fifo    # a single coder: burns out at 400
./codexion 4 310 200 100 100 5 0 edf     # impossible timing: a coder burns out
```

Output format:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
...
```

## Code layout

| File | Contents |
|---|---|
| `includes/codexion.h` | Data structures and prototypes |
| `srcs/main.c` | Entry point, starting and joining the threads |
| `srcs/parsing.c` | Argument checking |
| `srcs/cleanup.c` | Setting up and tearing down the simulation |
| `srcs/coder.c` | The coder thread (compile / debug / refactor loop) |
| `srcs/dongles.c` | Taking both dongles at once through the waiting queues |
| `srcs/dongles_init.c` | Setting up dongles, releasing a dongle |
| `srcs/heap.c`, `srcs/heap_utils.c` | The binary min-heap (priority queue) |
| `srcs/monitor.c` | The monitor thread (burnout and end detection) |
| `srcs/logger.c` | Serialized logging |
| `srcs/utils.c` | Time, the stop flag, interruptible sleep |

## Blocking cases handled

### Deadlock: Coffman's conditions

A deadlock needs all four Coffman conditions at the same time. Codexion breaks
two of them.

1. **Mutual exclusion:** kept on purpose. A dongle can only be held by one
   coder at a time.
2. **Hold and wait:** broken. A coder never holds one dongle while waiting for
   the other. In `dongles_take()`, the coder locks both dongle mutexes and takes
   the two dongles in a single step, and only when both are free. Until then it
   holds neither.
3. **No preemption:** kept. A coder keeps its dongles until it has finished
   compiling.
4. **Circular wait:** broken, in two places:
   - Mutexes are always locked in increasing dongle index order. Each coder
     stores its dongles as `first` (lower index) and `second` (higher index),
     so two threads can never each hold a mutex that the other one wants.
   - All waiting queues use one shared order (see next section). The
     highest-ranked waiter is therefore first in line on *both* of its
     dongles. It can never wait for someone who is waiting for it.

### Starvation and fairness

Each dongle has a waiting queue implemented as a binary min-heap. When a coder
wants to compile, it is added to the queues of both of its dongles with the
same entry `(key, ticket)`:

- `ticket` comes from one global counter, `sim->next_ticket`, protected by
  `ticket_mutex`. It records the order in which requests arrived, across all
  dongles.
- With `fifo`, `key = ticket`, so requests are served in arrival order.
- With `edf`, `key = last_compile_start + time_to_burnout`, so the coder
  closest to burning out goes first. Equal deadlines are broken by `ticket`,
  which makes EDF fully deterministic.

A coder may only take its dongles when it is **first in both queues**, both
dongles are free, and both cooldowns have passed. Because every queue uses the
same order, the best-ranked waiter will get its dongles as soon as they are
released. Nobody can overtake it, so nobody is starved.

Using a separate counter per dongle would not be safe. With two coders and two
dongles, coder A could end up first in line for dongle 0 while coder B is first
in line for dongle 1, and they would wait for each other until one burned out.
The single global ticket rules this out.

On top of this, even-numbered coders start `time_to_compile / 2` ms later. As a
result, the first round puts every other coder at the compiler instead of
having all coders compete for the same dongles at once.

### Dongle cooldown

When a dongle is released (`dongle_release()`), it records
`available_at = now + dongle_cooldown`. A waiting coder only takes it once
`now >= available_at`.

### Precise burnout detection

A separate monitor thread checks every coder about once per millisecond. For
each coder it reads `last_compile_start` under that coder's mutex. When
`now - last_compile_start >= time_to_burnout`, it calls `log_burnout()`.

`last_compile_start` is updated as soon as a coder takes its two dongles,
before anything is printed. A coder that already has its dongles therefore
cannot be reported as burned out.

### Clean shutdown

When the simulation stops:

- `sim_sleep()` sleeps in 0.5 ms steps and returns as soon as the stop flag is
  set, so coders don't keep sleeping through a long compile, debug or refactor.
- Coders waiting for dongles see the stop flag within 1 ms. They remove
  themselves from both queues (`pair_cancel()`) and return.
- The monitor broadcasts on every dongle's condition variable to wake waiting
  coders.
- `main` joins every thread and frees all memory.

### Single coder

With one coder there is only one dongle, so the coder can never compile. It
prints `has taken a dongle` once and waits until the monitor reports the
burnout.

### Log serialization

Every line is printed by `log_event()` while holding `log_mutex`, so two lines
can never mix. See the next section for how this also guarantees that
`burned out` is the last line.

## Thread synchronization mechanisms

### Primitives used

| Primitive | What it protects |
|---|---|
| `dongle.mutex` (one per dongle) | The dongle's state: `taken`, `available_at` and its waiting queue `waiters` |
| `dongle.cond` (one per dongle) | Wakes coders waiting for that dongle when it is released, or when the simulation stops |
| `coder.mutex` (one per coder) | `last_compile_start` and `compile_count`, which the coder writes and the monitor reads |
| `sim.log_mutex` | Standard output, so that one line is printed at a time |
| `sim.stop_mutex` | The `stop` flag, which every thread reads and the monitor writes |
| `sim.ticket_mutex` | The global arrival counter `next_ticket` |

### How a coder takes its dongles

```
pair_enqueue:   get (key, ticket); lock a, lock b; push into both queues; unlock
dongles_take:   lock a, lock b
                while not (both free, both cooled down, first in both queues):
                    if stop: leave both queues, unlock, return failure
                    unlock b
                    pthread_cond_timedwait(a.cond, a.mutex, now + 1 ms)
                    lock b
                pop from both queues; mark both taken; unlock b, unlock a
```

Before going to sleep, the coder releases `b.mutex`. While sleeping,
`pthread_cond_timedwait` also releases `a.mutex`. Other threads can therefore
release, queue for, or take either dongle in the meantime. A release of `a`
signals `a.cond` and wakes the coder right away. A release of `b` does not
signal `a.cond`, so the wait is limited to 1 ms, after which the condition is
checked again.

### Examples of prevented race conditions

- **Two coders taking the same dongle:** `taken` is only read and written while
  holding the dongle's mutex. The check "both free and first in line" and the
  step "mark both taken" happen while both mutexes are held, so no other thread
  can step in between them.
- **Monitor reading a value while it is being written:** `last_compile_start`
  and `compile_count` are only accessed under `coder.mutex`. The monitor never
  reads a half-updated value.
- **Lines printed after the burnout:** `log_burnout()` holds `log_mutex`, sets
  the stop flag, and then prints `burned out`. `log_event()` checks the stop
  flag while holding the same `log_mutex`, and prints nothing once it is set. A
  coder message is therefore printed either completely before the burnout line
  or not at all.
- **Deadlock between the locks themselves:** locks are always taken in the same
  order: `log_mutex` before `stop_mutex`, and the lower dongle mutex before the
  higher one. `coder.mutex` and `ticket_mutex` are never held together with
  another lock.

### How the coders and the monitor communicate

They share no messages. They communicate only through shared state, always
under a mutex:

- **Coders → monitor:** `last_compile_start` and `compile_count`, under
  `coder.mutex`.
- **Monitor → coders:** the `stop` flag, under `stop_mutex`. Every coder checks
  it in its loops, `sim_sleep()` checks it every 0.5 ms, and `log_event()`
  checks it before printing. To end the simulation, the monitor sets the flag
  and then broadcasts on every dongle's condition variable, so no coder stays
  blocked.

## Resources

- `man pthread_mutex_lock`, `man pthread_cond_timedwait`, `man gettimeofday`
- [POSIX Threads Programming (LLNL)](https://hpc-tutorials.llnl.gov/posix/)
- [pthreads(7) on man7.org](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Deadlock and the Coffman conditions (Wikipedia)](https://en.wikipedia.org/wiki/Deadlock_(computer_science))
- [Earliest deadline first scheduling (Wikipedia)](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary heap (Wikipedia)](https://en.wikipedia.org/wiki/Binary_heap)
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks*, ACM Computing
  Surveys, 1971

### Use of AI

I used Claude (Anthropic) as a review and debugging assistant for these tasks:

- **Code review:** finding bugs in my implementation, including:
  - a `NULL` pointer that crashed the program
  - the system constant `SCHED_FIFO` used by mistake instead of
    `SCHEDULER_FIFO`, which made `fifo` behave like `edf`
  - an uninitialised sequence counter
  - log lines printed after the burnout message
  - a forbidden function (`strtol`) in the parsing
- **Liveness analysis:** working out why coders burned out even when the timing
  allowed them to survive (coders held one dongle while waiting for the other),
  and changing the design so that both dongles are taken in one step, ordered by
  a global ticket.
- **Norm refactoring:** splitting functions and files so that they follow the
  42 Norm, without changing behaviour.
- **Testing:** a log-checking script, and runs with valgrind, helgrind and DRD
  to check for memory leaks and data races.
- **Documentation:** drafting this README.

I reviewed, tested and understood every change before keeping it.
