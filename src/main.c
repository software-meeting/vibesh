#include <stddef.h>
#include <stdio.h>

#include <curl/curl.h>
#include <curl/easy.h>
#include <stdlib.h>
#include <string.h>

const static char * message = "{\"msg\": \"Hello World\"}";

size_t read_callback(char * buffer, size_t size, size_t nitems, void * userdata);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int main(void) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURL * handle = curl_easy_init();
    if (handle == nullptr) {
        perror("failed to open handle");
    }

    struct curl_slist * headers = { };
    curl_slist_append(headers, "Content-Type: application/json");

    CURLcode res = { };
    curl_easy_setopt(handle, CURLOPT_URL, "https://echo.free.beeceptor.com");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, message);

    res = curl_easy_perform(handle);

    if (res != CURLE_OK) {
        perror("request failed");
    }

    curl_easy_cleanup(handle);

    curl_global_cleanup();
    return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    char * msg = calloc(nmemb + 1, size);
    memcpy((void *)msg, (void *)ptr, nmemb);
    printf("%s\n", msg);
    free(msg);
    return size * nmemb;
}
