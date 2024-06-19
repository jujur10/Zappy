#!/bin/bash

cleanup() {
    echo "Terminating executables..."
    pkill zappy_gui
    pkill zappy_ai
    pkill zappy_server
}

if [ "$1" == "clean" ]; then
    cleanup
    exit 0
fi

if [ "$1" == "--help" ]; then
    echo "Usage: ./run.sh [map_size] [frequency] [nb_teams] [nb_players_per_teams]"
    echo "Optional argument: clean (terminates all executables)"
    exit 0
fi

teams=()
for ((i=1; i<=$3; i++)); do
    teams+=("team$i")
done

./zappy_server -p 4242 -x $1 -y $1 -n ${teams[@]} -c $4 -f $2 &

./zappy_gui -p 4242 -h 127.0.0.1 &

for ((i=1; i<=$3; i++)); do
    for ((j=1; j<=$4; j++)); do
        ./zappy_ai -p 4242 -n team$i -h 127.0.0.1 &
    done
done
