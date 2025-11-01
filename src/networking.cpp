#include "networking.h"
#ifdef __EMSCRIPTEN__
void downloadSucceeded(emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

void RecieveFileNames(emscripten_fetch_t *fetch)
{
    auto network_data = static_cast<NetworkData*>(fetch->userData);
    auto data_json = nlohmann::json::parse(std::string(fetch->data, fetch->numBytes));

    data_json.at("file_names").get_to(network_data->file_names);
    data_json.at("file_permissions").get_to(network_data->file_permissions);
}

void NetworkData::sendJson(std::string data_json, std::string adress, void (*onsuccess)(emscripten_fetch_t *fetch), void (*onerror)(emscripten_fetch_t *fetch))
{
    data_buffer = data_json;
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    // attr.userData = this;
    attr.onsuccess = onsuccess;
    attr.onerror = onerror;

    const char* headers[] = {
        "Content-Type", "application/json",
        nullptr // Null terminator for the header array
    };
    
    attr.requestHeaders = headers;
    attr.requestData = data_buffer.c_str();
    attr.requestDataSize = data_buffer.size();

    emscripten_fetch(&attr, adress.c_str());

}
#else
void downloadSucceeded() {}
void downloadFailed() {}
void NetworkData::sendJson(std::string data_json, std::string adress, void (*onsuccess)(), void (*onerror)()) {}

#endif



void NetworkData::Register(std::string passwrd, std::string user_name, std::string email)
{
    password = passwrd;
    username = user_name;

    nlohmann::json data;
    data["password"] = passwrd;
    data["username"] = user_name;
    data["email"] = email;

    sendJson(data.dump(), "http://127.0.0.1:5000/signup", downloadSucceeded, downloadFailed);
}

void NetworkData::LogIn(std::string passwrd, std::string user_name)
{
    password = passwrd;
    username = user_name;
}

#ifdef __EMSCRIPTEN__
void NetworkData::GetFileFromCloud(std::string link, void (*onsuccess)(emscripten_fetch_t *fetch), void* ptr)
{
    nlohmann::json data;

    int index = 0;
    while (index < link.size() && link[index] != '/') {
        index++;
    }
    int index2 = index + 1;
    while (index2 < link.size() && link[index2] != '/') {
        index2++;
    }
    std::string user_name = link.substr(index + 1, index2 - index - 2);
    if (user_name == username) {
        data["password"] = password;
    }
    data_buffer = data.dump();

    #ifdef __EMSCRIPTEN__
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = this;
    attr.onsuccess = onsuccess;
    attr.onerror = downloadFailed;

    const char* headers[] = {
        "Content-Type", "application/json",
        nullptr // Null terminator for the header array
    };

    attr.requestHeaders = headers;
    attr.requestData = data_buffer.c_str();
    attr.requestDataSize = data_buffer.size();

    emscripten_fetch(&attr, link.c_str());
    #endif

}

void NetworkData::GetFileFromCloud(std::string file_name, std::string user_name, void (*onsuccess)(emscripten_fetch_t *fetch), void* ptr)
{
    nlohmann::json data;
    if (user_name == username)
    {
        data["password"] = password;
    }
    #ifdef __EMSCRIPTEN__
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = ptr;
    attr.onsuccess = onsuccess;
    attr.onerror = downloadFailed;

    const char* headers[] = {
        "Content-Type", "application/json",
        nullptr // Null terminator for the header array
    };

    attr.requestHeaders = headers;
    attr.requestData = data_buffer.c_str();
    attr.requestDataSize = data_buffer.size();

    emscripten_fetch(&attr, ("http://127.0.0.1:5000/users/"+user_name+"/"+file_name).c_str());
    #endif
}
#endif

void NetworkData::UploadFile(std::string file)
{
    nlohmann::json data;

    data["password"] = password;
    data["username"] = username;
    data["file_name"] = current_file_name;
    data["file"] = file;

    sendJson(data.dump(), "http://127.0.0.1:5000/upload", downloadSucceeded, downloadFailed);
}
#ifdef __EMSCRIPTEN__
void NetworkData::GetUserFileNames()
{
    nlohmann::json data;
    data["password"] = password;
    data["username"] = username;
    data_buffer = data.dump();
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = this;
    attr.onsuccess = RecieveFileNames;
    attr.onerror = downloadFailed;

    const char* headers[] = {
        "Content-Type", "application/json",
        nullptr // Null terminator for the header array
    };
    
    attr.requestHeaders = headers;
    attr.requestData = data_buffer.c_str();
    attr.requestDataSize = data_buffer.size();

    emscripten_fetch(&attr, "http://127.0.0.1:5000/getuserfilenames");
}
#endif

void NetworkData::SetFilePermission(std::string file_name, std::string permission)
{
    nlohmann::json data;
    data["password"] = password;
    data["username"] = username;
    data["file_name"] = file_name;
    data["permission"] = permission;

    sendJson(data.dump(), "http://127.0.0.1:5000/setfilepermission", downloadSucceeded, downloadFailed);
}
