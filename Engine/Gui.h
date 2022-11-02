#ifndef GUI_H
#define GUI_H

#include "FirecrackerEffect.h"
#include "Model.h"
#include "Node.h"
#include "NozzleEffect.h"
#include "PerspectiveCamera.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"

#include <imgui.h>
#include <QtImGui.h>

namespace Canavar {
namespace Engine {

class Gui
{
private:
    Gui();

public:
    static void draw();
    static void draw(Canavar::Engine::Node *node);
    static void draw(Canavar::Engine::Model *node);
    static void draw(Canavar::Engine::Sky *node);
    static void draw(Canavar::Engine::Sun *node);
    static void draw(Canavar::Engine::PerspectiveCamera *node);
    static void draw(Canavar::Engine::Terrain *node);
    static void draw(Canavar::Engine::Light *node);
    static void draw(Canavar::Engine::PointLight *node);
    static void draw(Canavar::Engine::Haze *node);
    static void draw(Canavar::Engine::NozzleEffect *node);
    static void draw(Canavar::Engine::FirecrackerEffect *node);

private:
    static Canavar::Engine::Node *mSelectedNode;
};

} // namespace Engine
} // namespace Canavar

#endif // GUI_H
