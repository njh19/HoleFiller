#include "Painter.h"

SoSeparator* Painter::getShapeSep(Mesh* mesh)
{
	SoSeparator* sep = new SoSeparator();

	//transformation
	//not needed

	//material
	SoMaterial* mat = new SoMaterial();
	mat->diffuseColor.setValue(1, 1, 0);
	sep->addChild(mat);

	SoShapeHints* hints = new SoShapeHints();
	hints->creaseAngle = 3.14f;
	sep->addChild(hints); //Gouraud shading

	//shape
	SoCoordinate3* coords = new SoCoordinate3();
	for (int v = 0; v < (int)mesh->verts.size(); v++) {
		coords->point.set1Value(v, mesh->verts[v]->coords[0], mesh->verts[v]->coords[1], mesh->verts[v]->coords[2]);
	}
	sep->addChild(coords);

	SoIndexedFaceSet* faceSet = new SoIndexedFaceSet();
	int nt = 0;
	for (int t = 0; t < (int) mesh->tris.size(); t++)
	{
		faceSet->coordIndex.set1Value(0 + 4*nt, mesh->tris[t]->v1i);
		faceSet->coordIndex.set1Value(1 + 4*nt, mesh->tris[t]->v2i);
		faceSet->coordIndex.set1Value(2 + 4 * nt, mesh->tris[t]->v3i);
		faceSet->coordIndex.set1Value(3 + 4 * nt++, -1);
	}
	sep->addChild(faceSet);

	return sep;
}
