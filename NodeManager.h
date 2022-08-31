#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Manager.h"
#include "Mesh.h"
#include "Node.h"

#include <QObject>

class CameraManager;
class LightManager;
class RendererManager;
class NodeManager : public Manager
{
    Q_OBJECT

private:
    explicit NodeManager(QObject *parent = nullptr);

public:
    bool init() override;

    Node *create(Node::NodeType type, const QString &name = QString());
    void removeNode(Node *node);
    void setSelectedNode(Node *node);
    void setSelectedNode(unsigned int nodeIndex);
    void setSelectedMesh(unsigned int meshIndex);
    void setSelectedMesh(Mesh *mesh);
    void setSelectedVertexIndex(int index);

    const QList<Node *> &nodes() const;
    static NodeManager *instance();

    void drawGUI();

    Node *selectedNode() const;
    Mesh *selectedMesh() const;
    Model *selectedModel() const;

    bool vertexSelectionEnabled() const;
    bool meshSelectionEnabled() const;
    bool meshLockEnabled() const;

private:
    void populateNodesComboBox(Node *node);
    void populateMeshesComboBox(Mesh *mesh);

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    RendererManager *mRendererManager;

    // GUI Stuff
    Node *mSelectedNode;
    Model *mSelectedModel; // Casted from mSelectedNode
    Mesh *mSelectedMesh;
    int mSelectedVertexIndex;
    QVector<Mesh *> mSelectedModelMeshes;

    bool mMeshSelectionEnabled;
    bool mVertexSelectionEnabled;
    bool mMeshLockEnabled;
};

#endif // NODEMANAGER_H
