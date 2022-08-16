#include "NodeManager.h"

NodeManager::NodeManager(QObject *parent)
    : QObject(parent)
    , mNumberOfNodes(0)
{}

void NodeManager::addNode(Node *node)
{
    mNodes << node;

    if (node->name().simplified().isEmpty())
        node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));

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
