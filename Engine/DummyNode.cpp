#include "DummyNode.h"

Canavar::Engine::DummyNode::DummyNode(QObject *parent)
    : Node(parent)
{
    mType = Node::NodeType::DummyNode;
}

Canavar::Engine::DummyNode::~DummyNode() {}
