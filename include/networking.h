#ifndef NETWORKING_H
#define NETWORKING_H
#include <string>
#include <vector>
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
    #ifdef __EMSCRIPTEN__
    void GetFileFromCloud(std::string link, void (*onsuccess)(emscripten_fetch_t *fetch), void* ptr);
    void GetFileFromCloud(std::string file_name, std::string user_name, void (*onsuccess)(emscripten_fetch_t *fetch), void* ptr);
    #endif
    void GetUserFileNames();
    
    std::string password;
    std::string username;
    std::string data_buffer;

    std::string current_file_name = "";

    std::vector<std::string> file_names;
    std::vector<std::string> file_permissions;

    std::string downloaded_file;
private:
    #ifdef __EMSCRIPTEN__
    void sendJson(std::string data_json, std::string adress, void (*onsuccess)(emscripten_fetch_t *fetch), void (*onerror)(emscripten_fetch_t *fetch));
    #else
    void sendJson(std::string data_json, std::string adress, void (*onsuccess)(), void (*onerror)());
    #endif
};
#endif