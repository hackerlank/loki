#pragma once
#include "Vector.h"
#include <memory>

class Scene;

class SceneObject
{
	private:
		static uint32_t s_id;
	public:
		uint32_t tempid;		//id in scene
		std::string name;

		std::shared_ptr<Scene> scene;

		SceneObject();
		virtual ~SceneObject();

		Vector3 position;
		float dir = 0;

		virtual void SendMeToNine(){}
};
