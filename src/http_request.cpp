#include "http_request.h"

http_attribute::http_attribute()
{
    // WARNING this is because of a weird compaitability layer 
    // where in emscripten's container the default is all zeros
    // so I had to do it in there too
    memset(this, 0, sizeof(*this));
    // need not to use C++ things here I guess
}

void emscripten_fetch_attr_init(http_attribute* http_attr) {
    *http_attr = {};
}
#ifdef __EMSCRIPTEN__
void user_data_container::convert_response(emscripten_fetch_t *fetch, http_response& response)
{
    response.data = fetch->data;
    response.dataOffset = fetch->dataOffset;
    response.id = fetch->id;
    response.numBytes = fetch->numBytes;
    response.userData = userData;
    response.readyState = fetch->readyState;
    //response.responseUrl = fetch->responseUrl;
    response.status = fetch->status;
    strcpy(response.statusText, fetch->statusText);
    response.totalBytes = fetch->totalBytes;
    response.url = fetch->url;
}
#endif

#ifdef __EMSCRIPTEN__

void onsuccess_wrapper(emscripten_fetch_t *fetch)
{
    user_data_container *data = static_cast<user_data_container*>(fetch->userData);
    if (data->onsuccess)
    {
        http_response response;
        data->convert_response(fetch, response);
        data->onsuccess(&response);
    }
    delete data;
    emscripten_fetch_close(fetch);
}
void onerror_wrapper(emscripten_fetch_t *fetch)
{
    user_data_container *data = static_cast<user_data_container*>(fetch->userData);
    if (data->onerror)
    {
        http_response response;
        data->convert_response(fetch, response);
        data->onerror(&response);
    }
    delete data;
    emscripten_fetch_close(fetch);
}
#endif


void request(http_attribute* fetch_attr, const char* url)
{
    #ifdef __EMSCRIPTEN__
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, fetch_attr->requestMethod);
    attr.destinationPath = fetch_attr->destinationPath;
    attr.attributes = fetch_attr->attributes;
    auto container = new user_data_container{fetch_attr->userData, fetch_attr->onsuccess, fetch_attr->onerror};
    attr.userData = container;
    attr.onsuccess = onsuccess_wrapper;
    attr.onerror = onerror_wrapper;
    // todo: implement like the onsuccess_wrapper
    // attr.onprogress = onprogress;
    // attr.onreadystatechange = onreadystatechange;
    attr.requestHeaders = fetch_attr->requestHeaders;
    attr.requestData = fetch_attr->requestData;
    attr.requestDataSize = fetch_attr->requestDataSize;
    attr.password = fetch_attr->password;
    attr.userName = fetch_attr->userName;
    attr.timeoutMSecs = fetch_attr->timeoutMSecs;
    attr.overriddenMimeType = fetch_attr->overriddenMimeType;
    attr.withCredentials = fetch_attr->withCredentials;
    emscripten_fetch(&attr, url);
    #else
    // desktop version
    #endif
}
