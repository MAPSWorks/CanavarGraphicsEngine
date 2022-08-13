#include "NodeManager.h"

NodeManager::NodeManager(QObject *parent)
    : QObject(parent)
    , mNumberOfNodes(0)
{}

void NodeManager::addNode(Node *node)
{
    mNodes << node;

    if (node->name().isEmpty())
        node->setName(QString("Node #%1").arg(mNumberOfNodes));

    mNumberOfNodes++;
}

void NodeManager::removeNode(Node *node)
{
    mNodes.removeAll(node);
}

NodeManager *NodeManager::instance()
{
    static NodeManager instance;
    return &instance;
}

const QList<Node *> &NodeManager::nodes() const
{
    return mNodes;
}
