#!/bin/bash

cleanup() {
    echo "Terminating executables..."
    pkill zappy_server
    pkill zappy_gui
    pkill zappy_ai
}

if [ "$1" == "clean" ]; then
    cleanup
    exit 0
fi

./zappy_server -p 4242 -x 100 -y 100 -n red green blue -c 34 -f 100 &

./zappy_gui -p 4242 -h 127.0.0.1 &

for i in {1..33}; do
    ./zappy_ai -p 4242 -n red -h 127.0.0.1 &
done

for i in {1..33}; do
    ./zappy_ai -p 4242 -n green -h 127.0.0.1 &
done

for i in {1..34}; do
    ./zappy_ai -p 4242 -n blue -h 127.0.0.1 &
done