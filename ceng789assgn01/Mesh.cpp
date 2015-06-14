#define _CRT_SECURE_NO_DEPRECATE
#include "Mesh.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <queue>

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

void Mesh::addVertex(float* coords)
{
	int idx = (int) verts.size();
	verts.push_back(new Vertex(idx, coords));
}

void Mesh::addTriangle(int v1i, int v2i, int v3i)
{
	int idx = (int) tris.size();
	tris.push_back(new Triangle(idx, v1i, v2i, v3i));

	verts[v1i]->triList.push_back(idx);
	verts[v2i]->triList.push_back(idx);
	verts[v3i]->triList.push_back(idx);

	if (! makeVertsNeighbors(v1i, v2i) )
		addEdge(v1i, v2i);

	if (! makeVertsNeighbors(v1i, v3i) )
		addEdge(v1i, v3i);

	if (! makeVertsNeighbors(v2i, v3i) )
		addEdge(v2i, v3i);
}

bool Mesh::makeVertsNeighbors(int v, int w)
{
	//try to make v and w neighbors; return true if already neigbors

	for (int check = 0; check < (int) verts[v]->vertList.size(); check++)
		if (verts[v]->vertList[check] == w)
			return true;
	
	verts[v]->vertList.push_back(w);
	verts[w]->vertList.push_back(v);
	return false;
}

inline float distanceBetween(float* a, float* b)
{
	return sqrt( (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]));
}

void Mesh::addEdge(int a, int b)
{
	int idx = (int) edges.size();
	edges.push_back(new Edge(idx, a, b, distanceBetween(verts[a]->coords, verts[b]->coords)));

	verts[a]->edgeList.push_back(idx);
	verts[b]->edgeList.push_back(idx);
}


void Mesh::printGeodesicDistanceMatrix() {

	ofstream file; 
	char fname[100]; 
	strcpy(fname, filename);
	strcat(fname, ".matrix");
	file.open(fname);

	for (int i = 0; i < verts.size(); i++) {
		vector<int> row = shortestDistSet(i);
		for (int j = 0; j < row.size(); j++) {
			file << row[j] << " ";
		}
		file << "\n";
	}
	file.close();

}

vector<int> Mesh::shortestDistSet(int source) {

	vector<int> dist(verts.size());
	vector<Vertex *> unvisited(verts.size());

	dist[source] = 0;	// Distance from source to source

	for (int i = 0; i < verts.size(); i++) {
		if (verts[i]->idx != source) {
			dist[verts[i]->idx] = std::numeric_limits<int>::max();	// infinite distance from source to v
		}
		unvisited[i] = verts[i]; // All nodes initially in Q (unvisited nodes)
	}

	while (unvisited.size() != 0) {
		Vertex *u = NULL; //u ← vertex in Qunvisited with min dist[u]  // Source node in first case
		int tmp = std::numeric_limits<int>::max(), tmp_i;  
		for (int i = 0; i < unvisited.size(); i++) {
			if (dist[unvisited[i]->idx] < tmp) {
				tmp = dist[unvisited[i]->idx];
				u = unvisited[i];
				tmp_i = i;
			}
		}

		unvisited.erase(unvisited.begin() + tmp_i); //remove u from Qunvisited

		for (int i = 0; i < u->vertList.size(); i++) {
			if (std::find(unvisited.begin(), unvisited.end(), verts[u->vertList[i]]) != unvisited.end()) {  //v (u->vertList[i]) is still in Qunvisited.
				int alt = dist[u->idx] + distanceBetween(u->coords, verts[u->vertList[i]]->coords);
				int xx = dist[verts[u->vertList[i]]->idx];
				if (alt < xx) { // A shorter path to v has been found
					//dist[v] ← alt
					//prev[v] ← u
					dist[verts[u->vertList[i]]->idx] = alt;
				}
			}
		}
	}

	return dist;
}



vector<int> Mesh::shortestPath(int source, int target) {

	vector<int> dist(verts.size());
	vector<Vertex *> unvisited(verts.size()), prev(verts.size());

	dist[source] = 0;	// Distance from source to source
	prev[source] = NULL; // Previous node in optimal path initialization

	for (int i = 0; i < verts.size(); i++) {
		if (verts[i]->idx != source) {
			dist[verts[i]->idx] = std::numeric_limits<int>::max();	// infinite distance from source to v
			prev[verts[i]->idx] = NULL;			// Previous node in optimal path initialization
		}
		unvisited[i] = verts[i]; // All nodes initially in Q (unvisited nodes)
	}

	while (unvisited.size() != 0) {
		Vertex *u = NULL; //u ← vertex in Qunvisited with min dist[u]  // Source node in first case
		int tmp = std::numeric_limits<int>::max(), tmp_i;
		for (int i = 0; i < unvisited.size(); i++) {
			if (dist[unvisited[i]->idx] < tmp) {
				tmp = dist[unvisited[i]->idx];
				u = unvisited[i];
				tmp_i = i;
			}
		}

		if (verts[target] == u) {
			vector<int> path;
			while (prev[u->idx]!=NULL) {
				path.push_back(u->idx);
				u = prev[u->idx];
			}
			path.push_back(source);
			return path;
		}

		unvisited.erase(unvisited.begin() + tmp_i); //remove u from Qunvisited

		for (int i = 0; i < u->vertList.size(); i++) {
			if (std::find(unvisited.begin(), unvisited.end(), verts[u->vertList[i]]) != unvisited.end()) {  //v (u->vertList[i]) is still in Qunvisited.
				int alt = dist[u->idx] + distanceBetween(u->coords, verts[u->vertList[i]]->coords);
				int xx = dist[verts[u->vertList[i]]->idx];
				if (alt < xx) { // A shorter path to v has been found
					//dist[v] ← alt
					//prev[v] ← u
					dist[verts[u->vertList[i]]->idx] = alt;
					prev[verts[u->vertList[i]]->idx] = u;
				}
			}
		}
	}
	vector<int> empty;
	return empty;
}


/*vector<int> Mesh::farthestPtsSet() {
	int M = 10;
	vector<int> topM(M);// = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	vector<int> result(M);// = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	int max = 0, maxIdx = -1;
	for (int i = 0; i < verts.size(); i++) {
		int total = shortestDistTotal(i);
		//check this point's distance from previous points
		bool skip = false;
		for (int k = 0; !skip && k < result.size(); k++) {
			if (result[k] != 0) {
				vector<int> x = shortestPath(i, result[k]);
				if (x.size() < 5) {
					skip = true;
				}
			}
		}
		if (!skip) {
			if (total > 10000) {
				for (int k = 0; k < M; k++) {
					if (topM[k] < total) {
						//shift right
						for (int l = M - 1; k < l; l--) {
							topM[l] = topM[l - 1];
						}
						//insert
						topM[k] = total;
						result[k] = verts[i]->idx;
						break;
					}
				}
			}
		}
	}
	return result;
}*/

vector<int> Mesh::farthestPtsSet() {
	int M = 5, max = 0, maxIdx = -1;
	for (int i = 0; i < verts.size(); i++) {
		int total = shortestDistTotal(i);
		if (max < total) {
			max = total;
			maxIdx = i;
		}
	}
	vector<int> result;
	result.push_back(maxIdx);
	cout << "first farthest: " << maxIdx << "\n";
	for (int i = 1; i < M; i++) {
		int x = farthestTo(result);
		cout << "new farthest: " << x << "\n";
		result.push_back(x);
	}
	return result;
}
int Mesh::farthestTo(vector<int> vertices) { //finds the vertex with max sum of distances to the given vertives
	vector<vector<int>> distSetSet(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		distSetSet[i] = shortestDistSet(vertices[i]);
	}
	float max = 0, maxIdx = 0;
	for (int i = 0; i < distSetSet[0].size(); i++) {
		int total = 0;
		for (int j = 0; j < vertices.size(); j++) {
			total += distSetSet[j][i];
		}
		if (max < total) {
			max = total;
			maxIdx = i;
		}
	}
	return maxIdx;
}


int Mesh::shortestDistTotal(int idx) {
	vector<int> dist = shortestDistSet(idx);
	int total = 0;
	for (int j = 0; j < dist.size(); j++) {
		total += dist[verts[j]->idx];
	}
	return total;
}

int Mesh::shortestDistTotalF(int idx) { //reads from matrix file - very slow
	FILE* fPtr;
	char matrixFile[100];
	strcpy(matrixFile, filename);
	strcat(matrixFile, ".matrix");
	if (!(fPtr = fopen(matrixFile, "r")))
	{
		cout << "cannot read " << matrixFile << endl;
		cout << "please run the first problem to create the matrix file." << endl;
		exit(0);
	}

	for (int j = 0; j < verts.size(); j++) {
		float total = 0.0;
		for (int i = 0; i < verts.size(); i++) {
			float a;
			fscanf(fPtr, "%f ", &a);
			if (j == idx) {
				total += a;
			}
		}
		if (total > 0)
			return total;
	}

	fclose(fPtr);

	return 0;
}

//returns an int between 0 - 10 for each vertex
vector<int> Mesh::averageGeodesicDescriptor() {

	vector<float> totals(verts.size());
	float min = 0, max = 0;
	for (int i = 0; i < verts.size(); i++) {
		totals[i] = shortestDistTotal(i);
		if (i == 0) {
			min = totals[i];
			max = totals[i];
		}
		else if (totals[i] < min){
			min = totals[i];
		}
		else if (totals[i] > max){
			max = totals[i];
		}
	}
	float interval = (max - min) / 100;
	vector<int> desc(verts.size());

	for (int i = 0; i < verts.size(); i++) {
		desc[i] = (totals[i] - min) / interval;
	}

	return desc;

}