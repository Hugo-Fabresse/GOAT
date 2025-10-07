# GOAT — Git Overhauled, Advanced & Tailored

**GOAT** is a lightweight, local-first version control system — built from scratch in C — reimagining Git’s core ideas to make them simpler, more understandable, and more adaptable.

GOAT doesn’t aim to replace Git.
It exists to **make the mechanics of version control more transparent**, while introducing features Git never dared to touch.

---

## Why GOAT?

Git is powerful — but it’s also complex, opaque, and rigid for many developers.

**GOAT is:**

- **Minimalist**: Only the essential, no hidden magic.
- **Local-first**: No remotes, no networks, no noise.
- **Configurable**: Every behavior is user-defined through clean config files.
- **Readable**: Commits and history are displayed as ASCII graphs.

Whether you’re a student, a low-level developer, or just curious about what happens behind `git commit`, **GOAT gives you visibility, control, and insight**.

---

## What Can GOAT Do?

GOAT implements the most important commands to manage your code versions locally:

| Command         | Purpose                                 |
|----------------|------------------------------------------|
| `goat init`     | Start a new local repository             |
| `goat add`      | Stage files for commit                   |
| `goat commit`   | Save a snapshot of your current state    |
| `goat status`   | Show current changes and staged files    |
| `goat diff`     | See what changed (line-by-line)          |
| `goat log`      | Browse history with a readable graph     |
| `goat checkout` | Go back to any previous snapshot         |
| `goat config`   | Adjust behavior via `.goatconfig` files  |

---

## What Makes GOAT Different?

GOAT introduces some **key improvements** over Git’s default behavior:

### 1. Human-Friendly Hashes

Commit hashes are short, readable, and easier to share or remember.
You’ll never have to copy-paste a 40-character string again.

### 2. Fully Configurable Behavior

You control how GOAT works with a simple `.goatconfig` file.
Prefer automatic staging? Want shorter hashes? Use your own editor? Just change the config — no aliases or plumbing tricks needed.

### 3. Clean History View

Your commit history is displayed as an **ASCII graph**, not a cryptic log.
It’s visual, lightweight, and understandable at a glance.

---

## Example `.goatconfig`

```ini
[core]
auto_stage = true
hash_format = readable
editor = nano

[display]
show_ascii_graph = true
commit_hash_length = 8

[commit]
default_message = "WIP"
timestamp_format = "YYYY-MM-DD@HH:mm"
```

You can create your config globally (`~/.goatconfig`) or per project (`.goatconfig` in your repo).

---

## Who Is GOAT For?

- **Students** learning how version control actually works
- **Low-level developers** interested in systems programming
- **Anyone** who’s ever said “Git is too complex”

---

##  Built With

- **Language**: C (C99)
- **Hashing**: SHA-256
- **No dependencies**, no third-party tools

---

## License

This project is open-source and made for educational and experimental purposes.

Use it, break it, learn from it.

---

## Author

GOAT was built as a personal challenge to explore what Git hides under the hood, and to provide a clean, local, no-BS version control system.

For feedback, forks, or suggestions — contributions are welcome.
