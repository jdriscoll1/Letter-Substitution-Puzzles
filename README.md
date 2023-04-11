# Letter Substitution Puzzles

This repository contains code for solving letter substitution puzzles using pathfinding and reinforcement learning algorithms. The goal is to find the optimal path between two words by changing one letter at a time.

# Overview
Letter substitution puzzles involve changing one letter in a word to create a new word. For example, starting with the word "pies" and changing the 'p' to a 't' creates the word "ties". The Four Letter Word Path is a variant of the puzzle where a participant is given a starting word and must create a path to a goal word.

This repository contains libraries for solving the Four Letter Word Path using Breadth First Search to find the shortest path between the start and goal words, and to verify that the goal word is at minimum n-letters away.

Additionally, the repository includes the Four Letter Word Game, a two-player puzzle game where players take turns substituting letters until one player gets stuck. The codebase includes algorithms for Minimax with Alpha Beta Pruning and Monte Carlos Tree Search for two players, and both hypermax and max-n for more than two players.

# Building from Source
To build the Four Letter Word Game executable, follow these steps:

1. Clone the repository to your local machine.
```bash
git clone https://github.com/username/Letter-Substitution-Puzzles.git
```
2. Navigate to the project directory and compile the source code using your preferred compiler.
```bash
cd Letter-Substitution-Puzzles
make
```
3. Run the executable to start the game.
```bash 
./flwo
```
To build the Four Letter Word Path executable, repeat the same steps with the FourLetterWordPath directory.

# Contributing
Contributions to this repository are welcome. Please create a pull request with your changes and ensure they adhere to the overall design and functionality of the project. If you encounter any issues, please open a new issue in the repository.
