#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Node.h"

#include <QObject>

class NodeManager : public QObject
{
    Q_OBJECT

private:
    explicit NodeManager(QObject *parent = nullptr);

public:
    void addNode(Node *node);
    void removeNode(Node *node);

    static NodeManager *instance();

    const QList<Node *> &nodes() const;

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;
};

#endif // NODEMANAGER_H
