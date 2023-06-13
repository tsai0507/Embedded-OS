#!/bin/bash

SESSION="Hw3"
PORT=5555

make clean
make

tmux has-session -t $SESSION 2>/dev/null

if [[ $? -eq 0 ]]
then
	tmux kill-session -t $SESSION
fi

tmux new-session -d -s $SESSION

tmux split-window -h -p 80
tmux split-window -v -p 75
tmux split-window -v -p 66
tmux split-window -v -p 50

tmux send-keys -t 0 "./server $PORT" C-m
sleep 1

tmux send-keys -t 1 "echo -n 'Report | Science Park | Child 5' | nc localhost $PORT" C-m
tmux send-keys -t 2 "echo -n 'Report | Science Park | Adult 2' | nc localhost $PORT" C-m
tmux send-keys -t 3 "echo -n 'Report | City God Temple | Elder 16 | Big City | Adult 2' | nc localhost $PORT" C-m
tmux send-keys -t 4 "echo -n 'Report | NYCU | Adult 2 | NYCU | Child 5' | nc localhost $PORT" C-m

tmux -2 attach-session -t $SESSION
