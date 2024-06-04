# Zappy Network protocol

> All packets are sent in plaintext, just like CLI programs. So no fancy structs to send to eachother. They also always end with a linebreak (\n).

Here is an example of a command that could be sent by the server:

```bash
msz 128 128
```

The first word is the command and all the others are arguments, in this case, this command is sent by the server to the GUI, and it will tell it that the map has a width of 128 and a height of 128.

## Handshake

```
Server      ->    WELCOME
Client      ->    [team_name]
Server      ->    [client_num]
Server      ->    [x] [y]
```

``team_name`` realates to the client's team, it will always be "``GRAPHICAL``" when we are talking about the GUI.

``client_num`` relates to the number of clients from the specified team that can connect. Because there can theoretically be an infinite number of GUIs, and because the ``select()`` function can only handle a maximum of 1024 conccurent connections, it will display 1024 minus the number of connected clients in this case.

``x`` and ``y`` relate to the map size.

## GUI/Server protocol

### Glossary

| **Symbol** | **Meaning**                         |
|------------|-------------------------------------|
| X          | width or horizontal position        |
| Y          | height or vertical position         |
| q0         | resource 0 (food) quantity          |
| q1         | resource 1 (linemate) quantity      |
| q2         | resource 2 (deraumere) quantity     |
| q3         | resource 3 (sibur) quantity         |
| q4         | resource 4 (mendiane) quantity      |
| q5         | resource 5 (phiras) quantity        |
| q6         | resource 6 (thystame) quantity      |
| n          | player number                       |
| O          | orientation: 1(N), 2(E), 3(S), 4(W) |
| L          | player or incantation level         |
| e          | egg number                          |
| T          | time unit                           |
| N          | name of the team                    |
| R          | incantation result                  |
| M          | message                             |
| i          | resource number                     |

### Commands

When there are 2 commands by line on this table, consider it like a question/answer from the client to the server; when the client asks for the map size, the server responds with the map size and the client acts accordingly.

| **Server**                                 | **Client** | **Details**                                   |
|--------------------------------------------|------------|-----------------------------------------------|
| msz X Y\n                                  | msz\n      | map size                                      |
| bct X Y q0 q1 q2 q3 q4 q5 q6\n             | bct X Y\n  | content of a tile                             |
| bct X Y q0 q1 q2 q3 q4 q5 q6\n * nbr_tiles | mct\n      | content of the map (all the tiles)            |
| tna N\n * nbr_teams                        | tna\n      | name of all the teams                         |
| pnw #n X Y O L N\n                         |            | connection of a new player                    |
| ppo #n X Y O\n                             | ppo #n\n   | player’s position                             |
| plv #n L\n                                 | plv #n\n   | player’s level                                |
| pin #n X Y q0 q1 q2 q3 q4 q5 q6\n          | pin #n\n   | player’s inventory                            |
| pex #n\n                                   |            | expulsion                                     |
| pbc #n M\n                                 |            | broadcast                                     |
| pic X Y L #n #n . . . \n                   |            | start of an incantation (by the first player) |
| pie X Y R\n                                |            | end of an incantation                         |
| pfk #n\n                                   |            | egg laying by the player                      |
| pdr #n i\n                                 |            | resource dropping                             |
| pgt #n i\n                                 |            | resource collecting                           |
| pdi #n\n                                   |            | death of a player                             |
| enw #e #n X Y\n                            |            | an egg was laid by a player                   |
| ebo #e\n                                   |            | player connection for an egg                  |
| edi #e\n                                   |            | death of an egg                               |
| sgt T\n                                    | sgt\n      | time unit request                             |
| sst T\n                                    | sst T\n    | time unit modification                        |
| seg N\n                                    |            | end of game                                   |
| smg M\n                                    |            | message from the server                       |
| suc\n                                      |            | unknown command                               |
| sbp\n                                      |            | command parameter                             |

## AI/Server protocol

### Getting individual's field of view

Because the individual's field of view looks like a reverse pyramid, the information of each tile is given starting from the bottom spike to the upper right base, like that:

```
9 | 10 | 11 | 12 | 13 | 14 | 15
-------------------------------
     4 |  5 |  6 |  7 |  8
    -----------------------
          1 |  2 |  3
         -------------
               0
```

Because there can be multiple objects in each tile, every object in each tile will be sent separated by a space. Each cell is separated by a comma.

> Keep in mind that tile 0 will always contain a 'player' object, because this is the tile where the individual always is. + There can be a space after each comma, this doesn't necessarily mean an object, and it can very well not be there at all for whatever reason.

To ask for its field of view, the AI will send the ``look`` command, the server's answer can look something like this:

``[player, thystame, food deraumere, ...]``

### Commands

| **Action**                   | **Command**    | **Time limit** | **Response**                           |
|------------------------------|----------------|----------------|----------------------------------------|
| move up one tile             | Forward        | 7/f            | ok                                     |
| turn 90° right               | Right          | 7/f            | ok                                     |
| turn 90° left                | Left           | 7/f            | ok                                     |
| look around                  | Look           | 7/f            | [tile1, tile2,. .. ]                   |
| inventory                    | Inventory      | 1/f            | [linemate n, sibur n,. .. ]            |
| broadcast text               | Broadcast text | 7/f            | ok                                     |
| number of team unused slots  | Connect_nbr    |                | value                                  |
| fork a player                | Fork           | 42/f           | ok                                     |
| eject players from this tile | Eject          | 7/f            | ok/ko                                  |
| death of a player            |                |                | dead                                   |
| take object                  | Take object    | 7/f            | ok/ko                                  |
| set object down              | Set object     | 7/f            | ok/ko                                  |
| start incantation            | Incantation    | 300/f          | Elevation underway Current level: k/ko |

> In case of a bad/unknown command, the server must answer “ko”.

> Relating incantations, the server sends back "``Elevation underway``" at the start of every incantation if conditions are valid. And if all the conditions are still valid at the end, it sends "``Current level: K``". In case of any issue, it sends "``ko``" at the end or at the start of the incantation.

> Fork not only creates a new egg on the tile, but it also adds a new slot in the team.

### Time

If ``f = 1``, an action that takes 7 ticks will last 7 seconds ( 7/1 ).

If ``f = 100``, and action that takes 7 ticks will last 0.07 seconds ( 7/100 ).

f is the frequency at which the simulation runs, it's set by default to 100.

### Broadcasting

When the client sends the broadcast command:
```
Broadcast [text]
```
the server sends this to all clients:
```
message K, [text]
```
Where K is the message authors direction.

The message author is anonymous.

### Eject

This command will most likely not be utilized, but in case it ever does, here is how it works:

When the client sends the ``eject`` command every individual on the same tile will get pushed away in a random direction, and will receive this command:
```
eject: K
```
Where K is the direction the player got pushed in, so that it knows where it got pushed.