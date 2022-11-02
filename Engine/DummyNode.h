#ifndef DUMMYNODE_H
#define DUMMYNODE_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class DummyNode : public Node
{
protected:
    friend class NodeManager;
    DummyNode(const QString &uuid = QString());
    virtual ~DummyNode();
};

} // namespace Engine
} // namespace Canavar

#endif // DUMMYNODE_H
