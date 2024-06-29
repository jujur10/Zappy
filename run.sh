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
    echo "Usage: ./run.sh [map_size] [frequency] [nb_teams] [nb_players_per_teams] [players_to_start_per_team]"
    echo "Optional argument: clean (terminates all executables)"
    exit 0
fi

if [ "$#" != "5" ]; then
    echo "Usage: ./run.sh [map_size] [frequency] [nb_teams] [nb_players_per_teams] [players_to_start_per_team]"
    echo "Optional argument: clean (terminates all executables)"
    exit 0
fi

teams=()
for ((i=1; i<=$3; i++)); do
    teams+=("team$i")
done

DATE=$(date -Iminutes)
mkdir -p log

./zappy_server -p 4242 -x $1 -y $1 -n ${teams[@]} -c $4 -f $2 &

(./zappy_gui -p 4242 -h 127.0.0.1 > /dev/null) &

for ((i=1; i<=$3; i++)); do
    for ((j=1; j<=$5; j++)); do
        (./zappy_ai -p 4242 -n "team$i" -h 127.0.0.1 2> "log/aiLog$j.$DATE.team$i.log") &
    done
done

# We sleep in a subprocess to echo the desired message after the logs from the other executables appear
# Echo is used twice because it first sends the message as a command to the shell, and not a text to display
$(sleep 1; echo echo Press 'Ctrl-D' or 'q' to exit) &

stty -echo -icanon time 0 min 1
while true; do
    char=$(dd bs=1 count=1 2>/dev/null)
    if [ -z "$char" ]; then
        break
    fi
    if [ "$char" == "q" ]; then
        break
    fi
done
stty sane

cleanup