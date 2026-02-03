#ifndef NETWORKING_H
#define NETWORKING_H
#include <string>
#include <vector>
#include "http_request.h"
#include<json.hpp>
#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#include <emscripten/fetch.h>
#endif
class NetworkData
{
public:
    void Register(std::string password, std::string username, std::string email);
    void LogIn(std::string password, std::string username);
    void SetFilePermission(std::string file_name, std::string permission);
    void UploadFile(std::string file);
    
    void GetFileFromCloud(std::string link, void (*onsuccess)(http_response* fetch), void* ptr);
    void GetFileFromCloud(std::string file_name, std::string user_name, void (*onsuccess)(http_response *fetch), void* ptr);
    #ifdef __EMSCRIPTEN__
    void UpdateLink();
    #endif
    void GetUserFileNames();
    
    std::string password;
    std::string email;
    std::string username;
    std::string data_buffer;

    std::string current_file_name = "";
    std::string current_file_owner = "oh";

    std::vector<std::string> file_names;
    std::vector<std::string> file_permissions;

    std::string downloaded_file;
private:
    void sendJson(std::string data_json, std::string adress, void (*onsuccess)(http_response *fetch), void (*onerror)(http_response *fetch));
};
#endif