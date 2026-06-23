# htype

**Mimic human typing on stdout.**

htype reads from stdin and writes to stdout one character at a time, introducing realistic delays, speed variation, and corrected typos that make automated text output indistinguishable from a person actually typing. It is a single C file, has no runtime dependencies beyond libc, and follows Unix conventions — it composes cleanly with pipes.

Inspired [Clack](https://github.com/ThisWasAryan/clack) to exist.

---

## Why

Most tools that "type" text either dump it instantly or introduce a flat per-character sleep. Neither looks human. A real typist has a base speed, rhythmic inconsistency around that speed, and occasional adjacent-key mistakes that get immediately corrected. htype models all three. The output is a character stream that, when fed to a terminal or replayed through any observer, reads as a person typing — not a script running.

---

## Install

**From source:**

```sh
git clone https://github.com/lord-of-the-strings/htype
cd htype
make
make install          # installs to /usr/local/bin and /usr/local/share/man/man1
```

Override install prefix:

```sh
make install PREFIX=~/.local
```

**Arch Linux and derived distributions (AUR):**

```sh
yay -S htype
# or
paru -S htype
```
Note that the htype in AUR is unstable but out of date and pretty much just the original version. It will be updated soon.

---

## Usage

```
htype [--wpm N] [--error F] [--jitter F] [--debug-timing]
```

htype reads from stdin and writes to stdout. Pipe any text source into it.

### Flags

| Flag | Default | Description |
|------|---------|-------------|
| `--wpm N` | `120` | Typing speed in words per minute. One word = 5 characters. |
| `--error F` | `0.0` | Per-character mistype probability (0.0–1.0). When a mistype fires on a lowercase letter, htype types an adjacent key, pauses, then backspaces and types the correct character — the same sequence a human makes when catching a typo immediately. |
| `--jitter F` | `0.0` | Per-character speed variation (0.0–1.0). At `0.0`, every character is typed at exactly the WPM-derived interval. At `0.5`, each character's delay is drawn from a Gaussian distribution centred on the base delay with a standard deviation of `delay × 0.5`. Models the natural rhythm variation of real typing. |
| `--debug-timing` | off | Writes each character and its actual delay in microseconds to stderr. Useful for verifying jitter behaviour or profiling timing. |
| `--help` | — | Prints usage and exits. |

### How WPM maps to delay

The standard definition: 1 word = 5 characters. So:

```
delay_us = 60_000_000 µs / (wpm × 5)
```

| WPM | Per-character delay |
|-----|-------------------|
| 40  | 300 ms |
| 60  | 200 ms |
| 80  | 150 ms |
| 120 | 100 ms |
| 180 | 66 ms |

### How jitter works

The jitter value is a fraction of the base delay used as the standard deviation of a Box–Muller Gaussian draw. The result is clamped to `[base × (1 − jitter), base × (1 + jitter)]` so the output speed never escapes a sensible range. This is the same statistical model underlying several keystroke-dynamics research papers and produces interval distributions that match real typing data.

### How error correction works

When an error fires on a lowercase letter:
1. An adjacent-key character is written (substitution — the most common real mistype)
2. htype sleeps for the normal character delay
3. It writes `\b \b` (backspace, space, backspace — the standard erase sequence)
4. It sleeps again
5. The correct character is written

This produces the exact byte sequence a terminal renders when a person catches and corrects a typo inline. Only lowercase letters are eligible for errors; punctuation, digits, and uppercase pass through unmodified in the current version.

---

## Examples

```sh
# Basic: type a message at 80 WPM
echo "hello world" | htype --wpm 80

# Type a file at 120 WPM with 3% error rate and natural rhythm variation
cat essay.txt | htype --wpm 120 --error 0.03 --jitter 0.25

# Pipeline: type into a file while also showing output in the terminal
cat script.txt | htype --wpm 100 | tee output.txt

# Inspect timing: see each character's actual delay on stderr
echo "timing test" | htype --wpm 60 --jitter 0.4 --debug-timing 2>&1 | head -20

# Very slow, very human: low WPM, high jitter, occasional errors
echo "draft message" | htype --wpm 40 --jitter 0.5 --error 0.05
```

---

## How it works

htype is 161 lines of C. The core loop reads one character at a time from stdin. For each character:

1. The base delay is computed from `--wpm`.
2. If `--jitter` is set, a Gaussian offset is drawn using a Box–Muller transform and added to the delay. The result is clamped.
3. If `--error` is set and the character is a lowercase letter, a uniform random draw decides whether a mistype occurs. If so, the adjacent-key map is consulted, the wrong character is written, and the correction sequence runs.
4. The correct character is written.
5. `clock_nanosleep(CLOCK_MONOTONIC, ...)` sleeps for the computed delay. Interrupted sleeps (EINTR) are retried with the remaining time, so the timing is accurate across signals.

The adjacent-key map (`adj[]`) encodes QWERTY neighbours for all 26 lowercase letters. A mistype picks uniformly from the neighbour string for the pressed key — modelling the most common substitution error class in real typing data.

---

## Roadmap

- [x] Stage 1 — WPM-based per-character timing
- [x] Stage 2 — Adjacent-key error injection with immediate correction
- [x] Stage 3 — Gaussian jitter for rhythm variation (`--jitter`, Box–Muller)
- [ ] Stage 4 — Advanced humanization (see below)
- [ ] hinject — Browser extension for DOM `KeyboardEvent` injection (active development)

### Stage 4 (future CLI work)

Planned for the core htype tool after the browser extension work:

- Log-normal IKI distribution (empirically matches the Dhakal et al. 136M-keystroke dataset more accurately than Gaussian jitter)
- Word-boundary and sentence-boundary pauses
- Session warmup and fatigue curves
- Delayed error correction (noticing a mistake several characters later)
- Shift-key penalty for capitalised characters
- Common-word acceleration / difficult-word slowdown

### hinject (active)

A browser extension target for injecting synthetic `keydown` / `keyup` / `keypress` DOM events rather than filling form fields. Goal: timing-accurate keystroke injection that passes bot-detection fingerprinting at the event level. This is a separate engineering problem from the CLI stream tool and is the current development focus.

---

### License

MIT. See [LICENSE](LICENSE).
