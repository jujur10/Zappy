# Zappy Project

## Overview

Zappy is a network-based game.
The game involves several teams
competing on a tile-based map to gather resources
and achieve the highest level of elevation.
The game consists of three main parts: a server,
a graphical user interface (GUI), and an artificial intelligence (AI) client.

## Project Structure

- **Server** (`zappy_server`): Written in C, it handles the game world
  and player interactions.
- **GUI** (`zappy_gui`): Developed in C++, it provides a visual
  representation of the game world.
- **AI Client** (`zappy_ai`): Can be written in any language,
  it controls the player's actions autonomously.

## Gameplay

- **Objective**: Teams must gather resources and elevate their players.
  The first team with six players at the maximum elevation wins.
- **Map**: The game takes place on a continuous,
  wrap-around map with no relief features.
- **Resources**: The world contains various resources like
  food and stones (linemate, deraumere, sibur, mendiane, phiras, thystame)
  essential for survival and elevation.

## Key Features

1. **Environment Management**: The server generates and manages the world,
   including spawning resources at regular intervals.
2. **Player Interaction**: Players can move, gather resources, communicate,
   reproduce, and eject others from tiles.
3. **AI Control**: The AI's client drives the player's actions without user
   intervention after launch.
4. **Graphical Interface**: The GUI provides a 3D visualization of the game
   world, allowing players to observe the game's progress.

## Commands and Usage

### Server

```
./zappy_server -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq
```
- `-p`: Port number
- `-x`: Width of the world
- `-y`: Height of the world
- `-n`: Team names
- `-c`: Number of clients per team
- `-f`: Frequency of action execution

### GUI

```
./zappy_gui -p port -h machine
```
- `-p`: Port number
- `-h`: Hostname of the server

### AI Client

```
./zappy_ai -p port -n name -h machine
```
- `-p`: Port number
- `-n`: Team name
- `-h`: Hostname of the machine (default is localhost)

## Conclusion

Zappy is an engaging network game that combines strategy,
resource management, and team collaboration.
With its flexible AI component and detailed GUI,
it offers a comprehensive and interactive gaming experience.
