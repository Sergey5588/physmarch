#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#include <emscripten/fetch.h>
#else
//just so that the desktop version compiles
#define EMSCRIPTEN_FETCH_LOAD_TO_MEMORY  1
#define EMSCRIPTEN_FETCH_STREAM_DATA 2
#define EMSCRIPTEN_FETCH_PERSIST_FILE 4
#define EMSCRIPTEN_FETCH_APPEND 8
#define EMSCRIPTEN_FETCH_REPLACE 16
#define EMSCRIPTEN_FETCH_NO_DOWNLOAD 32
#define EMSCRIPTEN_FETCH_SYNCHRONOUS 64
#define EMSCRIPTEN_FETCH_WAITABLE 128
#endif
#include <cstdint>
#include <cstring>
struct http_response;
//don't use c++ things there to avoid corruption
struct http_attribute
{
    http_attribute();
    // 'POST', 'GET', etc.
    char requestMethod[32];

    // Custom data that can be tagged along the process.
    void *userData;

    void (*onsuccess)(http_response *fetch);
    void (*onerror)(http_response *fetch);
    void (*onprogress)(http_response *fetch);
    void (*onreadystatechange)(http_response *fetch);

    // EMSCRIPTEN_FETCH_* attributes
    uint32_t attributes;

    // Specifies the amount of time the request can take before failing due to a
    // timeout.
    uint32_t timeoutMSecs;

    // Indicates whether cross-site access control requests should be made using
    // credentials.
    bool withCredentials;

    // Specifies the destination path in IndexedDB where to store the downloaded
    // content body. If this is empty, the transfer is not stored to IndexedDB at
    // all.  Note that this struct does not contain space to hold this string, it
    // only carries a pointer.
    // Calling emscripten_fetch() will make an internal copy of this string.
    const char *destinationPath;

    // Specifies the authentication username to use for the request, if necessary.
    // Note that this struct does not contain space to hold this string, it only
    // carries a pointer.
    // Calling emscripten_fetch() will make an internal copy of this string.
    const char *userName;

    // Specifies the authentication username to use for the request, if necessary.
    // Note that this struct does not contain space to hold this string, it only
    // carries a pointer.
    // Calling emscripten_fetch() will make an internal copy of this string.
    const char *password;

    // Points to an array of strings to pass custom headers to the request. This
    // array takes the form
    // {"key1", "value1", "key2", "value2", "key3", "value3", ..., 0 }; Note
    // especially that the array needs to be terminated with a null pointer.
    const char * const *requestHeaders;

    // Pass a custom MIME type here to force the browser to treat the received
    // data with the given type.
    const char *overriddenMimeType;

    // If non-zero, specifies a pointer to the data that is to be passed as the
    // body (payload) of the request that is being performed. Leave as zero if no
    // request body needs to be sent.  The memory pointed to by this field is
    // provided by the user, and needs to be valid throughout the duration of the
    // fetch operation. If passing a non-zero pointer into this field, make sure
    // to implement *both* the onsuccess and onerror handlers to be notified when
    // the fetch finishes to know when this memory block can be freed. Do not pass
    // a pointer to memory on the stack or other temporary area here.
    const char *requestData;

    // Specifies the length of the buffer pointed by 'requestData'. Leave as 0 if
    // no request body needs to be sent.
    size_t requestDataSize;

};
void emscripten_fetch_attr_init(http_attribute* http_attr);

struct http_response
{
    // Unique identifier for this fetch in progress.
    uint32_t id;

    // Custom data that can be tagged along the process.
    void *userData;

    // The remote URL set in the original request.
    const char *url;

    // In onsuccess() handler:
    //   - If the EMSCRIPTEN_FETCH_LOAD_TO_MEMORY attribute was specified for the
    //     transfer, this points to the body of the downloaded data. Otherwise
    //     this will be null.
    // In onprogress() handler:
    //   - If the EMSCRIPTEN_FETCH_STREAM_DATA attribute was specified for the
    //     transfer, this points to a partial chunk of bytes related to the
    //     transfer. Otherwise this will be null.
    // The data buffer provided here has identical lifetime with the
    // emscripten_fetch_t object itself, and is freed by calling
    // emscripten_fetch_close() on the emscripten_fetch_t pointer.
    const char *data;

    // Specifies the length of the above data block in bytes. When the download
    // finishes, this field will be valid even if EMSCRIPTEN_FETCH_LOAD_TO_MEMORY
    // was not specified.
    uint64_t numBytes;

    // If EMSCRIPTEN_FETCH_STREAM_DATA is being performed, this indicates the byte
    // offset from the start of the stream that the data block specifies. (for
    // onprogress() streaming XHR transfer, the number of bytes downloaded so far
    // before this chunk)
    uint64_t dataOffset;

    // Specifies the total number of bytes that the response body will be.
    // Note: This field may be zero, if the server does not report the
    // Content-Length field.
    uint64_t totalBytes;

    // Specifies the readyState of the XHR request:
    // 0: UNSENT: request not sent yet
    // 1: OPENED: emscripten_fetch has been called.
    // 2: HEADERS_RECEIVED: emscripten_fetch has been called, and headers and
    //    status are available.
    // 3: LOADING: download in progress.
    // 4: DONE: download finished.
    // See https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
    unsigned short readyState;

    // Specifies the status code of the response.
    unsigned short status;

    // Specifies a human-readable form of the status code.
    char statusText[64];

    // The response URL set by the fetch. It will be null until HEADERS_RECEIVED
    // readyState in async, or until completion in sync.
    //const char *responseUrl;
};

struct user_data_container
{
    void *userData;
    void (*onsuccess)(http_response *fetch);
    void (*onerror)(http_response *fetch);

    #ifdef __EMSCRIPTEN__
    void convert_response(emscripten_fetch_t *fetch, http_response& response);
    #endif
};

#ifdef __EMSCRIPTEN__
void onsuccess_wrapper(emscripten_fetch_t *fetch);
void onerror_wrapper(emscripten_fetch_t *fetch);
#endif

void request(http_attribute* fetch_attr, const char* url);
#endif