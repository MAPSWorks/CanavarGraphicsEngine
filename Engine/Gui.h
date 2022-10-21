#ifndef GUI_H
#define GUI_H

#include "Model.h"
#include "Node.h"
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

private:
    static void drawGui(Canavar::Engine::Node *node);
    static void drawGui(Canavar::Engine::Model *node);
    static void drawGui(Canavar::Engine::Sky *node);
    static void drawGui(Canavar::Engine::Sun *node);
    static void drawGui(Canavar::Engine::PerspectiveCamera *node);
    static void drawGui(Canavar::Engine::Terrain *node);

private:
    static Canavar::Engine::Node *mSelectedNode;
};

} // namespace Engine
} // namespace Canavar

#endif // GUI_H
