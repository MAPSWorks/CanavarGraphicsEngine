#include "DummyNode.h"

Canavar::Engine::DummyNode::DummyNode(const QString &uuid)
    : Node(uuid)
{
    mType = Node::NodeType::DummyNode;
}

Canavar::Engine::DummyNode::~DummyNode() {}
