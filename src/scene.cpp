#include"scene.h"

void Scene::update_ln() {
	for(int i = 0; i < T__LENGTH; ++i) {
        printf("%d\n",lengths[i]);
		lengths[i] = 0;
	}

	for(auto i:objects) {
		lengths[i.type]++;
	}
}

void Scene::add_object(Object obj, std::string name) {
	for(int i = 0; i < objects.size(); ++i) {
		if(objects[i].type >= obj.type) {
			objects.insert(objects.begin()+i, obj);
            //lengths[obj.type]++;
			labels.insert(labels.begin()+i,name);
			break;
		}
	}
}

void Scene::delete_object(Object* obj, std::string* label) {
    auto object_iterator = objects.begin() + (obj - &objects[0]);
    auto label_iterator = labels.begin() + (label - &labels[0]);
    objects.erase(object_iterator);
    labels.erase(label_iterator);
    update_ln();
}
// struct alignas(16) Object {
// 	alignas(4) int type;
//     alignas(4) int operation;
//     alignas(16) glm::vec3 pos;  // vec3 equivalent
//     alignas(16) glm::vec4 args; // vec4 equivalent  
//     alignas(4) int material_id;
//     alignas(4) int _padding;   // Explicit padding
// 	Object(int type, int operation, glm::vec3 pos, glm::vec4 args, int material_id) : type(type), operation(operation), pos(pos), args(args), material_id(material_id) {}

// };


void to_json(nlohmann::json& j, const Object& object) {
    j = {
        {"type", object.type},
        {"operation", object.operation},
        {"pos", {object.pos.x, object.pos.y, object.pos.z}},
        {"args", {object.args.x, object.args.y, object.args.z, object.args.w}},
        {"material_id", object.material_id},
    };
}

void to_json(nlohmann::json& j, const Material& material) {
    j = {
        {"color", {material.color.r, material.color.g, material.color.r, material.color.w},},
        {"roughness", material.roughness}
    };
}

void from_json(const nlohmann::json& j, Object& object)
{
    j.at("type").get_to(object.type);

    j.at("operation").get_to(object.operation);

    std::vector<float> vec;
    j.at("pos").get_to(vec);
    object.pos = glm::vec3(vec[0], vec[1], vec[2]);

    j.at("args").get_to(vec);
    object.args = glm::vec4(vec[0], vec[1], vec[2], vec[4]);

    j.at("material_id").get_to(object.material_id);
}

void from_json(const nlohmann::json& j, Material& material)
{
    printf("from_json_mat\n");
    std::vector<float> vec;
    j.at("color").get_to(vec);
    printf("color\n");
    material.color = glm::vec4(vec[0], vec[1], vec[2], vec[3]);

    j.at("roughness").get_to(material.roughness);
    printf("rough\n");
}



std::string Scene::ConvertToJson()
{
    nlohmann::json file;
    file["objects"] = objects;
    file["materials"] = materials;
    file["object_names"] = labels;
    file["material_names"] = material_names;
    return file.dump();
}

void Scene::LoadFromJson(std::string file)
{
    printf("%s\n", file.c_str());
    printf("function_called\n");
    
    nlohmann::json json = nlohmann::json::parse(file);
    printf("parsed?\n");
    json.at("objects").get_to(objects);
    printf("objects?\n");
    json.at("materials").get_to(materials);
    printf("materials?\n");
    json.at("object_names").get_to(labels);
    printf("labels\n");
    json.at("material_names").get_to(material_names);
    printf("done?\n");
    update_ln();
}


#ifdef __EMSCRIPTEN__

EM_JS(void, downloadFile, (const char* filename, const char* mime_type, const char* buffer, size_t buffer_size), {
// Create a JavaScript Blob from the C++ buffer.
// Module.HEAPU8 provides access to the WebAssembly memory.
var blob = new Blob([new Uint8Array(Module.HEAPU8.buffer, buffer, buffer_size)], {
    type: UTF8ToString(mime_type)
});

// Create an anchor element for the download.
var a = document.createElement('a');
a.download = UTF8ToString(filename); // Set the filename.
a.href = URL.createObjectURL(blob); // Set the Blob as the download source.
a.click(); // Programmatically click the link to trigger the download.
URL.revokeObjectURL(a.href); // Clean up the URL object.
});
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void receive_file_data(void* scene_ptr_void, const char* filename, const char* data, int size) {
        // std::cout << "Received file: " << filename << std::endl;
        // std::cout << "Size: " << size << " bytes" << std::endl;

        // // Example: Print the first 20 bytes of the file
        // std::cout << "First 20 bytes: ";
        // for (int i = 0; i < std::min(size, 20); ++i) {
        //     std::cout << data[i];
        // }
        // std::cout << std::endl;
        // LoadFromJson(std::string(data));
        printf("almost there\n");
        Scene* scene = static_cast<Scene*>(scene_ptr_void);
        printf("ded\n");
        scene->LoadFromJson(std::string(data, size));
    }
}
EM_JS(void, open_file_dialog, (long scene_ptr), {
    // Create a hidden file input element
    var file_selector = document.createElement('input');
    file_selector.setAttribute('type', 'file');

    // This function runs when a user selects a file
    file_selector.onchange = function(e) {
        var file = e.target.files[0];
        if (!file) {
            return;
        }

        var reader = new FileReader();
        reader.onload = function(e) {
            var contents = e.target.result;
            var data = new Uint8Array(contents);
            var size = data.length;
            var filename = file.name;

            // Allocate memory in WASM heap
            var buffer = _malloc(size);
            HEAPU8.set(data, buffer);

            // Call the C++ function with the file data
            Module.ccall(
                'receive_file_data',
                'null',
                ['number','string', 'number', 'number'],
                [scene_ptr, filename, buffer, size]
            );

            // Clean up allocated memory
            _free(buffer);
        };
        reader.readAsArrayBuffer(file);
    };

    // Trigger the file selection dialog
    file_selector.click();
});

#endif
void Scene::OfferDownload()
{
    auto file = ConvertToJson();
    #ifdef __EMSCRIPTEN__
    downloadFile("physmarch_file.json", "text/plain", file.c_str(), file.length());
    #endif
}
void Scene::LoadFromLocalFile()
{
    #ifdef __EMSCRIPTEN__
    open_file_dialog(reinterpret_cast<long>(this));
    #endif
}

#ifdef __EMSCRIPTEN__
void RecieveFile(struct emscripten_fetch_t *fetch)
{
    auto scene = static_cast<Scene*>(fetch->userData);
    scene->LoadFromJson(std::string(fetch->data, fetch->numBytes));
}
#endif

void Scene::LoadFromLink(std::string link)
{
    #ifdef __EMSCRIPTEN__
    network_data->GetFileFromCloud(link,  RecieveFile, static_cast<void*>(this));
    LoadFromJson(network_data->downloaded_file);
    #endif
}

void Scene::LoadFromCloudFile(std::string file_name, std::string user_name)
{
    #ifdef __EMSCRIPTEN__
    network_data->GetFileFromCloud(file_name, user_name, RecieveFile, static_cast<void*>(this));
    #endif
}

