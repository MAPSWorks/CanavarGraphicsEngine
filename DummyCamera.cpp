#include "DummyCamera.h"

DummyCamera::DummyCamera(QObject *parent)
    : Camera(parent)
{
    mNodeType = Node::NodeType::DummyCamera;
}

DummyCamera::~DummyCamera() {}

void DummyCamera::onKeyPressed(QKeyEvent *) {}

void DummyCamera::onKeyReleased(QKeyEvent *) {}

void DummyCamera::onMousePressed(QMouseEvent *) {}

void DummyCamera::onMouseReleased(QMouseEvent *) {}

void DummyCamera::onMouseMoved(QMouseEvent *) {}

void DummyCamera::update(float) {}

void DummyCamera::onResized(int, int) {}
