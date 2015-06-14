#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/SbVec3d.h>

#include "Mesh.h"
#include "Painter.h"

int main(int, char ** argv)
{
	HWND window = SoWin::init(argv[0]);

	SoWinExaminerViewer * viewer = new SoWinExaminerViewer(window);

	//make a dead simple scene graph by using the Coin library, only containing a single cone under the scenegraph root
	SoSeparator * root = new SoSeparator;
	root->ref();

	SoSeparator* generalShape = new SoSeparator();

	int question = 4;
	char* filename = "C:\\developer\\bunny_without_holes.off";


	argv[2] = "C:\\developer\\bunny_with_holes.off";/*
	argv[3] = "C:\\developer\\bunny_without_holes.off";*/

	Mesh * m = new Mesh();
	m->loadOff(filename);

	//if (question == 1) {
	//	m->printGeodesicDistanceMatrix(); //prints the geodesic distance matrix into <filename>.matrix file
	//}

	//else if (question == 2) {

	//	int start = 50, end = 100;

	//	vector<int> path = m->shortestPath(start, end);

	//	//Instantiate the separator, which we'll return at the end of the method.
	//	SoSeparator * sep1 = new SoSeparator();
	//	SoSeparator * sep2 = new SoSeparator();

	//	float *mVert[1][3] = { { 0, 0, 0 } };
	//	SoSphere * sphere1 = new SoSphere;
	//	SoSphere * sphere2 = new SoSphere;
	//	sphere1->radius = 2.5f;
	//	sphere2->radius = 2.5f;

	//	SoTranslation *mTranslation1 = new SoTranslation;
	//	mTranslation1->translation.setValue(m->verts[path[0]]->coords[0], m->verts[path[0]]->coords[1], m->verts[path[0]]->coords[2]);
	//	SoTranslation *mTranslation2 = new SoTranslation;
	//	mTranslation2->translation.setValue(m->verts[path[path.size() - 1]]->coords[0], m->verts[path[path.size() - 1]]->coords[1], m->verts[path[path.size() - 1]]->coords[2]);

	//	SoMaterial *red = new SoMaterial;
	//	red->ambientColor.setValue(.2, .0, .0);
	//	red->diffuseColor.setValue(.6, .0, .0);
	//	red->specularColor.setValue(.5, .0, .0);
	//	red->shininess = .5;

	//	sep1->addChild(mTranslation1);
	//	sep1->addChild(red);
	//	sep1->addChild(sphere1);

	//	sep2->addChild(mTranslation2);
	//	sep2->addChild(red);
	//	sep2->addChild(sphere2);

	//	SoCoordinate3 * sc3 = new SoCoordinate3();

	//	SbVec3f *vertices = new SbVec3f[path.size()];
	//	for (int i = 0; i < path.size(); i++) {
	//		SbVec3f vertex(m->verts[path[i]]->coords[0], m->verts[path[i]]->coords[1], m->verts[path[i]]->coords[2]);	// given X,Y point with unknown Z value
	//		vertices[i] = vertex;
	//	}

	//	sc3->point.setValues(0, path.size(), vertices);

	//	SoSeparator * sep3 = new SoSeparator();
	//	//Instantiate the lineset object.
	//	SoLineSet * sls = new SoLineSet();

	//	//Set up the "material"
	//	SoMaterial * sm = new SoMaterial();
	//	sm->diffuseColor.setValue(.6, .0, .0);
	//	sep3->addChild(sm);

	//	//Set up the line style
	//	SoDrawStyle *sds = new SoDrawStyle();
	//	sds->lineWidth.setValue(5.0f); //Line thickness
	//	sep3->addChild(sds); //Add it to the separator

	//	//Add the coords to the separator - this is the locations of the points.
	//	//The lineset looks to the last set of coords added to the scene forlocations of its points
	//	sep3->addChild(sc3);

	//	//Add the line set to the separator
	//	sep3->addChild(sls);

	//	root->addChild(sep1);
	//	root->addChild(sep2);
	//	root->addChild(sep3);

	//}

	//else if (question == 3) {

	//	vector<int> maxIdx = m->farthestPtsSet();

	//	for (int i = 0; i < maxIdx.size(); i++) {

	//		SoSeparator * highlight = new SoSeparator;

	//		float *mVert[1][3] = { { 0, 0, 0 } };
	//		SoSphere * sphere = new SoSphere;
	//		sphere->radius = 5.0f;

	//		SoTranslation *mTranslation = new SoTranslation;
	//		mTranslation->translation.setValue(m->verts[maxIdx[i]]->coords[0], m->verts[maxIdx[i]]->coords[1], m->verts[maxIdx[i]]->coords[2]);

	//		SoMaterial *red = new SoMaterial;
	//		red->ambientColor.setValue(.2, .0, .0);
	//		red->diffuseColor.setValue(.6, .0, .0);
	//		red->specularColor.setValue(.5, .0, .0);
	//		red->shininess = .5;

	//		highlight->addChild(mTranslation);
	//		highlight->addChild(red);
	//		highlight->addChild(sphere);

	//		root->addChild(highlight);
	//	}

	//}

	//else if (question == 4) {

	//	float colors[11][3] =
	//	{
	//		{ 1.0, .0, .0 },
	//		{ 1.0, .0, .0 },
	//		{ 1.0, 0.5, .0 },
	//		{ 1.0, 1.0, .0 },
	//		{ 0.5, 1.0, .0 },
	//		{ .0, 1.0, .0 },
	//		{ .0, 1.0, 0.5 },
	//		{ .0, 1.0, 1.0 },
	//		{ .0, 0.5, 1.0 },
	//		{ .0, .0, 1.0 },
	//		{ .5, .0, 1.0 }
	//	};

	//	vector<int> desc = m->averageGeodesicDescriptor();

	//	SoSeparator* sep = new SoSeparator();
	//	for (int i = 0; i < m->verts.size(); i++) {
	//		
	//		cout << "vertex " << i << " interval : " << desc[m->verts[i]->idx] << "\n";

	//		SoSeparator * highlight = new SoSeparator;

	//		float *mVert[1][3] = { { 0, 0, 0 } };
	//		SoSphere * sphere = new SoSphere;
	//		sphere->radius = 2.0f;

	//		SoTranslation *mTranslation = new SoTranslation;
	//		mTranslation->translation.setValue(m->verts[i]->coords[0], m->verts[i]->coords[1], m->verts[i]->coords[2]);

	//		SoMaterial *red = new SoMaterial;
	//		red->ambientColor.setValue(colors[desc[m->verts[i]->idx]][0], colors[desc[m->verts[i]->idx]][1], colors[desc[m->verts[i]->idx]][2]);
	//		red->diffuseColor.setValue(colors[desc[m->verts[i]->idx]][0], colors[desc[m->verts[i]->idx]][1], colors[desc[m->verts[i]->idx]][2]);
	//		red->specularColor.setValue(colors[desc[m->verts[i]->idx]][0], colors[desc[m->verts[i]->idx]][1], colors[desc[m->verts[i]->idx]][2]);
	//		red->shininess = .5;

	//		highlight->addChild(mTranslation);
	//		highlight->addChild(red);
	//		highlight->addChild(sphere);

	//		root->addChild(highlight);
	//	}
	//	root->addChild(sep);
	//}
	
	Painter* painter = new Painter();
	generalShape = painter->getShapeSep(m);

	root->addChild(generalShape);

	viewer->setSceneGraph(root);
	viewer->show();

	SoWin::show(window);
	SoWin::mainLoop();
	delete viewer;
	root->unref();
	return 0;
}
