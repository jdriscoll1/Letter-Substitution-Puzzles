# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
make             # gcc -O3 -flto -o flwo ./src/*.c ./src/*/src/*.c -lm
./flwo           # the debug console: pick a mode, name its start word, play it
                 # run from the repo root -- dictionary paths (docs/4.txt) are relative
make test        # build and run the unit tests (tests/), also from the repo root
make test-memcheck   # the same suite under valgrind -- this is what catches leaks
make clean
```

There is no incremental build and no linter: `make` recompiles every `.c` under `src/` in
one gcc invocation. The committed `flwo` and `src/flwp_game` are Linux x86-64 ELF binaries;
`gcc`/`make` are not on PATH in Git Bash on this machine, so build from WSL or an
MSYS2/MinGW shell.

`tests/` holds a dependency-free assertion harness (`test_framework.h`) plus five suites:
containers, dictionary/conversions, the game APIs, the debug starts, and one regression test
per bug fixed in the memory-leak pass. To add a test, write the function in the matching `tests/test_*.c`,
add a `RUN_TEST(...)` line to that file's `suite_*` function, and it is picked up — the
Makefile globs `tests/*.c`. Leaks are invisible to plain `make test`, so run
`make test-memcheck` when touching ownership. Tests pin `TEST_SEED` because the game APIs
choose start words with `rand()`; `open_dictionary` re-applies the seed after every load
since `initDataStructures` reseeds from the clock itself.

The sources use POSIX headers (`unistd.h`, `fcntl.h`) and `open()`/`close()` for dictionary
files, so they assume a POSIX-ish toolchain.

`src/debug_console.c` is what `main()` runs: a menu of the game modes, each of which asks
for the word to start on before it plays. Blank at that prompt falls through to the mode's
own picker. It exists because the modes choose their own start words, so without it the
position changes every run and a bug cannot be looked at twice.

`src/main.c` is a scratchpad of demo entry points (`flwg`, `flwp`, `flwc`, `flwic`, `flwt`,
`flwgp`, `level21`, ...), each a self-contained "init structures -> set parameters ->
init game -> print hints -> free" example of one game mode's API. They are the older way
round -- one mode each, parameters written into the source -- and still serve as the usage
documentation for the APIs. To run one instead of the console, call it from `main()`.

## Dictionary Files (`docs/`)

`docs/2.txt`, `docs/3.txt`, `docs/4.txt` hold 2-, 3-, and 4-letter words. Format:

```
1953                          <- word count
ware 1 3 4 16 20 ...          <- word, then the ids of every word one substitution away
```

A word's integer id is its 0-based position in the file after the count line. **Everything
downstream operates on int ids, not strings**; the adjacency lists are precomputed here, not
at runtime, so a word is "adjacent" exactly when the file says so.

`docs/{2,3,4}ranks.txt` sit beside them and are read by `Load_Obscurity`. Same shape: a count
line, then one number per line, in **the same order as the connections file**, so rank *i*
belongs to word *i* and nothing is looked up. A ranks file whose count disagrees with the
dictionary is refused whole rather than applied crookedly. The number is the word's place in a
list of the commonest English words, so smaller is commoner.

They are produced together by `docs/oldDictionaries/IntegerDocumentProducer.java` from
`Four_Letters.txt` / `Three_Letters.txt` / `Two_Letters.txt`, each line `word rank`. **The ranks
in those input files are not purely corpus frequency** — words the game shipped with before the
Scrabble list are given a rank whatever the corpus said, because frequency undersells short
concrete words (DAWS scores better than TARE in every corpus tried). Regenerating those ranks
from a frequency list alone silently drops 158 ordinary words back out of play;
`WordObscurity-test.ts` is the ratchet on that.

`initDataStructures(fd, numLetters)` takes `numLetters` as a separate argument from the file
descriptor — it must match the word length in the file being opened. Several demos in
`main.c` pass a mismatched value; copying one of those blindly produces silent corruption.

## Architecture

### Shared state: `struct DataStructures`

Created once per run by `initDataStructures(fd, numLetters)` (`src/api/src/FLWG-API.c`),
destroyed by `freeDataStructures`. Every game mode and algorithm takes a `struct
DataStructures*`. Three members:

- `W2I` — word -> id, a 26-way array of AVL/tree-set buckets (`TreeSet.h`, `HashFunctions.h`).
- `I2W` — `struct wordDataArray`, id -> `struct wordData` holding the word, its precomputed
  `connectionHeader` list, `numConnections`, plus two *mutable scratch fields*: `prevID`
  (used by BFS to reconstruct paths) and `hintFound`. The adjacencies are also kept flat in
  `connections` (same entries, same order, built once at load). Walk the list for iteration;
  read the array when you need the n-th adjacency, since indexing the list is a pointer
  chase. Anything that builds adjacencies has to fill both — `test_flat_connections_match_the_list`
  checks they agree.
- `wordSet` — a bitset over ids, one bit per word.

`WordSet` serves double duty: as the "already used in this game" marker (`markUsed_WordSet` /
`reset_WordSet`, reset between rounds) and as a general word-membership set. FLWC's goal and
avoid word lists are `WordSet`s built by `convertCharPtrPtrToWordSet` from a NULL-terminated
`char**`. Because both the word set and `wordData`'s scratch fields are shared mutable state,
algorithms that run back-to-back must reset them.

### `src/api` is the public boundary

`src/api/includes/FLWG-API.h` declares `struct DataStructures` and the entry points for the
FLWG, FLWP, and FLWGP modes; `FLWC-API.h` and `FLWT-API.h` cover the other two.
`Debug-API.h` is the same set of games started on a word you name rather than one the mode
chose: it seats the word and builds the ordinary components, so a debug game and a real one
differ only in how the first word was arrived at. It deliberately does not apply the
adjacency and distance ranges the pickers use -- naming a word is the point -- and reports
an unknown word through the mode's usual `isStartValid*`. Every mode
follows the same lifecycle, with mode-suffixed names:

`init<MODE>` -> `isStartValid<MODE>` (parameters may be unsatisfiable; **always check**) ->
`getStartWord<MODE>` -> `userEntersWord<MODE>` / `botTakesTurn<MODE>` -> `hint*<MODE>` ->
`isGameWon<MODE>` -> `freeGameComponents<MODE>`.

The `src/flwg`, `src/flwp`, `src/flwc` directories hold the game logic behind that facade;
callers (including the PHP/SWIG binding mentioned in the README) should go through the API
headers.

### Game modes

- **FLWG** (`src/flwg`) — two-player substitution game, last player who can move wins.
  `struct GameData`. Start word chosen by adjacency count range.
- **FLWP** (`src/flwp`) — pathfinder: get from start to goal. `struct GameComponents` carries
  the user's current path (`userConnections`), full undo/redo history (`storage`,
  `undoCalls`), and the BFS-computed `solution`.
- **FLWC** (`src/flwc`) — challenge mode: reach any word in a goal `WordSet` (or, inverted
  "FLWIC", avoid an avoid-set) within `numTurns`. `struct GameComponentsFLWC`.
  `isGameWonFLWC` returns -1 in progress, 0 tie/stuck, 1 goal reached, 2 avoid word hit.
- **FLWT** (`src/api/src/FLWT-API.c`) — tutorial: find N distinct adjacencies of one word.
- **FLWGP** — the generalized pathfinder: `struct GameComponentsFLWGP` simply *composes* an
  FLWP and an FLWC component, so a path game can also carry goal/avoid sets. Changes to
  either mode have to keep the composed undo/redo path working.

Start-word selection is the hard part of every mode: `Challenges.h`
(`struct StartWordParametersFLWC`, `chooseStartWord_FLWCGeneral`) and
`BreadthFirstSearch_FLWP.h` search for a word satisfying min/max adjacency counts *and*
min/max BFS distance to the goal and avoid sets. When no word qualifies, init succeeds but
`isStartValid*` returns false.

**Both pickers filter on the cheap checks, shuffle the survivors with
`Shuffle_IntArray`, and keep the first word that passes the expensive checks** (a BFS, and
for FLWC a full `is_game_winnable_FLWC` game search). Taking the first hit from a shuffled
list is the same uniform choice as scoring every word and picking one at random — verified
to select from an identical valid set — but it costs one search instead of one per word in
the dictionary. Do not "fix" this back into a full scan: that was a 40x slowdown on every
game start. For FLWP the goal search doubles as the validity test, so a start can no longer
be accepted and then fail to produce a goal.

### Which words the game may use

A word being **legal to play** and a word being one **the game deals or a bot answers with** are
different questions, and keeping them apart is what lets the dictionary be the Scrabble list
without the game being unfair. `takeUserInput` never asks how obscure a word is. Everything that
picks a word *for* the engine does.

`setObscurityCap(data, cap)` puts a ceiling on the word map (`wordDataArray.obscurityCap`), and
`isTooObscure(id, data)` / `isTooObscureForGraph(id, graph)` read it. A board sets its cap as it
is built, before the start word is chosen — after is too late.

What has to respect it, and each was missed once:

- **Four dealers**, sharing no code path: `ChooseStart_Range` (adversarial),
  `getWordWithNumberOfConnections` (turns), `chooseStartWord_FLWCGeneral` (constraint), and
  `findFLWPStartAndGoal` (the walk). The last was ungated for three commits because a test
  comment credited the walk to the turns game's picker.
- **Every bot ply the bot chooses**, but *not* the plies where the player moves — reading the
  player as restricted has the bot believing itself safer than it is. `botPly_Mirror` chooses
  nothing and needs no cap.
- **Routes**, not just their ends. A walk quotes `gc->minConnections` as the number of moves it
  takes, and the score and the first hint both read it, so the route behind that number must be
  walkable in the words the board deals. `getSolution_FLWP` measures inside the tier and falls
  back to the whole dictionary only when there is no route at all.

Two rules that are easy to get backwards:

- **The cap is not a preference.** Bands (adjacency, distance) are asked for and then widened
  when the dictionary has nothing like them — see `Relax.h`. The cap is refused on *every*
  relaxation round, and there is one extra round underneath that drops it, so nothing that could
  be dealt before can fail to be dealt now.
- **A floor and a ceiling are not measured on the same graph.** In
  `all_words_are_greater_than_min_distance_and_...`, "nothing the rule admits is nearer than N"
  is measured over the **whole dictionary** (the player may type anything, so a guard must
  assume they will), while "something is within N" is measured over **the board's own words** (a
  goal reachable only through words nobody says is not reachable). Narrowing a graph only ever
  makes distances longer, so both of those are the conservative reading.

**Hints sort rather than filter.** `Sort_ByObscurity` / `Neighbours_ByObscurity` order the
options so the commonest is offered first, and no cap is applied. A cap on top of an ordering is
provably inert — if any option is within it the commonest option is, and if none is, something
must still be offered — and being wrong about one word then costs it a place in a queue rather
than its existence. Counts (`numOptionsHint`) are deliberately uncapped too: they are claims
about the *player's* position, and a number they can check has to be the one they would get.

`OBSCURITY_UNKNOWN` does two jobs — "nothing has ranked this word", and, as the default cap, "no
limit at all". **It must therefore sit above every rank a word can actually have.** It was 99,999
while ranks came from a list truncated at fifty thousand; when the ranks went deeper that
silently stopped meaning "no cap", and the most obscure word in the dictionary sorted as less
obscure than ZOUK. It is 9,999,999 now, against a deepest real rank of ~1.65M.

### Algorithms (`src/algs`)

- **`Minimax-2.{h,c}` is the current engine.** `struct score` (word id, score, win
  percentage, depth) and `struct score_parameters` carry a *function pointer*
  `scoreFunction` — `flwg_score` (trap the opponent) or `flwc_score` (reach goal / avoid
  avoid-set) — so one alpha-beta search serves both game families. Add a game family by
  writing a new score function, not a new search.
- `Minimax.h` + `MinimaxTests.h` are the older single-purpose implementation and its
  experimental variants (`minimax_CountAtZero`, `_FiftyFifty`, `_QuitAtZero`, `_NoBeta`,
  `_ZeroOptions`), kept for comparison against the current engine — not unit tests.
- `MaxN.h` / `Hypermax.h` for more than two players; `TreeStorageNode.h` is the BFS/MCTS node.
- `MontyCarlosTreeSearch.h` is the MCTS alternative to minimax: 50,000 simulations of
  select (`traverse`, UCT) -> expand (`visit_mctsStruct`) -> play out (`rollout`) -> record
  (`backpropogate`). It borrows the shared `wordSet`, marking words as it descends and
  unmarking them on the way back up, so it must leave the set exactly as it found it — a
  stray mark silently deletes a word from the live game. `chooseRandom` (in `MinimaxTests.c`)
  is the playout policy and **must stay uniform**: the estimates mean nothing otherwise.
  Those two properties are what `tests/test_mcts.c` exists to hold down. The simulation
  count and rollout depth are hardcoded in `montyCarlosTreeSearch`; lowering them is not an
  optimization, it is a weaker search.
  Every node carries the `player` whose turn it is, and `montyCarlosTreeSearch_Multiplayer`
  takes the number of players — the caller is always player 0 and it is its turn.
  `montyCarlosTreeSearch` is that search with two. The count is not cosmetic: a playout
  only ever proves *who was left without a move*, and how many seats sit between this move
  and that one decides whether that is a win. Told two when there are three, the search
  credits most of its playouts to the wrong player and plays worse than random —
  `test_mcts_wins_a_three_player_game` is what holds that down.
- `BreadthFirstSearch.h` does distance-constrained search — it both validates/chooses
  start-goal pairs and backs the "how far am I from the goal" and "show me a path" hints.

Bot strength is selected by an int `botType` passed to `botTakesTurn*`:
`-2` mirror, `-1` maximum adjacencies, `0` random, and **any positive value is used as the
minimax search depth**.

### `src/structs`

Hand-rolled containers (`ArrayList`, `IntLinkedList`, `WordLinkedList`, `Queue`, `TreeSet`,
`HashMap`, `TranspositionTable`, `WordSet`) with no external dependencies. `HashMap.h` also
declares `Convert_WordToInt` / `Convert_IntToWord`, the conversions used everywhere.

## Conventions

- Every `.c` has a matching header in its module's `includes/`; cross-module includes are
  relative paths (`"../../structs/includes/HashMap.h"`) guarded by `#ifndef seen<Name>`.
- Ownership is manual and inconsistent: some hint functions return a `char*` the caller must
  `free` (e.g. `hintPathToGoalFLWC`), others return a pointer into `I2W` that must not be
  freed. Check the implementation before freeing a returned string. The `hint*FLWC` family
  returns `NULL` (or `-1`) when no path to a goal word exists — check before using the result.
- `initDataStructures` duplicates the file descriptor it is given, so the caller still owns
  the `fd` it opened and is the one that closes it.
- User input validation returns `enum ERROR_CODE` (`src/flwp/includes/UserInput.h`): `VALID=0`,
  `TOO_SHORT`, `TOO_LONG`, `NOT_ENOUGH_LETTERS_IN_COMMON`, `WORD_USED`, `WORD_DOES_NOT_EXIST`,
  `WRONG_ORDER`, `UNKNOWN_ERROR`, `TOO_MANY_LETTERS_IN_COMMON`. The `userEntersWord*` API
  functions propagate these codes; a word id of `-1` means "no move available / lost".
- Demo output is tagged `[GAME MESSAGE]` / `[HINT MESSAGE]` so a wrapping UI can parse it.
