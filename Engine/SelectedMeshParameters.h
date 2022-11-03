#ifndef SELECTEDMESHPARAMETERS_H
#define SELECTEDMESHPARAMETERS_H

#include <QVector4D>

namespace Canavar {
namespace Engine {

class Mesh;

class SelectedMeshParameters
{
public:
    SelectedMeshParameters();

    Mesh *mMesh;
    float mScale;
    bool mRenderVertices;
    int mSelectedVertexID;
    QVector4D mMeshStripColor;
    QVector4D mVertexColor;
    QVector4D mSelectedVertexColor;
};

} // namespace Engine
} // namespace Canavar

#endif // SELECTEDMESHPARAMETERS_H
