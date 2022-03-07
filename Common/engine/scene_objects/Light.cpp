//
// Created by yoonki on 2/27/22.
//
#include "Light.h"

void Light::PreRender() {
    MeshObject::PreRender();
    std140_structure.position = GetPosition();
    SetColor(Color(std140_structure.Ia.x / 255, std140_structure.Ia.y / 255, std140_structure.Ia.z / 255));
}
