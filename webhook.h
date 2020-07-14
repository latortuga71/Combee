#include <curl/curl.h>

int post(char* destination,char *jsonData)
{
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_ALL);

        curl = curl_easy_init();
        if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, destination);
                //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"title\" : \"The Title\"}");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData);
                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Accept: application/json");
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, "charset: utf-8");
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl,CURLOPT_TIMEOUT,5L); // 5 seconds before timeout

                res = curl_easy_perform(curl);

                if(res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                        curl_easy_strerror(res));

                curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        return 0;
}