/*
	Algorithms: Ford Fulkerson + (DFS / BFS)
	Description: Ford Fulkerson Algorihtm implemented using DFS. Also you can use
	BFS, which will change the complexity. Both implemented so just comment line 69
	and uncomment line 70 to change algorihtms. Ford Fulkerson is used to find Max 
	Flow or Min Cut. For the sake of simplicity I used 2D array to represent the 
	graph.
	Author: BedirT
*/
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <cstring>

#include "maxflow.h"

#define maxN 2500
using namespace std;

bool BFS(int **ways, int *parent, bool *visited, int n, int s, int t)
{
	queue<int> q;

	parent[s] = -1;
	q.push(s);

	while (!q.empty())
	{

		int u = q.front();
		q.pop();

		for (int i = 0; i < n; ++i)
		{
			if (!visited[i] && ways[u][i] > 0)
			{
				q.push(i);
				parent[i] = u;
				visited[i] = true;
			}
		}
	}

	return visited[t];
}

int FordFulkerson(int **graph, int n, int s, int t)
{
	int **rGraph = (int **)malloc(3000 * sizeof(int *));

	for (int i = 0; i < n; i++)
	{
		rGraph[i] = (int *)malloc(3000 * sizeof(int));
	}

	int parent[2501];
	bool visited[2501];

	// Replicate the graph for residual one
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			rGraph[i][j] = graph[i][j];

	int max_flow = 0;

	while (BFS(rGraph, parent, visited, n, s, t))
	{
		memset(visited, false, sizeof(visited));
		int path_flow = 0xfffffff; // Current flow to add on the max flow later

		for (int v = t; v != s; v = parent[v])
		{
			int u = parent[v];
			path_flow = min(path_flow, rGraph[u][v]);
		}

		for (int v = t; v != s; v = parent[v])
		{
			int u = parent[v];
			rGraph[u][v] -= path_flow;
			rGraph[v][u] += path_flow;
		}

		max_flow += path_flow;
	}
	return max_flow;
}

int maxflow(ifstream &infile)
{
	int n, m;

	infile >> n >> m; // n: nodes, m: edges

	int **graph = (int **)malloc(3000 * sizeof(int *));

	for (int i = 0; i < n; i++)
	{
		graph[i] = (int *)malloc(3000 * sizeof(int));
	}

	for (int j = 0, a, b, k; j < m; ++j)
	{
		infile >> a >> b >> k;
		graph[a][b] = k;
	}
	int s = 0, t = n - 1;

	return FordFulkerson(graph, n, s, t);
}