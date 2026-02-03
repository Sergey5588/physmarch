#include"imgui_renderer.h"

ImguiRenderer::ImguiRenderer(Scene& scene, NetworkData& network_data, glm::vec3& Position,int& ITERATIONS, int& SHADOW_RAYS)
 : scene(scene), network_data(network_data), Position(Position), ITERATIONS(ITERATIONS), SHADOW_RAYS(SHADOW_RAYS) {}

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
    bool sign_up_opened = false;
    bool file_list_opened = false;
    bool enter_file_name_opened = false;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                printf("good\n");
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                scene.LoadFromLocalFile();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                scene.OfferDownload();
            }
            if (ImGui::MenuItem("Rename")) {
                enter_file_name_opened = true;
            }
            if (ImGui::MenuItem("Getlink")) {
                printf("link\n");
            }
            if (ImGui::MenuItem("Upload")) {
                if (network_data.current_file_name != "") {
                    network_data.UploadFile(scene.ConvertToJson());
                }
                else {
                    enter_file_name_opened = true;
                }
            }
            if (ImGui::BeginMenu("Permission"))
            {
                if (ImGui::MenuItem("Set private")) {
                    network_data.SetFilePermission(network_data.current_file_name, "private");
                }
                if (ImGui::MenuItem("Set read only")) {
                    network_data.SetFilePermission(network_data.current_file_name, "read only");
                }
                if (ImGui::MenuItem("Set public")) {
                    network_data.SetFilePermission(network_data.current_file_name, "public");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Account"))
        {
            if (ImGui::MenuItem("Sign up")) {
                sign_up_opened = true;
            }
            if (ImGui::MenuItem("My files"))
            {
                network_data.GetUserFileNames();
                file_list_opened = true;
            }
            
            // bool is_pop_up_open = true;
            
            ImGui::EndMenu();
        }
        

        // ImGui::SameLine(ImGui::GetWindowWidth() - 100); // Position to the right
        // ImGui::Text("Status: OK");

        ImGui::EndMainMenuBar();
    }
    if (sign_up_opened) {
        ImGui::OpenPopup("Sign up");
    }
    if (file_list_opened) {
        ImGui::OpenPopup("My files");
    }
    if (enter_file_name_opened) {
        ImGui::OpenPopup("Enter file name");
    }
    if (ImGui::BeginPopupModal("Sign up"))
    {
        ImGui::InputText("User name", &network_data.username);
        ImGui::InputText("Email", &network_data.email);
        ImGui::InputText("password", &network_data.password);
        if (ImGui::Button("Sign up"))
        {
            network_data.Register(network_data.password, network_data.username, network_data.email);
        }
        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("My files"))
    {
        for (int i = 0; i < network_data.file_names.size(); i++)
        {
            ImGui::PushID(network_data.file_names[i].c_str());
            auto inner_spacing_x = ImGui::GetStyle().ItemInnerSpacing.x;
            auto window_padding_x = ImGui::GetStyle().WindowPadding.x;
            bool is_pressed = ImGui::ButtonEx("", ImVec2(ImGui::GetWindowWidth() - 2 * window_padding_x, ImGui::GetFrameHeight()));
            ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + inner_spacing_x);
            ImGui::Text("%s", network_data.file_names[i].c_str());
            std::string permission_text = network_data.file_permissions[i];
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
            ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(permission_text.c_str()).x - ImGui::GetStyle().WindowPadding.x - inner_spacing_x);
            ImGui::Text("%s", permission_text.c_str());
            ImGui::PopStyleColor();

            ImGui::PopID();

            if (is_pressed)
            {
                scene.LoadFromCloudFile(network_data.file_names[i], network_data.username);
                network_data.current_file_name = network_data.file_names[i];
                network_data.current_file_owner = network_data.username;
                #ifdef __EMSCRIPTEN__
                network_data.UpdateLink();
                #endif
            }

        }
        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Enter file name")) {
        ImGui::InputText("File name", &network_data.current_file_name);
        if(ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
    Header_flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
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
            }
            scene.update_ln();
        }
        if (ImGui::BeginPopup("Error")) {
            ImGui::Text("Error: name already exists");
            if(ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        for(int i = 0; i < scene.objects.size(); i++) {
            ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_AllowItemOverlap;
            if(object_selected == i) tree_flags |= ImGuiTreeNodeFlags_Selected;

            bool is_open = ImGui::TreeNodeEx(scene.labels[i].c_str(), tree_flags);
            

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
                object_selected = i;
            }

            if (is_open) {
                if (ImGui::BeginPopupContextItem("ObjectNodeContextMenu"))
                {
                    // if (ImGui::MenuItem("Duplicate")) { scene.add_object(scene.objects[i]) }
                    if (ImGui::MenuItem("Delete")) {
                        scene.delete_object(&scene.objects[i], &scene.labels[i]);
                    }
                    ImGui::EndPopup();
                }
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
             
            
        }
        if (ImGui::BeginPopup("Error")) {
                ImGui::Text("Error: name already exists");
                if(ImGui::Button("OK")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        for (int i = 0; i < scene.materials.size(); i++)
        {
            ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_AllowItemOverlap;
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

