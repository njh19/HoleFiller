#ifndef HOLEFILLER_H
#define HOLEFILLER_H

#include "Mesh.h"

typedef vector<Vertex *> HoleVertices;
typedef vector<Edge *> HoleEdges;
typedef vector<Triangle *> HoleTriangles;

class HoleFiller
{
public:
	HoleFiller(Mesh & m);
	~HoleFiller();
	void fillHoles();

private:
	void identifyHoles();
	void trianglulateHoles();
	void refine(int idx1 , int idx2 ,int idx3);

	//identifyHoles
	bool isBoundary(Vertex *v1 , Vertex *v2);
	bool isChecked(Edge *edge);

	//trianglulateHoles
	float computeWeight(float* coordsV1, float* coordsV2, float* coordsV3);
	float computeArea(float* coordsV1, float* coordsV2, float* coordsV3);
	float computeDihedralAngle(float* coordsV1, float* coordsV2, float* coordsV3);
	void addToMesh( int hole_id , vector<vector<int>> &minimum_weight_index , int begin , int end );

private:
	vector<HoleVertices *> v_holes;
	vector<HoleEdges *> e_holes;
	vector<HoleTriangles *> t_holes;
	Mesh *mesh;
};

#endif