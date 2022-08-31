#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Mesh.h"
#include "Node.h"

#include <QObject>

class CameraManager;
class LightManager;
class RendererManager;
class NodeManager : public QObject
{
    Q_OBJECT

private:
    explicit NodeManager(QObject *parent = nullptr);

public:
    Node *create(Node::NodeType type, const QString &name = QString());
    void removeNode(Node *node);
    void setSelectedNode(Node *node);
    void setSelectedNode(unsigned int nodeIndex);
    void setSelectedMesh(unsigned int meshIndex);
    void setSelectedMesh(Mesh *mesh);

    const QList<Node *> &nodes() const;
    static NodeManager *instance();

    void drawGUI();

private:
    void populateNodesComboBox(Node *node);
    void populateMeshesComboBox(Mesh *mesh);

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    RendererManager *mRendererManager;

    // Gui
    Node *mSelectedNode;
    Mesh *mSelectedMesh;
    Model *mSelectedModel; // Casted from mSelectedNode
    QVector<Mesh *> mSelectedModelMeshes;
};

#endif // NODEMANAGER_H
