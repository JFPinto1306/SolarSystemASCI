#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "planets.h"
#include "cJSON.h"
#include <time.h>
#include <math.h>
#define PI 3.141592654

typedef struct Coordinates {
    double x;
    double y;
} coordinates_t;

typedef struct Planets {
    char name[15];
    float mass;
    float radius;
    float period;
    float semi_major_axis;
    float temperature;
    float distance_light_year;
    float eccentricity; // manually set
    char* perihelion_date; // date of last perihelion
    float days_since_perihelion;
    double mean_anomaly;
    double eccentric_anomaly;
    double radial_distance;
    coordinates_t coordinates;
} planet_t;

// Read JSON straight stolen from tutorial
struct MemoryStruct {
  char *memory;
  size_t size;
};

cJSON *get_data_from_json(cJSON *json_item, char *info) {
    cJSON *json_info = cJSON_GetObjectItemCaseSensitive(json_item, info);
    return json_info;
}

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


// API call for planet data
planet_t retrieve_planet_t(char *planet_name) {
    
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

    planet_t data;
    strcpy(data.name, name->valuestring);
    data.mass = mass->valuedouble * 100;
    data.radius = radius->valuedouble * 100;
    data.period = period->valuedouble;
    data.semi_major_axis = semi_major_axis->valuedouble;
    data.temperature = temperature->valuedouble;
    data.distance_light_year = distance_light_year->valuedouble;

    return data;
}

void set_mean_anomaly_ptr(planet_t * planet_ptr) {
    planet_ptr->mean_anomaly = 2*PI*planet_ptr->days_since_perihelion/planet_ptr->period;
}


void set_eccentric_anomaly(planet_t* planet_ptr) { 
    // using approximation E≈M+esinM
    planet_ptr->eccentric_anomaly = (double) planet_ptr->mean_anomaly + planet_ptr->eccentricity * sin(planet_ptr->mean_anomaly);

}


void set_radial_distance(planet_t* planet_ptr) {
    
    planet_ptr->radial_distance = planet_ptr->semi_major_axis * (1 - planet_ptr->eccentric_anomaly * cos(planet_ptr->mean_anomaly));
}

void set_coordinates(planet_t* planet_ptr) {
    set_mean_anomaly_ptr(planet_ptr);
    set_eccentric_anomaly(planet_ptr); 
    set_radial_distance(planet_ptr);
    planet_ptr->coordinates.x = (double)planet_ptr->radial_distance * (cos(planet_ptr->mean_anomaly) - planet_ptr->eccentric_anomaly);
    planet_ptr->coordinates.y = (double)planet_ptr->radial_distance * (sin(planet_ptr->mean_anomaly) - planet_ptr->eccentric_anomaly);
}


int main() {
        
    planet_t *mercury = malloc(sizeof(planet_t));
    planet_t *venus = malloc(sizeof(planet_t));
    planet_t *earth = malloc(sizeof(planet_t));          
    planet_t *mars = malloc(sizeof(planet_t));
    planet_t *jupiter = malloc(sizeof(planet_t));
    planet_t *saturn = malloc(sizeof(planet_t));
    planet_t *uranus = malloc(sizeof(planet_t));
    planet_t *neptune = malloc(sizeof(planet_t));

    // char* planet_name = "Mars";
    *mercury = retrieve_planet_t("Mercury");
    *venus = retrieve_planet_t("Venus");
    *earth = retrieve_planet_t("Earth");
    *mars = retrieve_planet_t("Mars");
    *jupiter = retrieve_planet_t("Jupiter");
    *saturn = retrieve_planet_t("Saturn");
    *uranus = retrieve_planet_t("Uranus");
    *neptune = retrieve_planet_t("Neptune");

    // Manually setting eccentricities source: https://nssdc.gsfc.nasa.gov/planetary/factsheet/
    mercury->eccentricity = 0.2056;
    venus->eccentricity = 0.0068;
    earth->eccentricity = 0.0167;
    mars->eccentricity = 0.0934;
    jupiter->eccentricity = 0.0489;
    saturn->eccentricity = 0.0565;
    uranus->eccentricity = 0.0463;
    neptune->eccentricity = 0.0086;

    // Manually setting perihelion dates. 

    mercury->perihelion_date = "03/06/2025"; // source https://ssd.jpl.nasa.gov/horizons/app.html#/ -> by enabling the "heliocentric range & range rate" setting in the output, and recording what times that reaches a minimum
    venus->perihelion_date = "20/02/2025"; // source https://ssd.jpl.nasa.gov/horizons/app.html#/
    earth->perihelion_date = "04/01/2025"; // source https://www.timeanddate.com/astronomy/perihelion-aphelion-solstice.html
    mars->perihelion_date = "09/05/2024"; // source https://ssd.jpl.nasa.gov/horizons/app.html#/
    jupiter->perihelion_date = "21/01/2023"; // source wikipedia (confirmed by https://ssd.jpl.nasa.gov/horizons/app.html#/)
    saturn->perihelion_date = "29/11/2032"; // source wikipedia (confirmed by https://ssd.jpl.nasa.gov/horizons/app.html#/)
    uranus->perihelion_date = "19/08/2050"; // source wikipedia (too lazy to confirm)
    neptune->perihelion_date = "04/09/2042"; // source wikipedia (too lazy to confirm)


    // testing days_since_perihelion = 0

    
    planet_t* planets[] = {mercury,venus,earth,mars,jupiter,saturn,uranus,neptune};

    for (int i = 0; i < 8; i++) {
        planets[i]->days_since_perihelion = 0;
        set_coordinates(planets[i]);
        printf("When %s is the closest to the sun (aka perihelion), it is located at (%f,%f) relative to the sun (0,0)\n", planets[i]->name, planets[i]->coordinates.x, planets[i]->coordinates.x);
    }


    // char user_date[11];
    // printf("Enter a date in the dd/mm/yyyy format:");
    // scanf("%10s", user_date);

    //planet_t planet = *uranus;

    //printf("%s has %f and %f %% of Jupiter's mass and radius.\n", planet.name, planet.mass, planet.radius);
    //printf("%s's period is %f days and its semi-major axis is %f\n", 
    //       planet.name, planet.period, planet.semi_major_axis);


    free(venus);
    free(earth);
    free(mars);
    free(jupiter);
    free(saturn);
    free(uranus);
    free(neptune);

    return 0;
}