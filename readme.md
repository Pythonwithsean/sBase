# sBase

sBase is a simple command-line SaneQL database application with a REPL (Read-Eval-Print Loop) interface. This project is an implementation of a SQLite-like database from scratch.

## Features

- Command-line interface
- Help and version information
- REPL for interactive commands
- Tokenizer for parsing input commands

## Project Structure

```
.gitignore
.vscode/
    settings.json
build/
    Sbase
core/
    src/
        Repl.cpp
        Server.cpp
        Tokenizer.cpp
include/
    Tokenizer.h
makefile
readme.md
```

## Usage

### Build

To build the project, run the following command:

```sh
make b
```

### Run

To run the project, use the following command:

```sh
make r
```

### Command-line Options

- `-h` or `--help`: Display help information.
- `-v` or `--version`: Display version information.

Example:

```sh
./build/Sbase -h
```

## REPL Commands

- `exit`: Exit the REPL.
- `.help`: Display help information.
- `.version`: Display version information.

## Tokenizer

The `Tokenizer` class is responsible for parsing input commands. It is defined in [`include/Tokenizer.h`](include/Tokenizer.h) and implemented in [`core/src/Tokenizer.cpp`](core/src/Tokenizer.cpp).

### Tokenizer Class

```cpp
class Tokenizer
{
private:
    int currentLine;
    int currentChar;
    int currentToken;

public:
    Tokenizer();
    int getCurrentLine();
    int getCurrentChar();
    int getCurrentToken();
    void tokenize(std::string input);
};
```

### Tokenizer Methods

- `Tokenizer()`: Constructor that initializes the tokenizer state.
- `int getCurrentLine()`: Returns the current line number.
- `int getCurrentChar()`: Returns the current character position.
- `int getCurrentToken()`: Returns the current token.
- `void tokenize(std::string input)`: Tokenizes the input string and prints the tokens.

## Example

After building the project, you can run the REPL and enter commands:

```sh
./build/Sbase
```

In the REPL, you can enter commands like:

```
sBase> .help
sBase> .version
sBase> exit
```

## License

This project is licensed under the MIT License.
