## Letter Substitution Puzzles

# Goal
The purpose of this repository is to capitalize on the power of compter processing and pathfinding and reinforcement learning algorithms to find solutions to letter substituion puzzles  

# Overview
Letter substitution puzzles follow the concept that when starting with an 'n'-letter word, a participant can change one of the letters making it a new word. For example, starting with the word "pies", changing the 'p' to a 't' converts it to "ties". From there it can be changed to a new word for, for example, "tins". 

# Four Letter Word Path
The Four Letter Word Path is a variant of the letter substitution puzzles in which a participant is given a starting word
For example, pies, and must create a path to a goal word, for example, 'tins'.
In this case, the path would be as follows: pies->ties->tins. It uses Breadth First Search to find both the shortest path
from the start to the goal word, as well as BFS to verify that the goal word is at minimum n-letters away. 

# Four Letter Word Game
The Four Letter Word Game is a game in which two or more users take turns substituting letters until a user gets stuck. 
This repository contains two algorithmic solutions to this puzzle with only two players, and two algorithms for more than two palyers. 

This codebase contains libraries for Minimax with Alpha Beta Pruning and Monte Carlos Tree Search for two players.
It contains both hypermax and max-n for more than two players.

# Building from source 
To build the four letter word game executable



To build the four letter word path executable

