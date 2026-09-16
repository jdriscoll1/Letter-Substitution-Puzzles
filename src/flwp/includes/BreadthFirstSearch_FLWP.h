#ifndef seenFLWPBFS
#define seenFLWPBFS

#include "../../structs/includes/HashMap.h"
#include "../../api/includes/FLWG-API.h"
#include "PathGameComponents.h"
struct GameComponents *findFLWPStartAndGoal(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data);

int BFS_IsFLWPStartValid(int id, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data);

/* withinTier: keep the goal, and every word on the way to it, among the words
   this board is allowed to deal. */
int chooseGoalBFS_FLWP(int id, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, int withinTier, struct DataStructures* data);

void getSolution_FLWP(int id, int goalId, struct GameComponents* gc, struct DataStructures* data); 
#endif
