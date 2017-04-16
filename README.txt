Dana Payonk
CS 5400
Spring 2017
Game Project #4

Compiling and Running (on linux/mac):
----------------------------------------
"make" to compile
"./testRun SessionName" will run a game with name SessionName for another AI or person to join


Information:
---------------------------------------
There will be no output, game will be played using time-limited iterative deepening depth-limited minimax search with alpha-beta pruning, quiescence search, and history table by switching between white (high scores) and black (low scores).  When searching is complete, the move with the top depth level (meaning the move can be made that turn) and the highest move count will be made.  A quiescence search is defined by any move that changes the score of the board (capturing a piece, promotion, check, etc.).
