#ifndef SCENE_H
#define SCENE_H

#include"object.h"
#include<vector>
#include<string>
#include<json.hpp>
#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

class Scene
{
public:
    void add_object(Object obj, std::string name);
    void update_ln();
    void UpdateUniforms();
    void OfferDownload();
    void LoadFromJson(std::string file);
    std::string ConvertToJson();
    std::vector<Object> objects;
    std::vector<Material> materials;
    std::vector<std::string> labels;
    std::vector<std::string> material_names;
    int lengths[T__LENGTH];
};
#endif