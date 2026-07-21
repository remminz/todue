# todue
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![SQLite](https://img.shields.io/badge/SQLite-4E92D0?style=for-the-badge&logo=SQLite&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

todue is a small cross-platform CLI (command line interface) to-do list app written in C with SQLite

## Commands
To see a list of commands run `todue help`
```
todue commands:
  help                                                 | Show this screen
  load {db_path | --home}                              | Load an existing database or create a new one
  reload                                               | Reload the current database
  init                                                 | Initialize an empty todue project
  add <brief> [-n notes] [-d due_date]                 | Add an item
  edit <id> [-b <brief>] [-n <notes>] [-d <due_date>]  | Edit one or more details of an item
  rm {id... | --done | --all}                          | Remove one or more items
  done {id... | --all}                                 | Mark one or more items as done
  ls                                                   | List all todues
  config {list | create | set <setting> <value>}       | View, create, or edit config file
  quit                                                 | Exit the CLI
```
*Additional help on specific command arguments can be found by running the command without any arguments*

## Building
***Windows users: use a Unix like terminal such as Git Bash or WSL when building and installing with make***

```sh
git clone https://github.com/remminz/todue.git
cd todue && make
```

Run the app with `make run`

Running `todue` with no arguments will enter interactive mode for running multiple commands. Otherwise you can run one command with `todue COMMAND`.

## Installing
1. `make -B release`
2. **Mac / Linux:** Install to `/usr/local/bin/`: `sudo make install`<br>
   **Windows:** Install to `%LOCALAPPDATA%/Programs/todue/`: `make install`
3. Make sure binary location is in PATH

Install to `path/bin/`: `make install PREFIX=path`

Install to `path/`: `make install BINDIR=path`

#### Optional installation tips
- add ".todue/" to global .gitignore file for project todue lists
- add "alias to=todue" to terminal configuration file for faster use

### Uninstalling
Just removes the binary, leaves the app data

`make uninstall` follows the same conventions as `make install`

## Config
Run `todue config list` to view the current config

If no config file exists yet, these are the default values that the app sets

To make a new config file with the defaults, run `todue config create`

Any missing config fields will use defaults when running the app

## License
This project is licensed under the [MIT License](./LICENSE).
