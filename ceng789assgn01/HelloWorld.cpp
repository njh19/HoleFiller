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
	char* filename = "C:\\developer\\bunny_with_holes.off";

	Mesh * m = new Mesh();
	m->loadOff(filename);

	HoleFiller hFiller(*m);
	hFiller.fillHoles();

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
