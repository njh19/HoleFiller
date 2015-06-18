#include "HoleFiller.h"

HoleFiller::HoleFiller(Mesh & m)
{
	mesh = &m;
}

HoleFiller::~HoleFiller()
{
	for(int i = 0 ; i < v_holes.size() ; i++)
		for(int j = 0 ; j < v_holes[i]->size() ; j++)
			delete (*v_holes[i])[j];
}

void HoleFiller::fillHoles()
{
	cout << "identifying holes..." << endl;
	identifyHoles();
	cout << "number of holes:" << v_holes.size() << endl;
	for (int i = 0; i < v_holes.size(); i++) {
		HoleTriangles *t_hole = new HoleTriangles;
		t_holes.push_back(t_hole);
	}

	trianglulateHoles();

	for (int i = 0; i<t_holes.size(); i++) {
		cout << "refining hole: " << i << "..." << endl;
		HoleTriangles ht = *t_holes[i];
		for (int k = 0; k<ht.size(); k++) {
			refine(ht[k]->v1i, ht[k]->v2i, ht[k]->v3i);
		}
	}
}

void HoleFiller::identifyHoles()
{
	for(int i = 0; i < mesh->edges.size() ; i++)
	{
		if( !isChecked(mesh->edges[i]) && isBoundary(mesh->verts[mesh->edges[i]->v1i] , mesh->verts[mesh->edges[i]->v2i]) )
		{
			HoleVertices *v_hole = new HoleVertices;
			HoleEdges *e_hole = new HoleEdges;

			Vertex *current_vertex = mesh->verts[mesh->edges[i]->v1i];
			Edge *current_edge = mesh->edges[i];

			do
			{
				v_hole->push_back(current_vertex);
				e_hole->push_back(current_edge);
				
				for(int j = 0 ; j < current_vertex->edgeList.size() ; j++)
				{
					if(current_vertex->edgeList[j] != current_edge->idx )
					{
						Vertex *v1 = mesh->verts[mesh->edges[current_vertex->edgeList[j]]->v1i];
						Vertex *v2 = mesh->verts[mesh->edges[current_vertex->edgeList[j]]->v2i];
						if (isBoundary(v1 , v2))
						{
							current_edge = mesh->edges[current_vertex->edgeList[j]];
							current_vertex = (v1 == current_vertex) ? v2 : v1;
							break;
						}
					}					
				}
			}while(mesh->edges[i]->idx != current_edge->idx);

			v_holes.push_back(v_hole);
			e_holes.push_back(e_hole);
		}
	}
}

void HoleFiller::trianglulateHoles()
{
	for(int hole_id = 0 ; hole_id < v_holes.size() ; hole_id++)
	{
	
		cout << "triangulating hole: " << hole_id << "..." << endl;

		HoleVertices holeVertices = *v_holes[hole_id];
		int size = holeVertices.size();

		// size x size boyutunda içi 0 dolu matris yarattýk
		vector<vector<float>> minimum_weight(size , vector<float>(size , 0));
		vector<vector<int>> minimum_weight_index(size , vector<int>(size , -1));
		for(int j = 2 ; j < size ; j++)
		{
			for(int i = 0 ; i < size - j ; i++)
			{
				float min = FLT_MAX;
				int index = -1;
				int k = i + j;

				for(int m = i + 1 ; m < k ; m++)
				{
					float val = minimum_weight[i][m] + minimum_weight[m][k] + computeArea(holeVertices[i]->coords, holeVertices[m]->coords, holeVertices[k]->coords);
					if( val < min )
					{
						min = val;
						index = m;
					}
				}
				minimum_weight[i][k] = min;
				minimum_weight_index[i][k] = index;
			}
		}

		addToMesh(hole_id , minimum_weight_index , 0 , size -1 );
	}
}

void HoleFiller::refine(int idx1 , int idx2 ,int idx3)
{
	Vertex *v1 =  mesh->verts[idx1];
	Vertex *v2 =  mesh->verts[idx2];
	Vertex *v3 =  mesh->verts[idx3];

	float *coord1 = v1->coords;
	float *coord2 = v2->coords;
	float *coord3 = v3->coords;

	float coord_centroid[3];
	coord_centroid[0] = (coord1[0] + coord2[0] + coord3[0]) / 3;
	coord_centroid[1] = (coord1[1] + coord2[1] + coord3[1]) / 3;
	coord_centroid[2] = (coord1[2] + coord2[2] + coord3[2]) / 3;

	//float coord_centroid[3] = { -0.0085290000, -0.0348879993, -0.0362650007 };

	float sigma_vertices[3] = {0};
	float sigma_centroid;

	for(int j = 0 ; j < v1->edgeList.size() ; j++)
	{
		sigma_vertices[0] += mesh->edges[v1->edgeList[j]]->length;
	}
	sigma_vertices[0] /= v1->edgeList.size();


	for(int j = 0 ; j < v2->edgeList.size() ; j++)
	{
		sigma_vertices[1] += mesh->edges[v2->edgeList[j]]->length;
	}
	sigma_vertices[1] /= v2->edgeList.size();


	for(int j = 0 ; j < v3->edgeList.size() ; j++)
	{
		sigma_vertices[2] += mesh->edges[v3->edgeList[j]]->length;
	}
	sigma_vertices[2] /= v3->edgeList.size();


	sigma_centroid = (sigma_vertices[2] + sigma_vertices[1] + sigma_vertices[0])/3;

	
	float scale = sqrt(2.0);

	float temp1 = scale * mesh->distanceBetween(coord_centroid , coord1);
	float temp2 = scale * mesh->distanceBetween(coord_centroid , coord2);
	float temp3 = scale * mesh->distanceBetween(coord_centroid , coord3);
	if( ( temp1 > sigma_vertices[0] && temp1 > sigma_centroid) ||
		(temp2 > sigma_vertices[1] && temp2 > sigma_centroid) ||
		(temp3 > sigma_vertices[2] && temp3 > sigma_centroid) )
	{
		int idx_centroid = mesh->addVertex(coord_centroid);
		mesh->removeTriangle(idx1, idx2, idx3);
		//mesh->addTriangle(idx3, idx2, idx1);

		//cout << "split " << idx1 << " " << idx2 << " " << idx3 << " *" << idx_centroid << endl;

		mesh->addTriangle(idx1, idx_centroid, idx3);
		mesh->addTriangle(idx3, idx_centroid, idx2);
		mesh->addTriangle(idx2, idx_centroid, idx1);
	}
}



bool HoleFiller::isChecked(Edge *edge)
{
	for(int i= 0 ; i < e_holes.size() ; i++)
	{
		for(int j= 0 ; j < e_holes[i]->size() ; j++)
		{
			if((*e_holes[i])[j]->idx == edge->idx)
				return 1;
		}
	}
	return 0;
}

bool HoleFiller::isBoundary(Vertex *v1 , Vertex *v2)
{
	int counter = 0;
	for(int i = 0 ; i < v1->triList.size() && counter < 2; i++)
	{
		for(int j = 0 ; j < v2->triList.size() ; j++)
		{
			if(v2->triList[j] == v1->triList[i])
			{
				counter++;
				break;
			}
		}
	}
	return (counter == 1);
}


float HoleFiller::computeArea(float* coordsV1, float* coordsV2, float* coordsV3)
{
	float a = sqrt((coordsV1[0] - coordsV2[0])*(coordsV1[0] - coordsV2[0]) + (coordsV1[1] - coordsV2[1])*(coordsV1[1] - coordsV2[1]) + (coordsV1[2] - coordsV2[2])*(coordsV1[2] - coordsV2[2]));
	float b = sqrt((coordsV1[0] - coordsV3[0])*(coordsV1[0] - coordsV3[0]) + (coordsV1[1] - coordsV3[1])*(coordsV1[1] - coordsV3[1]) + (coordsV1[2] - coordsV3[2])*(coordsV1[2] - coordsV3[2]));
	float c = sqrt((coordsV3[0] - coordsV2[0])*(coordsV3[0] - coordsV2[0]) + (coordsV3[1] - coordsV2[1])*(coordsV3[1] - coordsV2[1]) + (coordsV3[2] - coordsV2[2])*(coordsV3[2] - coordsV2[2]));
	float p = (a + b + c) / 2;
	return sqrt(p*(p-a)*(p-b)*(p-c));
}


float HoleFiller::computeDihedralAngle(float* coordsV1, float* coordsV2, float* coordsV3)
{
	return 0.0;
}


float HoleFiller::computeWeight(float* coordsV1, float* coordsV2, float* coordsV3)
{
	return computeDihedralAngle(coordsV1, coordsV2, coordsV3) + computeArea(coordsV1, coordsV2, coordsV3);
}



void HoleFiller::addToMesh( int hole_id , vector<vector<int>> &minimum_weight_index , int begin , int end  )
{
	if(end - begin > 1)
	{
		HoleVertices holeVertices = *v_holes[hole_id];
		int current = minimum_weight_index[begin][end];

		mesh->addTriangle(holeVertices[begin]->idx , holeVertices[current]->idx , holeVertices[end]->idx);
		(*t_holes[hole_id]).push_back(mesh->tris[mesh->tris.size()-1]);

		addToMesh( hole_id , minimum_weight_index , begin , current );
		addToMesh( hole_id , minimum_weight_index , current , end );
	}
}