#include "Mesh.h"

#include <Inventor/nodes/SoSeparator.h>
//#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

class Painter
{
public:
	SoSeparator* getShapeSep(Mesh* mesh);
private:

};