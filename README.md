# htype and hinject typing bots

Mimic human typing - now with hardcoded automation

Reads from stdin, writes to stdout one character at a time with
configurable speed and error injection

## Build

```bash
git clone https://github.com/lord-of-the-strings/htype
cd htype
make
```

## Install

```bash
make install
```

Installs to `/usr/local/bin/htype` and `/usr/local/share/man/man1/htype.1`.
Override prefix:

```bash
make install PREFIX=~/.local
```

## Usage

```
htype [--wpm N] [--error-rate F]
```

| Flag | Default | Description |
|------|---------|-------------|
| `--wpm N` | 120 | Typing speed in words per minute |
| `--error F` | 0.0 | Mistype probability per letter (0.0–1.0) |
| `--help`| N/A | Standard help function |
| `--jitter` | 0.0 | Amount of "jitter" to add ie the probability of a different speed for a particular letter (human inconsistency) |
| `--debug-timing` | 0 | Boolean flag to enable debugging logs of letter timing |

## Examples

```bash
echo "hello world" | htype --wpm 80

cat essay.txt | htype --wpm 120 --error-rate 0.05

cat script.txt | htype --wpm 100 | tee output.txt
```

## Stages

- [x] Stage 1 — WPM timing
- [x] Stage 2 — Error injection
- [x] Stage 3 — Confidence / rhythm
- [ ] Stage 4 — Advanced humanization (open-ended)

## Deployment
- Deployed to the Arch User Repository (AUR)
  https://aur.archlinux.org/packages/htype
  Use your favorite AUR helper to install htype and run:
  ```bash
  yay -S htype
  #OR
  paru -S htype
  ```
## Introducing the hinject bot
hinject is an automation for htype. 
- It provides a neat TUI made using InquirerPy.
- It automatically initializes and runs the htype bot as a child process, intercepts stdout, and writes the output in real time to an online text box.
- For the automation, hinject uses monkeytype in Firefox, automated using selenium. The hinject bot launches firefox, waits for monkeytype to load, enters zen mode and types out.

---------------
##Future Plans
- Deployment to a custom web application using hinject bot
- Making hinject bot send keystrokes so that it becomes difficult to detect that it is a bot
- 
## License

MIT
