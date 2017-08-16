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
#include "HoleFiller.h"

int main(int, char ** argv)
{
	HWND window = SoWin::init(argv[0]);

	SoWinExaminerViewer * viewer = new SoWinExaminerViewer(window);

	//make a dead simple scene graph by using the Coin library, only containing a single cone under the scenegraph root
	SoSeparator * root = new SoSeparator;
	root->ref();

	SoSeparator* generalShape = new SoSeparator();

	int question = 4;
	char* filename = "bunny_with_holes.off";

	Mesh * m = new Mesh();
	m->loadOff(filename);

	/*HoleFiller hFiller(*m);
	hFiller.fillHoles();*/

	for (int i = 10; i < 12; i++) {

		//m->splitTriangle(i);

		Triangle *tri = m->tris[i];

		float coord_centroid[3] = {
			(m->verts[tri->v1i]->coords[0] + m->verts[tri->v2i]->coords[0] + m->verts[tri->v3i]->coords[0]) / 3,
			(m->verts[tri->v1i]->coords[1] + m->verts[tri->v2i]->coords[1] + m->verts[tri->v3i]->coords[1]) / 3,
			(m->verts[tri->v1i]->coords[2] + m->verts[tri->v2i]->coords[2] + m->verts[tri->v3i]->coords[2]) / 3 };

		int idx_centroid = m->addVertex(coord_centroid);

		cout << coord_centroid[0] << " " << coord_centroid[1] << " " << coord_centroid[2]  << " " << idx_centroid << endl;

		m->addTriangle(m->verts[tri->v3i]->idx, m->verts[tri->v1i]->idx, idx_centroid);
		m->addTriangle(m->verts[tri->v2i]->idx, m->verts[tri->v3i]->idx, idx_centroid);
		m->addTriangle(m->verts[tri->v1i]->idx, m->verts[tri->v2i]->idx, idx_centroid);

		m->removeTriangle(m->verts[tri->v1i]->idx, m->verts[tri->v2i]->idx, m->verts[tri->v3i]->idx);
	}


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
