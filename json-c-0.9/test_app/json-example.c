#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
function_pt(char *ptr, size_t size, size_t nmemb, void *stream){
//  printf("%s", ptr);
    struct json_object *new_obj;
    new_obj = json_tokener_parse(ptr);
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
    new_obj = json_object_object_get(new_obj, "glossary");
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
    new_obj = json_object_object_get(new_obj, "pageCount");
    int pageCount = json_object_get_int(new_obj);
    printf("Page count = %d", pageCount);
    json_object_put(new_obj);
}


int main(int argc, char **argv)
{
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.twitter.com/1/statuses/public_timeline.json?count=3&include_entities=true");
        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }


    return 0;
}
