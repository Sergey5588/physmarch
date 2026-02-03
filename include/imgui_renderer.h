#ifndef IMGUI_RENDERER_H
#define IMGUI_RENDERER_H
#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include<imgui/imgui_stdlib.h>
#include<imgui/imgui_internal.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<algorithm>
#include<vector>

#include"object.h"
#include"scene.h"
#include"networking.h"

// struct ui_translation {
//     std::string File;
//     std::string Account;
//     std::string Language;
//     std::string Scene_editor;
//     std::string Control_panel;
//     std::string Objects;
//     std::string Materials;
//     std::string Camera;
//     std::string Iterations;
//     std::string Shadow_rays;
//     std::string Camera_position;
// };

class ImguiRenderer
{
public:
    ImguiRenderer(Scene& scene, NetworkData& network_data, glm::vec3& Position,int& ITERATIONS, int& SHADOW_RAYS);
    void imgui_start_frame();
    void render_top_bar();
    void imgui_render_control_panel();
    void imgui_render_scene_editor();
private:
    int &ITERATIONS;
    int& SHADOW_RAYS;
    glm::vec3& Position;
    Scene& scene;
    NetworkData& network_data;

    const char* ObjAsStr[6] = {
		"T_SPHERE",
		"T_BOX",
		"T_PRISM",
		"T_TORUS",
		"T_PLANE",
		"T_BULB",
	};
    int object_selected = -1;
    int material_selected = -1;

    int selected_obj_type = 0;
    std::string new_object_name = "null";
    std::string new_material_name = "null";

};
#endif
