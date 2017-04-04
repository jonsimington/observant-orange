Dana Payonk
CS 5400
Spring 2017
Game Project #3

Compiling and Running (on linux/mac):
----------------------------------------
"make" to compile
"./testRun SessionName" will run a game with name SessionName for another AI or person to join


Information:
---------------------------------------
There will be no output, game will be played using time-limited iterative deepening depth-limited minimax search with alpha-beta pruning by switching between white (high scores) and black (low scores) and searching until the alloted turn time divided by 40 average moves a game runs out.  If a depth is not completed when time runs out, the search will stop and revert to the last depth successfully completed to determine what move to make.
