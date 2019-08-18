#include "ConsoleGameEngine.h"

#include <map>

const int GRAPH_WIDTH = 20;
const int GRAPH_HEIGHT = 20;

const int NODE_WIDTH = 4;
const int NODE_HEIGHT = 4;

class BreadthFirstSearch : public ConsoleGameEngine
{
	struct Node
	{
		bool obstacle = false;
		std::vector<Node*> neighbors;

		int x, y; //Top-left coordinates in screen space
		int ox, oy; //Center coordinates in screen space
	};

	Node *graph;

	std::queue<Node*> frontier;
	std::map<Node*, Node*> cameFrom;

	Node* start = nullptr;
	Node* finish = nullptr;

	bool OnStart() override
	{
		SetApplicationTitle(L"Breadth First Search");

		graph = new Node[GRAPH_WIDTH * GRAPH_HEIGHT];

		start = &graph[0];
		finish = &graph[GRAPH_WIDTH * GRAPH_HEIGHT - 1];

		//Give OnUpdate() a nod that pathfinding may be started
		Search();

		//Accumulate X and Y offsets to form outline between nodes
		int xOffset = 1;
		int yOffset = 1;

		//Go through each node and pre-compute everything that's possible
		for (int i = 0; i < GRAPH_WIDTH; i++)
		{
			for (int j = 0; j < GRAPH_HEIGHT; j++)
			{
				int node = GRAPH_WIDTH * j + i;

				int x = i * NODE_WIDTH + xOffset;
				int y = j * NODE_HEIGHT + yOffset;

				graph[node].x = x;
				graph[node].y = y;

				graph[node].ox = x + (NODE_WIDTH / 2);
				graph[node].oy = y + (NODE_HEIGHT / 2);

				if (i + 1 < GRAPH_WIDTH)
					graph[node].neighbors.push_back(&graph[GRAPH_WIDTH * j + (i + 1)]);

				if (j - 1 >= 0)
					graph[node].neighbors.push_back(&graph[GRAPH_WIDTH * (j - 1) + i]);

				if (i - 1 >= 0)
					graph[node].neighbors.push_back(&graph[GRAPH_WIDTH * j + (i - 1)]);

				if (j + 1 < GRAPH_HEIGHT)
					graph[node].neighbors.push_back(&graph[GRAPH_WIDTH * (j + 1) + i]);

				yOffset++;
			}
			yOffset = 1;
			xOffset++;
		}

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Let user place obstacles or move starting node
		if (GetKey(VK_LBUTTON).held)
		{
			int i, j;
			if (MouseWithinNode(i, j))
			{
				Node* clicked = &graph[GRAPH_WIDTH * j + i];

				if (GetKey(VK_LSHIFT).held)
				{
					if (!clicked->obstacle)
						start = clicked;
				}
				else
				{
					if (clicked != start && clicked != finish)
						clicked->obstacle = true;
				}

				Search();
			}
		}

		//Let user remove obstacles or move ending node
		if (GetKey(VK_RBUTTON).held)
		{
			int i, j;
			if (MouseWithinNode(i, j))
			{
				Node* clicked = &graph[GRAPH_WIDTH * j + i];

				if (GetKey(VK_LSHIFT).held)
				{
					if (!clicked->obstacle)
						finish = clicked;
				}
				else
				{
					if (clicked != start && clicked != finish)
						clicked->obstacle = false;
				}

				Search();
			}
		}

		//The algorithm itself
		while (!frontier.empty())
		{
			Node* current = frontier.front();

			if (current == finish)
				break;

			frontier.pop();

			for (auto &neighbor : current->neighbors)
			{
				if (!neighbor->obstacle && cameFrom.find(neighbor) == cameFrom.end())
				{
					frontier.push(neighbor);
					cameFrom[neighbor] = current;
				}
			}
		}

		//Construct path if it exists
		std::vector<Node*> path;

		//Path exists if there is a node connected to the finish
		if (cameFrom[finish] != nullptr)
		{
			Node* current = finish;
			while (current != start)
			{
				path.push_back(current);
				current = cameFrom[current];
			}
			path.push_back(start);
		}
		
		//Redraw the graph
		ClearScreen();

		for (int i = 0; i < GRAPH_WIDTH * GRAPH_HEIGHT; i++)
		{
			if (&graph[i] == finish)
				DrawFilledRectangle(graph[i].x, graph[i].y, graph[i].x + NODE_WIDTH - 1, graph[i].y + NODE_HEIGHT - 1, PIXEL_SOLID, FG_GREEN);
			else if (&graph[i] == start)
				DrawFilledRectangle(graph[i].x, graph[i].y, graph[i].x + NODE_WIDTH - 1, graph[i].y + NODE_HEIGHT - 1, PIXEL_SOLID, FG_RED);
			else if (graph[i].obstacle)
				DrawFilledRectangle(graph[i].x, graph[i].y, graph[i].x + NODE_WIDTH - 1, graph[i].y + NODE_HEIGHT - 1, PIXEL_SOLID, FG_DARK_GRAY);
			else
				DrawFilledRectangle(graph[i].x, graph[i].y, graph[i].x + NODE_WIDTH - 1, graph[i].y + NODE_HEIGHT - 1, PIXEL_SOLID, FG_DARK_BLUE);
		}

		//Highlight each node we've inspected
		for (auto &node : cameFrom)
		{
			Node* visited = node.first;

			if (visited != start && visited != finish)
			{
				DrawFilledRectangle(visited->x, visited->y, visited->x + NODE_WIDTH - 1, visited->y + NODE_HEIGHT - 1, PIXEL_SOLID, FG_DARK_YELLOW);
			}
		}

		//Draw the path if it exists
		Node* prevNode = nullptr;

		for (auto &node : path)
		{
			if (prevNode != nullptr)
			{
				DrawLine(prevNode->ox, prevNode->oy, node->ox, node->oy, PIXEL_SOLID, FG_WHITE);
			}

			prevNode = node;
		}

		return true;
	}

	bool OnDestroy() override
	{
		delete[] graph;

		return true;
	}

	void Search()
	{
		std::queue<Node*> empty;
		std::swap(frontier, empty);
		frontier.push(start);

		cameFrom.clear();
		cameFrom[start] = nullptr;
	}


	bool MouseWithinNode(int &mouse_i, int &mouse_j)
	{
		for (int i = 0; i < GRAPH_WIDTH; i++)
		{
			for (int j = 0; j < GRAPH_HEIGHT; j++)
			{
				Node &current = graph[GRAPH_WIDTH * j + i];

				if (GetMouseX() >= current.x && GetMouseX() < current.x + NODE_WIDTH &&
					GetMouseY() >= current.y && GetMouseY() < current.y + NODE_HEIGHT)
				{
					mouse_i = i;
					mouse_j = j;
					return true;
				}
			}
		}
		return false;
	}
};

int main()
{
	//Screen size is calculated based on graph configuration
	int screenWidth = (GRAPH_WIDTH * NODE_WIDTH) + (GRAPH_WIDTH + 1);
	int screenHeight = (GRAPH_HEIGHT * NODE_HEIGHT) + (GRAPH_HEIGHT + 1);

	BreadthFirstSearch demo;
	if (demo.ConstructScreen(screenWidth, screenHeight, 8, 8))
		demo.Start();

	return 0;
}