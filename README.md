# HunspellColorize

A simple command-line spell-checking colorizer that highlights potentially misspelled words using Hunspell.

## Description

HunspellColorize is a lightweight utility that reads text input and highlights words that are not found in the Hunspell dictionary. Originally created as a "stop-gap" tool to help with typo detection in text output, it's particularly useful when integrated with pagers like `less` or as a Git pager.

## Features

- **Real-time spell checking**: Uses Hunspell library for accurate spell checking
- **Visual highlighting**: Misspelled words are highlighted in bold using ANSI escape codes
- **Pipe-friendly**: Works seamlessly with Unix pipes and pagers
- **Git integration**: Can be used as a Git pager to highlight typos in diffs and logs
- **Lightweight**: Minimal dependencies and fast execution

## Prerequisites

- C compiler (gcc, clang, etc.)
- Hunspell library and development headers
- US English Hunspell dictionary files

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install libhunspell-dev hunspell-en-us
```

**Fedora/RHEL:**
```bash
sudo dnf install hunspell-devel hunspell-en-US
```

**macOS:**
```bash
brew install hunspell
```

## Building

```bash
make
```

This will compile the `huncolor` executable.

## Installation

To install to your local bin directory:
```bash
make install
```

Or manually copy the executable:
```bash
cp huncolor ~/bin/
# or
sudo cp huncolor /usr/local/bin/
```

## Usage

### Basic Usage
```bash
# Check a file
huncolor < textfile.txt

# Pipe from another command
echo "This is a sentance with a typo" | huncolor

# Use with less pager
cat document.txt | huncolor | less
```

### Git Integration
Set as your Git pager to highlight typos in commits and diffs:
```bash
export GIT_PAGER=huncolor
export LESS=-FRSX

# Now git commands will highlight typos
git log
git diff
git show
```

### Advanced Usage
```bash
# Combine with other tools
grep "TODO" source.c | huncolor
man some_command | huncolor
```

## Configuration

The tool currently uses US English dictionary files located at:
- `/usr/share/hunspell/en_US.aff`
- `/usr/share/hunspell/en_US.dic`

## Limitations

- Only supports US ASCII text
- Uses US English dictionary only
- No context-aware spell checking
- Does not recognize URLs, file paths, or code syntax
- Simple word-by-word analysis without grammar checking

## Contributing

This is a simple utility, but contributions are welcome! Please feel free to:
- Report bugs
- Suggest improvements
- Submit pull requests
- Add support for other languages/dictionaries
