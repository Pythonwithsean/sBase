# Getting Started

## Installation

There's a few ways to install TobsDB.

### 1. go install

```bash
$ go install github.com/tobshub/tobsdb/cmd/tdb@latest
```

### 2. Github Releases

Navigate to the [releases page](https://github.com/tobshub/tobsdb/releases) and download the latest release.

### 3. Docker

**Coming soon**


## Configuration

### Command line flags

- `-db=<path>`: the path to the db.tdb file (or where it should be).
If the value is relative then it is resolved from the current working directory.
- `-m`: when used or set to true, the database data remains in memory and is not written a file when the program exits. Defaults to false.
- `-port=<port>`: the listening port. Defaults to 7085 (tobs in leet-speak :v)
- `-log`: optionally print logs. Defaults to false
- `-dbg`: optionally print extra logs. Defaults to false
- `-u`: override the set username to use when running the program. Defaults to ENV.TDB_USER
- `-p`: override the set password to use when running the program. Defaults to ENV.TDB_PASS

### Environment variables

- `TDB_USER`: set the username to use when running the program.
- `TDB_PASS`: set the password to use when running the program.
