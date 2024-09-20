# SQUARE GAME
## Description

This project is a network game with a graphical interface using GTK. The game allows two players to connect via a local network or play in local mode. The project includes server and client functionalities, as well as log management for debugging.

## Features

- Network game with a server and a client.
- Graphical interface using GTK.
- Log management for the server, client, and local mode.
- Support for AI (Artificial Intelligence).
- Game restart via the graphical interface.
- Display of game rules.

## Prerequisites

- GTK+ 3.0
- GCC (GNU Compiler Collection)
- Make

## Installation

Clone the repository:

```sh
git clone https://github.com/Gabrieleirbag1/Square-Game.git
```

Compile the project using make:

```sh
make
```

## Usage

### Start the server

To start the server, use the `-s` option followed by the port (optional):

```sh
./game -s [port]
```

### Start the client

To start the client, use the `-c` option followed by the server address and port:

```sh
./game -c <server_address> <port>
```

### Start in local mode

To start the game in local mode, use the `-l` option:

```sh
./game -l
```

### Enable AI mode

To enable AI mode, add the `-ia` option:

#### In server mode
```sh
./game -s [port] -ia
```
#### In client mode
```sh
./game -c <server_address> <port> -ia
```
#### In local mode
```sh
./game -l -ia
```

## Project Structure

- `src/` : Contains the project's source files.
- `include/` : Contains the header files.
- `build/` : Contains the generated object files.
- `docs/`: Contains the documentation.
- `versions/`: Contains old files.
- `Doxyfile` : Configuration file for generating documentation with Doxygen.

## Important Files

- `game.c` : Main entry point of the program.
- `server.c` : Contains the server logic.
- `client.c` : Contains the client logic.
- `game_gtk.c` : Contains the graphical interface logic.
- `game_logic.c` : Contains the game logic.
- `ia.c`: Contains the IA logic.
- `Makefile` : Configuration file for compiling the project.

## Documentation Generation

To generate the project documentation, use the following command:

```sh
make docs
```
A log file for network exchanges and certain errors is automatically generated at the root of the project when the executable is launched.

## Cleaning

To clean the generated files (executables, objects, docs, etc.), use the following command:

```sh
make clean
```

## Authors

- GARRONE Gabriel - [GitHub](https://github.com/Gabrieleirbag1)
- PIGEYRE Nathan - [Github](https://github.com/nathanpigeyre)
- ROMEO Raphael - [Github](https://github.com/raph-romeo)