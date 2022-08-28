#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Node.h"

#include <QObject>

class CameraManager;
class LightManager;
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

    const QList<Node *> &nodes() const;
    static NodeManager *instance();

    void drawGUI();

private:
    void populateComboBox(Node *node);

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    // Gui
    Node *mSelectedNode;
};

#endif // NODEMANAGER_H
