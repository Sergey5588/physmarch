#include"scene.h"

void Scene::update_ln() {
	for(int i = 0; i < T__LENGTH; ++i) {
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
			return;
		}
	}
    objects.push_back(obj);
    labels.push_back(name);
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
        {"color", {material.color.r, material.color.g, material.color.b, material.color.w},},
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
    std::vector<float> vec;
    j.at("color").get_to(vec);
    material.color = glm::vec4(vec[0], vec[1], vec[2], vec[3]);

    j.at("roughness").get_to(material.roughness);
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
    if (file == "")
        return;
    nlohmann::json json = nlohmann::json::parse(file);
    
    json.at("objects").get_to(objects);
    json.at("materials").get_to(materials);
    json.at("object_names").get_to(labels);
    json.at("material_names").get_to(material_names);
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
        Scene* scene = static_cast<Scene*>(scene_ptr_void);
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

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void receive_link(void* scene_ptr_void, const char* data, int size) {
        Scene* scene = static_cast<Scene*>(scene_ptr_void);
        std::string link = std::string(data, size);
        scene->LoadFromLink2(link);
    }
}

EM_JS(char*, get_browser_url_js, (long scene_ptr), {
    // This code runs in the browser's JavaScript environment
    const url = window.location.href;
    const encoder = new TextEncoder();
    const data = encoder.encode(url);
    const size = data.length;
    console.log(data.length);
    // Allocate memory in WASM heap
    var buffer = _malloc(size);
    HEAPU8.set(data, buffer);

    // Call the C++ function with the file data
    Module.ccall(
        'receive_link',
        'null',
        ['number','number', 'number'],
        [scene_ptr, buffer, size]
    );

    // Clean up allocated memory
    _free(buffer);
});
#endif
void RecieveFile(http_response *fetch)
{
    auto scene = static_cast<Scene*>(fetch->userData);
    scene->LoadFromJson(std::string(fetch->data, fetch->numBytes));
}
#ifdef __EMSCRIPTEN__
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void loadFromArrow(void* scene_ptr, const char* file_name, int file_name_len, const char* username, int username_len)
    {
        Scene* scene = static_cast<Scene*>(scene_ptr);
        scene->LoadFromCloudFile(std::string(file_name, file_name_len), std::string(username, username_len));
    }
}

EM_JS(void, SetupListener, (long scene_ptr), {
    window.addEventListener('popstate', function(event) {
        if (event.state)
        {
            var username = event.state.username;
            var file_name = event.state.file_name; 
            Module.ccall(
                'loadFromArrow',
                'null',
                ['number', 'string', 'number', 'string', 'number'],
                [scene_ptr, file_name, file_name.length, username, username.length]
            );
            document.title = username + ": " + file_name;
        } else 
        {
            window.location.reload();
        }
    });
});


#endif
void Scene::SetupLoadListener()
{
    #ifdef __EMSCRIPTEN__
    SetupListener(reinterpret_cast<long>(this));
    #endif
}
void Scene::LoadFromLink()
{   
    #ifdef __EMSCRIPTEN__
    get_browser_url_js(reinterpret_cast<long>(this));
    #endif
}
void Scene::LoadFromLink2(std::string link)
{
    #ifdef __EMSCRIPTEN__
    network_data->GetFileFromCloud(link, RecieveFile, static_cast<void*>(this));
    #endif
}

void Scene::LoadFromCloudFile(std::string file_name, std::string user_name)
{
    #ifdef __EMSCRIPTEN__
    network_data->GetFileFromCloud(file_name, user_name, RecieveFile, static_cast<void*>(this));
    #endif
}

