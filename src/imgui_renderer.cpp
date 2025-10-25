#include"imgui_renderer.h"

ImguiRenderer::ImguiRenderer(Scene& scene, glm::vec3& Position,int& ITERATIONS, int& SHADOW_RAYS)
 : scene(scene), Position(Position), ITERATIONS(ITERATIONS), SHADOW_RAYS(SHADOW_RAYS) {}

void ImguiRenderer::imgui_start_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::ShowDemoWindow();
}

void ImguiRenderer::render_top_bar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                printf("good\n");
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                printf("nope\n");
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                printf("saved\n");
            }
            if (ImGui::MenuItem("Getlink")) {
                printf("link\n");
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Account"))
        {
            if (ImGui::MenuItem("Login")) {}
            ImGui::EndMenu();
        }

        // ImGui::SameLine(ImGui::GetWindowWidth() - 100); // Position to the right
        // ImGui::Text("Status: OK");

        ImGui::EndMainMenuBar();
    }

}

void ImguiRenderer::imgui_render_control_panel()
{
    ImGui::Begin("Control panel");
    ImGui::SliderInt("Iterations", &ITERATIONS, 0, 1000);
    ImGui::SliderInt("Shadow rays", &SHADOW_RAYS, 0, 100);
    ImGui::DragFloat3("Camera position", glm::value_ptr(Position), 0, 0.01);
    ImGui::End();
}

void ImguiRenderer::imgui_render_scene_editor()
{
    ImGui::Begin("Scene editor");
    
    ImGuiTreeNodeFlags Header_flags =  ImGuiTreeNodeFlags_Framed;
    if (ImGui::TreeNodeEx("Objects", Header_flags)) {

        ImGui::Combo("Object type", &selected_obj_type, ObjAsStr,IM_ARRAYSIZE(ObjAsStr));
        ImGui::InputText("Object name", &new_object_name);
        if(ImGui::Button("Add object")) {
            if(std::find(scene.labels.begin(), scene.labels.end(), new_object_name)==scene.labels.end()) {
                scene.add_object(
                    Object{selected_obj_type, O_BASE, glm::vec3(0,3,0), glm::vec4(1), 0},
                    new_object_name
                );

            } else {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopup("Error")) {
                    ImGui::Text("Error: name already exists");
                    if(ImGui::Button("OK")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            scene.update_ln();
        }

        for(int i = 0; i < scene.objects.size(); i++) {
            ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
            if(object_selected == i) tree_flags |= ImGuiTreeNodeFlags_Selected;
            bool is_open = ImGui::TreeNodeEx(scene.labels[i].c_str(), tree_flags);

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
                object_selected = i;
            }

            if (is_open) {
                ImGui::BulletText("Props:");
                ImGui::DragFloat3("Position", glm::value_ptr(scene.objects[i].pos), 0.1f);
                ImGui::DragFloat4("Arguments", glm::value_ptr(scene.objects[i].args), 0.1f);

                int material_index = scene.objects[i].material_id;
                if (ImGui::BeginCombo("Material", scene.material_names[material_index].c_str()))
                {
                    for (int j = 0; j < scene.materials.size(); ++j)
                    {
                        const bool is_selected = (material_index == j);
                        if (ImGui::Selectable(scene.material_names[j].c_str(), is_selected))
                        {
                            scene.objects[i].material_id = j;
                        }

                        // Set the initial focus when opening the combo (scrolling to the selected item)
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Materials", Header_flags))
    {
        ImGui::InputText("material name", &new_material_name);
        if(ImGui::Button("Add material")) {
            if(std::find(scene.material_names.begin(), scene.material_names.end(), new_material_name)==scene.material_names.end()) {
                scene.materials.push_back(
                    Material{glm::vec4(1, 0, 1, 1), 1.0}
                );
                scene.material_names.push_back(new_material_name);

            } else {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopup("Error")) {
                    ImGui::Text("Error: name already exists");
                    if(ImGui::Button("OK")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            
        }

        for (int i = 0; i < scene.materials.size(); i++)
        {
            ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
            if(material_selected == i) tree_flags |= ImGuiTreeNodeFlags_Selected;
            bool is_open = ImGui::TreeNodeEx(scene.material_names[i].c_str(), tree_flags);

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
                material_selected = i;
            }
            if (is_open)
            {
                ImGui::ColorEdit3("Color", glm::value_ptr(scene.materials[i].color));
                ImGui::DragFloat("Roughness", &scene.materials[i].roughness, 0.01, 100);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    
    

    ImGui::End();
    
}

