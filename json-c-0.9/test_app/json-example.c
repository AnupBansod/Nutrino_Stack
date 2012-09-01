#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
size_t function_pt(char *ptr, size_t size, size_t nmemb, void *stream){
//    printf("%d", size);
    struct json_object *new_obj;
    int pageCount=0;
    new_obj = json_tokener_parse(ptr);
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
    new_obj = json_object_object_get(new_obj, "name");
    if(new_obj!=NULL)
	    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
    if(new_obj!=NULL)
            new_obj = json_object_object_get(new_obj, "pageCount");
    if(new_obj!=NULL)
 	    pageCount = json_object_get_int(new_obj);
    printf("Page count = %d", pageCount);
    if(new_obj!=NULL)
            json_object_put(new_obj);
    return size*nmemb;
}


int main(int argc, char **argv)
{
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, "https://api.twitter.com/1/statuses/public_timeline.json?count=3&include_entities=true");
       curl_easy_setopt(curl, CURLOPT_URL, "http://api.geonames.org/citiesJSON?north=44.1&south=-9.9&east=-22.4&west=55.2&lang=de&username=demo");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt); 
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
