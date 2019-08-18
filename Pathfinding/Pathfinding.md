# Pathfinding

Those 4 projects are visualizations of 4 respective pathfinding algorithms, namely Breadth First Search, Greedy Best First Search, Dijkstra's Algorithm, and A\* Algorithm. All 4 demos are completely the same except for the algorithms itself, which is a great way to compare each algorithm's performance and accuracy.

They are all set on a grid-based graph, which may affect their performance because, in a lot of cases, moving between nodes will have same cost/heuristic/priority, and the actual decision as to which node is the closest is left to C++ to decide, which usually depends on the order of their insertion (the best example of that is A\*, it really doesn't perform as well as it should even though the algorithm is implemented correctly).

*Requires "ConsoleGameEngine.h"*