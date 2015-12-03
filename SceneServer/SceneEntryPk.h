#pragma once

#include <set>
#include <map>
#include "zSceneEntry.h"
#include "zSceneEntryIndex.h"
#include "Scene.h"

struct SceneEntryPk: public zSceneEntry
{
  public:
    SceneEntryPk(Scene* s, SceneEntryType type,const zSceneEntry::SceneEntryState state = SceneEntry_Normal):zSceneEntry(type,state), scene(s)
    {
    }
    virtual ~SceneEntryPk(){}

  public:
	Scene* scene{nullptr};
};

typedef std::vector<SceneEntryPk *> SceneEntryPk_vec;
