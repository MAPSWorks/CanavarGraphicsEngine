#ifndef DUMMYNODE_H
#define DUMMYNODE_H

#include "Node.h"
#include "Selectable.h"

namespace Canavar {
namespace Engine {

class DummyNode : public Node, public Selectable
{
protected:
    friend class NodeManager;
    explicit DummyNode(QObject *parent = nullptr);
    virtual ~DummyNode();
};

} // namespace Engine
} // namespace Canavar

#endif // DUMMYNODE_H
