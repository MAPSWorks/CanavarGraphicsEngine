#include "DummyCamera.h"

DummyCamera::DummyCamera(QObject *parent)
    : Camera(parent)
{}

DummyCamera::~DummyCamera() {}

void DummyCamera::onKeyPressed(QKeyEvent *) {}

void DummyCamera::onKeyReleased(QKeyEvent *) {}

void DummyCamera::onMousePressed(CustomMouseEvent) {}

void DummyCamera::onMouseReleased(CustomMouseEvent) {}

void DummyCamera::onMouseMoved(CustomMouseEvent) {}

void DummyCamera::update(float) {}
