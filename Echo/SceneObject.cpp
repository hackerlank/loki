#include "SceneObject.h"

uint32_t SceneObject::s_id = 0;

SceneObject::SceneObject():tempid(++s_id)
{
}

SceneObject::~SceneObject()
{
}
