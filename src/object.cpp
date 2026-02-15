#include "object.h"
#include<vector>

std::string CustomObjects::generateFragmentShader(std::string shader_template)
{
    size_t implementation_idx = shader_template.find("//***Custom_object_implementation_placeholder***//");
    std::string shader = shader_template.substr(0, implementation_idx);
    for (CustomObject object : objects)
    {
        shader += object.implementation;
    }

    size_t distance_computation_idx = shader_template.find("//***Custom_object_distance_computation_placeholder***//");
    shader += shader_template.substr(implementation_idx, distance_computation_idx - implementation_idx);
    for (CustomObject object : objects)
    {
        shader += ("PROCESS_OBJECT_TYPE(" + std::to_string(object.id) + ", " + object.name + ")\n");
    }

    size_t material_computation_idx = shader_template.find("//***Custom_object_material_computation_placeholder***//");
    shader += shader_template.substr(distance_computation_idx, material_computation_idx - distance_computation_idx);
    for (CustomObject object : objects)
    {
        shader += ("PROCESS_MATERIAL_ID(" + std::to_string(object.id) + ", " + object.name + ")\n");
    }

    shader += shader_template.substr(material_computation_idx);
    return shader;
}