#include "Haze.h"

Canavar::Engine::Haze::Haze()
    : Node()
    , mColor(0.33f, 0.38f, 0.47f)
    , mEnabled(true)
    , mDensity(1.0f)
    , mGradient(1.5f)
{
    mType = Node::NodeType::Haze;
    mName = "Haze";
    mSelectable = false;
}

Canavar::Engine::Haze *Canavar::Engine::Haze::instance()
{
    static Haze instance;
    return &instance;
}
