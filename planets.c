#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "planets.h"
#include "cJSON.h"

typedef struct planet_data {
    char name[50];
    float mass;
    float radius;
    float period;
    float semi_major_axis;
    float temperature;
    float distance_light_year;
    float eccentricity; // manually set
    float days_since_perihelion;
} planet_data;

struct MemoryStruct {
  char *memory;
  size_t size;
};

cJSON *get_data_from_json(cJSON *json_item, char *info) {
    cJSON *json_info = cJSON_GetObjectItemCaseSensitive(json_item, info);
    return json_info;
}

planet_data retrieve_planet_data(char *planet_name) {
    
    char* api_key = "V4hB8TpUJAg1cV1+J5/DVA==iHCe8S41JONK282u";
    char* base_url = "https://api.api-ninjas.com/v1/planets?name=";
    char* api_url = malloc(256 * sizeof(char));
    
    strcpy(api_url, base_url);
    strcat(api_url, planet_name);

    // Initialize CURL
    CURL *curl;
    CURLcode result;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "HTTP request failed\n");  
        free(chunk.memory);
        //return NULL;
    }

    // Headers
    struct curl_slist *headers = NULL;
    char header[256];
    sprintf(header, "X-Api-Key: %s", api_key);
    headers = curl_slist_append(headers, header);


    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, api_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
    }
    
    // printf("%s <--- response string\n", chunk.memory);
    curl_easy_cleanup(curl);
    free(api_url);
    
    // parse the JSON data
    cJSON *json = cJSON_Parse(chunk.memory);
    if (json == NULL) {
        printf("Error: Failed to parse JSON\n");
        exit(1); // Exit with an error code
    }    

    free(chunk.memory);

    cJSON *json_item = cJSON_GetArrayItem(json, 0);
    cJSON *name = get_data_from_json(json_item, "name");
    cJSON *mass = get_data_from_json(json_item, "mass");
    cJSON *radius = get_data_from_json(json_item, "radius");
    cJSON *period = get_data_from_json(json_item, "period");
    cJSON *semi_major_axis = get_data_from_json(json_item, "semi_major_axis");
    cJSON *temperature = get_data_from_json(json_item, "temperature");
    cJSON *distance_light_year = get_data_from_json(json_item, "distance_light_year");

    planet_data data;
    strcpy(data.name, name->valuestring);
    data.mass = mass->valuedouble * 100;
    data.radius = radius->valuedouble * 100;
    data.period = period->valuedouble;
    data.semi_major_axis = semi_major_axis->valuedouble;
    data.temperature = temperature->valuedouble;
    data.distance_light_year = distance_light_year->valuedouble;

    return data;
}

// straight stolen from tutorial
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) 
  {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

int main() {
        
    planet_data *mercury = malloc(sizeof(planet_data));
    planet_data *venus = malloc(sizeof(planet_data));
    planet_data *earth = malloc(sizeof(planet_data));          
    planet_data *mars = malloc(sizeof(planet_data));
    planet_data *jupiter = malloc(sizeof(planet_data));
    planet_data *saturn = malloc(sizeof(planet_data));
    planet_data *uranus = malloc(sizeof(planet_data));
    planet_data *neptune = malloc(sizeof(planet_data));

    // char* planet_name = "Mars";
    *mercury = retrieve_planet_data("Mercury");
    *venus = retrieve_planet_data("Venus");
    *earth = retrieve_planet_data("Earth");
    *mars = retrieve_planet_data("Mars");
    *jupiter = retrieve_planet_data("Jupiter");
    *saturn = retrieve_planet_data("Saturn");
    *uranus = retrieve_planet_data("Uranus");
    *neptune = retrieve_planet_data("Neptune");

    // Manually setting eccentricities source: https://nssdc.gsfc.nasa.gov/planetary/factsheet/
    mercury->eccentricity = 0.2056;
    venus->eccentricity = 0.0068;
    earth->eccentricity = 0.0167;
    mars->eccentricity = 0.0934;
    jupiter->eccentricity = 0.0489;
    saturn->eccentricity = 0.0565;
    uranus->eccentricity = 0.0463;
    neptune->eccentricity = 0.0086;

    // Manually setting days since perihelion source: 
    mercury->days_since_perihelion = 
    venus->days_since_perihelion = 
    earth->days_since_perihelion = 
    mars->days_since_perihelion = 
    jupiter->days_since_perihelion =;
    saturn->days_since_perihelion = 
    uranus->days_since_perihelion = 
    neptune->days_since_perihelion =;

    printf("%s has %f and %f %% of Jupiter's mass and radius.\n", saturn->name, saturn->mass, saturn->radius);
    printf("%s's period is %f and its semi-major axis is %f\n", 
           saturn->name, saturn->period, saturn->semi_major_axis);

    free(venus);
    free(earth);
    free(mars);
    free(jupiter);
    free(saturn);
    free(uranus);
    free(neptune);

    return 0;
}