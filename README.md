# SHORKMINES

An ncurses-based minesweeper game. It is a fork of Joel Ekström's [shorkmines](https://github.com/joelekstrom/shorkmines), which is a reference frontend for [libminesweeper](https://github.com/joelekstrom/libminesweeper). SHORKMINES is modified for use with SHORK Operating Systems such as [SHORK 486](https://github.com/SharktasticA/SHORK-486).

<img src="screenshots/shorkmines.png" alt="Game screenshot" height="260" width="350"> 



## Installation

### Other OS's/manual compilation
After cloning the project, `cd` to the folder and run:

```
git submodule update --init
make install
```

Terminal Mines is now installed on your system.



## Starting the game

Run the game by typing `shorkmines`. Run `shorkmines --help` or `man shorkmines`
to view additional info.

To select size/difficulty, play with the following options:

- `--width` Width of the game board
- `--height` Height of the game board
- `--mine-density` a value between 0.0 and 1.0 where 1.0 means every tile will have a mine, and 0.0 means zero tiles will have mines.

All the arguments can be written shorthand with the first letter in the name.

Example:
```
shorkmines -w 20 -h 20 -m 0.5
```



## Controls

- Movement: `hjkl`, `wasd` or arrow keys
- Place flag: `f` or `g`
- Open tile: `,` or Enter
- Place flag on unopened tile & open adjacent tiles of opened tile: ` `

The controls were inspired by nethack/vim.



## Uninstall

To completely remove terminal mines (if you compiled it yourself), run `make uninstall` in
the source folder.
