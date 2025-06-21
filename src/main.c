#include <stddef.h>
#include <stdio.h>

#include <curl/curl.h>
#include <curl/easy.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char * start; // Optional, NULL if error
    size_t len;
} str_view;


const static char * message = "{\"msg\": \"Hello World\"}";

str_view get_value(const char * input);
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

// pos[len] == last character in str
str_view get_value(const char * input) {
    char match[] = "\"type\": \"output_text\",";
    char * maybe_content = strstr(input, match);

    if (!maybe_content)
        return (str_view) {NULL, 0};

    char * pos = maybe_content + sizeof(match);

    while (isspace(*pos)) pos++;

    char match2[] = "\"text\": \"";
    if (strncmp(pos, match2, sizeof(match2) - 1)) // It found some other "text": field
        return (str_view) {NULL, 0};
    pos += sizeof(match2) - 1; // Jump to just after first quote

    size_t len = 0;
    while (pos[len] != '"') {
        if (pos[len] == '\\')
            len++;
        len++;
    }

    return (str_view) {pos, len - 1};
}
