#include "DummyCamera.h"

DummyCamera::DummyCamera(QObject *parent)
    : PerspectiveCamera(parent)
{
    mNodeType = Node::NodeType::DummyCamera;
}

DummyCamera::~DummyCamera() {}

void DummyCamera::onMouseDoubleClicked(QMouseEvent *) {}

void DummyCamera::onMousePressed(QMouseEvent *) {}

void DummyCamera::onMouseReleased(QMouseEvent *) {}

void DummyCamera::onMouseMoved(QMouseEvent *) {}

void DummyCamera::onWheelMoved(QWheelEvent *) {}

void DummyCamera::onKeyPressed(QKeyEvent *) {}

void DummyCamera::onKeyReleased(QKeyEvent *) {}

void DummyCamera::update(float) {}
