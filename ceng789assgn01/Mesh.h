#pragma once

#include <vector>
#include <iostream>
using namespace std;

struct Triangle
{
	int idx; //tris[idx] is this triangle
	int v1i, v2i, v3i; //verts[v1i]-verts[v2i]-verts[v3i] forms this triangle
	float area, *normal;

	Triangle(int i, int a, int b, int c) : idx(i), v1i(a), v2i(b), v3i(c) {};
};

struct Vertex
{
	int idx; //verts[idx] is this veryex
	float* coords, //coords[0] ~ x coordinate, ..
		 * normal;
	Vertex(int i, float* c) : idx(i), coords(c) {};

	vector< int > vertList; //adjacent verts
	vector< int > edgeList; //adjacent edges
	vector< int > triList; //adjacent tris
};

struct Edge
{
	int idx; //edges[idx] is this edge
	int v1i, v2i; //verts[v1i]-verts[v2i] are the endpnts of this edge
	float length;
	Edge(int i, int a, int b, float l) : idx(i), v1i(a), v2i(b), length(l) {};
};

class Mesh
{
public:
	vector< Triangle* > tris;
	vector< Vertex* > verts;
	vector< Edge* > edges;

	void loadOff(char* fName);
	void createCube(float sideLength);
	void printGeodesicDistanceMatrix(); /* new */
	vector<int> shortestPath(int source, int target); /* new */
	vector<int> shortestDistSet(int source); /* new */
	vector<int> farthestPtsSet(); /* new */
	vector<int> averageGeodesicDescriptor(); /* new */
private:

	char* filename; /* new */

	void addVertex(float* coords);
	void addTriangle(int v1i, int v2i, int v3i);
	bool makeVertsNeighbors(int v, int w);
	void addEdge(int a, int b);
	int shortestDistTotal(int idx); /* new */
	int shortestDistTotalF(int idx); /* new */
	int farthestTo(vector<int> vertices); /* new */
};