# Zappy Network protocol

> All packets are sent in plaintext, just like CLI programs. So no fancy structs to send to eachother. They also always end with a linebreak (\n).

Here is an example of a command that could be sent by the server:

```bash
msz 128 128
```

The first word is the command and all the others are arguments, in this case, this command is sent by the server to the GUI, and it will tell it that the map has a width of 128 and a height of 128.

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
