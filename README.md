# htype

Mimic human typing on stdout.

Reads from stdin, writes to stdout one character at a time with
configurable speed and error injection.

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
| `--error-rate F` | 0.0 | Mistype probability per letter (0.0–1.0) |

## Examples

```bash
echo "hello world" | htype --wpm 80

cat essay.txt | htype --wpm 120 --error-rate 0.05

cat script.txt | htype --wpm 100 | tee output.txt
```

## Stages

- [x] Stage 1 — WPM timing
- [x] Stage 2 — Error injection
- [ ] Stage 3 — Confidence / rhythm
- [ ] Stage 4 — Advanced humanization (open-ended)

## License

MIT
