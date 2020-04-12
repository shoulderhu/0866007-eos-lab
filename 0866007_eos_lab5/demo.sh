#!/bin/bash

set -e

SESSION="session"

make clean
make

tmux new-session -d -s $SESSION

tmux split-window -v -p 75
tmux split-window -v -p 66
tmux split-window -v -p 50

# tmux split-window -h -t 1
# tmux split-window -h -t 3
# tmux split-window -h -t 5

tmux send-keys -t 0 './lab5 4444' C-m
sleep 1

for i in {1..3}
do
	tmux send-keys -t $i 'nc localhost 4444' C-m
done

tmux -2 attach-session -t $SESSION
