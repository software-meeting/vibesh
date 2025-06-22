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


const static char * req_body_1 = "{\n"
    "\"model\": \"gpt-4o-mini\",\n"
    "\"input\": \"";

const static char * req_body_2 = "\",\n"
    "\"temperature\": 2.0"
"}";

static char msgbuf[4096] = {0};


str_view get_value(const char * input);
size_t read_callback(char * buffer, size_t size, size_t nitems, void * userdata);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
char * get_api_key_header();

int main(void) {
    curl_global_init(CURL_GLOBAL_ALL);


    // Make sure to have api_key.txt in the root directory
    char * api_key_header = get_api_key_header();

    CURL * handle = curl_easy_init();
    if (handle == nullptr) {
        perror("failed to open handle");
    }

    struct curl_slist * headers = { };
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, api_key_header);

    CURLcode res = { };
    curl_easy_setopt(handle, CURLOPT_URL, "https://api.openai.com/v1/responses");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        
    char input[4096] = {0};

    while (1) {
        fgets(input, 4096, stdin);
        input[strlen(input) - 1] = '\0';

        snprintf(msgbuf, 4096, "%s%s%s", req_body_1, input, req_body_2);
    
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, msgbuf);
        res = curl_easy_perform(handle);

        if (res != CURLE_OK) {
            perror("request failed");
            continue;
        }

        str_view resp_str = get_value(msgbuf);

        if (!resp_str.start) {
            perror("empty / invalid response");
            continue;
        }

        resp_str.start[resp_str.len + 1] = '\0';

        printf("%s", resp_str.start);
    }

    curl_easy_cleanup(handle);

    curl_global_cleanup();

    return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    char * msg = calloc(nmemb + 1, size);
    memcpy((void *)msg, (void *)ptr, nmemb);
    snprintf(msgbuf, 4096, "%s", msg);
    free(msg);
    return size * nmemb;
}

// pos[len] == last character in str
str_view get_value(const char * input) {
    char match[] = "\"text\": \"";
    char * maybe_content = strstr(input, match);

    if (!maybe_content)
        return (str_view) {NULL, 0};
    char * pos = maybe_content + sizeof(match) - 1;

    size_t len = 0;
    while (pos[len] != '"') {
        if (pos[len] == '\\')
            len++;
        len++;
    }

    return (str_view) {pos, len - 1};
}

char * get_api_key_header() {
    FILE * f  = fopen("../api_key", "r");

    // Woe betide ye who goes here

    char tmp[4096] = { };
    fgets(tmp, 4096, f);
    char * key = calloc(strlen(tmp) + 23, sizeof(char));

    sprintf(key, "Authorization: Bearer %s", tmp);
    key[strlen(key) - 1] = 0;

    fclose(f);

    return key;
}
