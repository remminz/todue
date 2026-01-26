# todue

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![SQLite](https://img.shields.io/badge/SQLite-4E92D0?style=for-the-badge&logo=SQLite&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

### todue is a small cross-platform CLI (command line interface) to-do list app written in C with SQLite
---

### Commands

To see a list of commands run `todue help`
```
todue commands:
  help                                    | Show this screen
  load db_path                            | Load an existing database or create a new one
  reload                                  | Reload the current database
  add brief [-n <notes>] [-d <due_date>]  | Add an item
  rm {id... | --done | --all}             | Remove one or more items
  done {id... | --done | --all}           | Mark one or more items as done
  ls                                      | List all todues
  quit                                    | Exit the CLI
```
***Additional help on specific command arguments can be found by calling the command without any arguments***

### Building
```sh
git clone https://github.com/remminz/todue.git
cd todue && make
```
Run the app with `make run`

Running `todue` with no arguments will enter interactive mode for running multiple commands. Otherwise you can run one command with `todue COMMAND`.

### Installing
`make release`

Mac / Linux: `mv todue $HOME`

Windows: `mv todue.exe $USERPROFILE`

#### Optional installation tips
- add "todue.db" to global .gitignore file for project todue lists
- add "alias to=todue" to terminal configuration file for faster use

## License

This project is licensed under the [MIT License](./LICENSE).