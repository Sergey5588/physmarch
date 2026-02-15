#ifndef SCENE_H
#define SCENE_H

#include"object.h"
#include"networking.h"
#include<vector>
#include<string>
#include<json.hpp>
#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#include <emscripten/fetch.h>
#endif

class Scene
{
public:
    void add_object(Object obj, std::string name);
    void delete_object(Object* obj, std::string* label);
    void update_ln();
    void UpdateUniforms();
    void OfferDownload();
    void LoadFromLocalFile();
    void LoadFromCloudFile(std::string file_name, std::string user_name);
    void LoadFromLink();
    void LoadFromLink2(std::string link);
    void SetupLoadListener();
    std::string ConvertToJson();
    std::vector<Object> objects;
    std::vector<Material> materials;
    CustomObjects custom_objects;
    std::vector<std::string> labels;
    std::vector<std::string> material_names;

    NetworkData* network_data;
    int lengths[max_object_type_count];
    void LoadFromJson(std::string file);
};
#endif