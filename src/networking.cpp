#include "networking.h"

// let's reasses this file for a bit
// we want a single function to send http request on both browser and desktop versions
// 

void downloadSucceeded(http_response *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  //emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(http_response *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  //emscripten_fetch_close(fetch); // Also free data on failure.
}


void RecieveFileNames(http_response *fetch)
{
    auto network_data = static_cast<NetworkData*>(fetch->userData);
    auto data_json = nlohmann::json::parse(std::string(fetch->data, fetch->numBytes));

    data_json.at("file_names").get_to(network_data->file_names);
    data_json.at("file_permissions").get_to(network_data->file_permissions);
}

void NetworkData::sendJson(std::string data_json, std::string adress, void (*onsuccess)(http_response *fetch), void (*onerror)(http_response *fetch))
{
    data_buffer = data_json;
    http_attribute attr;
    //emscripten_fetch_attr_init(&attr);
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

    request(&attr, adress.c_str());

}



void NetworkData::Register(std::string passwrd, std::string user_name, std::string email)
{
    password = passwrd;
    username = user_name;

    nlohmann::json data;
    data["password"] = passwrd;
    data["username"] = user_name;
    data["email"] = email;

    sendJson(data.dump(), "/api/signup", downloadSucceeded, downloadFailed);
}

void NetworkData::LogIn(std::string passwrd, std::string user_name)
{
    password = passwrd;
    username = user_name;
}


void NetworkData::GetFileFromCloud(std::string link, void (*onsuccess)(http_response *fetch), void* ptr)
{
    nlohmann::json data;
    printf("%s\n", link.c_str());
    int index = 8;
    while (index < link.size() && link[index] != '/') {
        index++;
    }
    int index2 = index + 1;
    while (index2 < link.size() && link[index2] != '/') {
        index2++;
    }
    printf("hm\n");
    if (index2 == index + 1 || index2 == link.size()) {
        printf("parseerror\n");
        return;
    }
    std::string user_name = link.substr(index + 1, index2 - index - 1);
    std::string file_name = link.substr(index2 + 1, link.size() - index2 - 1);
    current_file_name = file_name;
    current_file_owner = user_name;
    if (user_name == username) {
        data["password"] = password;
    }
    data_buffer = data.dump();


    http_attribute attr;
    //emscripten_fetch_attr_init(&attr);
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

    request(&attr, ("/api/users/" + user_name + "/" + file_name).c_str());


}

void NetworkData::GetFileFromCloud(std::string file_name, std::string user_name, void (*onsuccess)(http_response* fetch), void* ptr)
{
    current_file_name = file_name;
    current_file_owner = user_name;
    nlohmann::json data;
    if (user_name == username)
    {
        data["password"] = password;
    }
    data_buffer = data.dump();

    http_attribute attr;
    
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

    request(&attr, ("/api/users/"+user_name+"/"+file_name).c_str());

}


void NetworkData::UploadFile(std::string file)
{
    nlohmann::json data;

    data["password"] = password;
    data["username"] = current_file_owner;
    data["file_name"] = current_file_name;
    data["file"] = file;

    sendJson(data.dump(), "/api/upload", downloadSucceeded, downloadFailed);
}

void NetworkData::GetUserFileNames()
{
    nlohmann::json data;
    data["password"] = password;
    data["username"] = username;
    data_buffer = data.dump();
    http_attribute attr;
    //emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = this;
    attr.onsuccess = RecieveFileNames;
    //attr.onerror = downloadFailed;
    const char* headers[] = {
        "Content-Type", "application/json",
        nullptr // Null terminator for the header array
    };
    attr.requestHeaders = headers;
    attr.requestData = data_buffer.c_str();
    attr.requestDataSize = data_buffer.size();
    request(&attr, "/api/getuserfilenames");
}
#ifdef __EMSCRIPTEN__
// void NetworkData::GetUserFileNames()
// {
//     nlohmann::json data;
//     data["password"] = password;
//     data["username"] = username;
//     data_buffer = data.dump();
//     emscripten_fetch_attr_t attr;
//     emscripten_fetch_attr_init(&attr);
//     strcpy(attr.requestMethod, "POST");
//     attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
//     attr.userData = this;
//     attr.onsuccess = RecieveFileNames;
//     attr.onerror = downloadFailed;

//     const char* headers[] = {
//         "Content-Type", "application/json",
//         nullptr // Null terminator for the header array
//     };
    
//     attr.requestHeaders = headers;
//     attr.requestData = data_buffer.c_str();
//     attr.requestDataSize = data_buffer.size();

//     emscripten_fetch(&attr, "/api/getuserfilenames");
// }

EM_JS(void, push_link, (const char* file_name, const char* username), {
    var name = UTF8ToString(file_name);
    var user_name = UTF8ToString(username);
    var title = user_name + ": " + name;
    window.history.pushState({ file_name: name, username: user_name }, title, "/"+user_name+"/"+name);
    document.title = title;
});

void NetworkData::UpdateLink()
{
    push_link(current_file_name.c_str(), current_file_owner.c_str());
}
#endif

void NetworkData::SetFilePermission(std::string file_name, std::string permission)
{
    nlohmann::json data;
    data["password"] = password;
    data["username"] = username;
    data["file_name"] = file_name;
    data["permission"] = permission;

    sendJson(data.dump(), "/api/setfilepermission", downloadSucceeded, downloadFailed);
}


