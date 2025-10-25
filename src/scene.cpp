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
		if(objects[i].type == obj.type) {
			objects.insert(objects.begin()+i, obj);
            //lengths[obj.type]++;
			labels.insert(labels.begin()+i,name);
			break;
		}
	}
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
        {"operation", object.operation},
        {"pos", {object.pos.x, object.pos.y, object.pos.z}},
        {"args", {object.args.x, object.args.y, object.args.z, object.args.w}},
        {"material_id", object.material_id},
    };
}

void to_json(nlohmann::json& j, const Material& material) {
    j = {
        {"color", {material.color.r, material.color.g, material.color.r, material.color.w},},
        {"roughness", {material.roughness}}
    };
}

void from_json(const nlohmann::json& j, Object& object)
{
    j.at("operation").get_to(object.operation);

    std::vector<float> vec;
    j.at("pos").get_to(vec);
    object.pos = glm::vec3(vec[0], vec[1], vec[2]);

    j.at("args").get_to(vec);
    object.args = glm::vec4(vec[0], vec[1], vec[2], vec[4]);

    j.at("material_id").get_to(object.material_id);
}

void from_json(nlohmann::json& j, Material& material)
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
    nlohmann::json json = nlohmann::json::parse(file);
    objects[0] = json["objects"][0].get<Object>();
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
#endif
void Scene::OfferDownload()
{
    auto file = ConvertToJson();
    #ifdef __EMSCRIPTEN__
    downloadFile("physmarch_file.json", "text/plain", file.c_str(), file.length());
    #endif
}
