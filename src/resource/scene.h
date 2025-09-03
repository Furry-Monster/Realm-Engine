#pragma once

#include "resource/model.h"

#include <map>

namespace RealmEngine
{
    class Scene
    {
    public:
    private:
        std::map<unsigned int, Model> models;
    };
} // namespace RealmEngine