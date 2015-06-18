#define _CRT_SECURE_NO_DEPRECATE
#include "Mesh.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <queue>
#include <cmath>
#include <limits>

void Mesh::loadOff(char* meshFile)
{

	this->filename = meshFile;

	cout << "Mesh initializing (to " << meshFile << ")..\n";

	FILE* fPtr;
	if (! (fPtr = fopen(meshFile, "r")))
	{
		cout << "cannot read " << meshFile << endl;
		exit(0);
	}

	char off[25];
	fscanf(fPtr, "%s\n", &off); //cout << off << " type file\n";
	float a, b, c, d;	//for face lines and the 2nd line (that gives # of verts, faces, and edges) casting these to int will suffice
	fscanf(fPtr, "%f %f %f\n", &a, &b, &c);
	int nVerts = (int) a, v = 0;
//	minEdgeLen = INF;
//	maxEdgeLen = -INF;
//	edgeLenTotal = 0.0f;
	while (v++ < nVerts) //go till the end of verts coord section
	{
		fscanf(fPtr, "%f %f %f\n", &a, &b, &c);
		float* coords = new float[3];
		coords[0] = a;
		coords[1] = b;
		coords[2] = c;
		addVertex(coords); //ND: no duplicate check
	}
	//verts ready, time to fill triangles
	while (fscanf(fPtr, "%f %f %f %f\n", &d, &a, &b, &c) != EOF) //go till the end of file
	{
		addTriangle((int) a, (int) b, (int) c); //no -1 'cos idxs start from 0 for off files
	}

//	avgEdgeLen = edgeLenTotal / ((int) edges.size());
//	computeBoundingBox();
	fclose(fPtr);
	cout << "Mesh has " << (int) tris.size() << " tris, " << (int) verts.size() << " verts, " << (int) edges.size() << " edges\nInitialization done\n";
}

int Mesh::addVertex(float* coords)
{
	//cout << "adding vertex : " << coords[0] << " " << coords[1] << " " << coords[2] << endl;
	int idx = (int) verts.size();
	verts.push_back(new Vertex(idx, coords));
	return idx;
}

void Mesh::addTriangle(int v1i, int v2i, int v3i)
{
	int idx = (int)tris.size();
	tris.push_back(new Triangle(idx, v1i, v2i, v3i));

	verts[v1i]->triList.push_back(idx);
	verts[v2i]->triList.push_back(idx);
	verts[v3i]->triList.push_back(idx);

	if (!makeVertsNeighbors(v1i, v2i))
		addEdge(v1i, v2i);

	if (!makeVertsNeighbors(v1i, v3i))
		addEdge(v1i, v3i);

	if (!makeVertsNeighbors(v2i, v3i))
		addEdge(v2i, v3i);
}

bool Mesh::triangleExists(int v1i, int v2i, int v3i) {
	int idx = -1;
	for (int i = 0; i < tris.size(); i++) {
		if (tris[i]->v1i == v1i && tris[i]->v2i == v2i && tris[i]->v3i == v3i) {
			idx = tris[i]->idx;
		}
	}
	return idx != -1;
}

void Mesh::removeTriangle(int v1i, int v2i, int v3i)
{
	int idx = -1;
	for (int i = 0; i < tris.size(); i++) {
		if (tris[i]->v1i == v1i && tris[i]->v2i == v2i && tris[i]->v3i == v3i) {
			idx = tris[i]->idx;
			tris.erase(tris.begin() + i);
		}
	}

	if (idx == -1) {
		cout << "no such triangle " << endl;
		return;
	}

	for (int i = 0; i < verts[v1i]->triList.size(); i++) {
		if (idx == verts[v1i]->triList[i]) {
			verts[v1i]->triList.erase(verts[v1i]->triList.begin() + i);
		}
	}
	for (int i = 0; i < verts[v2i]->triList.size(); i++) {
		if (idx == verts[v2i]->triList[i]) {
			verts[v2i]->triList.erase(verts[v2i]->triList.begin() + i);
		}
	}
	for (int i = 0; i < verts[v3i]->triList.size(); i++) {
		if (idx == verts[v3i]->triList[i]) {
			verts[v3i]->triList.erase(verts[v3i]->triList.begin() + i);
		}
	}

	if (!makeVertsUnneighbors(v1i, v2i))
		removeEdge(v1i, v2i);

	if (!makeVertsUnneighbors(v1i, v3i))
		removeEdge(v1i, v3i);

	if (!makeVertsUnneighbors(v2i, v3i))
		removeEdge(v2i, v3i);
}

void Mesh::splitTriangle(int idx) {
	float coord_centroid[3] = {
		(verts[tris[idx]->v1i]->coords[0] + verts[tris[idx]->v2i]->coords[0] + verts[tris[idx]->v3i]->coords[0]) / 3,
		(verts[tris[idx]->v1i]->coords[1] + verts[tris[idx]->v2i]->coords[1] + verts[tris[idx]->v3i]->coords[1]) / 3,
		(verts[tris[idx]->v1i]->coords[2] + verts[tris[idx]->v2i]->coords[2] + verts[tris[idx]->v3i]->coords[2]) / 3 };

	int idx_centroid = addVertex(coord_centroid);

	removeTriangle(verts[tris[idx]->v1i]->idx, verts[tris[idx]->v2i]->idx, verts[tris[idx]->v3i]->idx);

	addTriangle(verts[tris[idx]->v3i]->idx, idx_centroid, verts[tris[idx]->v1i]->idx);
	addTriangle(verts[tris[idx]->v3i]->idx, idx_centroid, verts[tris[idx]->v2i]->idx);
	addTriangle(verts[tris[idx]->v1i]->idx, idx_centroid, verts[tris[idx]->v2i]->idx);
}

bool Mesh::makeVertsNeighbors(int v, int w)
{
	//try to make v and w neighbors; return true if already neigbors

	for (int check = 0; check < (int)verts[v]->vertList.size(); check++)
		if (verts[v]->vertList[check] == w)
			return true;

	verts[v]->vertList.push_back(w);
	verts[w]->vertList.push_back(v);
	return false;
}

bool Mesh::makeVertsUnneighbors(int v, int w)
{
	//try to make v and w unneighbors; return true if not neigbors

	for (int check = 0; check < (int)verts[v]->vertList.size(); check++)
		if (verts[v]->vertList[check] == w) {
			verts[v]->vertList.erase(verts[v]->vertList.begin() + check);
		}

	for (int check = 0; check < (int)verts[w]->vertList.size(); check++)
		if (verts[w]->vertList[check] == v) {
			verts[w]->vertList.erase(verts[w]->vertList.begin() + check);
			return false;
		}

	return true;
}

float Mesh::distanceBetween(float* a, float* b)
{
	return sqrt( (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]));
}

void Mesh::addEdge(int a, int b)
{
	int idx = (int)edges.size();
	edges.push_back(new Edge(idx, a, b, distanceBetween(verts[a]->coords, verts[b]->coords)));

	verts[a]->edgeList.push_back(idx);
	verts[b]->edgeList.push_back(idx);
}
void Mesh::removeEdge(int a, int b)
{
	int idx = -1;
	for (int i = 0; i < edges.size(); i++) {
		if ((edges[i]->v1i == a && edges[i]->v2i == b) || (edges[i]->v1i == b && edges[i]->v2i == a)) {
			idx = edges[i]->idx;
			edges.erase(edges.begin() + i);
		}
	}
	if (idx == -1)
		return;

	for (int i = 0; i < verts[a]->edgeList.size(); i++) {
		if (verts[a]->edgeList[i] == idx)
			verts[a]->edgeList.erase(verts[a]->edgeList.begin() + i);
	}
	for (int i = 0; i < verts[b]->edgeList.size(); i++) {
		if (verts[b]->edgeList[i] == idx)
			verts[b]->edgeList.erase(verts[b]->edgeList.begin() + i);
	}
}