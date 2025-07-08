#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "planets.h"
#include "cJSON.h"
#include <time.h>
#include <math.h>
#define PI 3.141592654
#define GRID_WIDTH 150
#define GRID_HEIGHT 40
#define MAX_RANGE 35.0 // in AU, adjust as needed

// Coordinates
typedef struct Coordinates {
    double x;
    double y;
} coordinates_t;

typedef struct Date {
    int day;
    int month;
    int year;
} date_t;

// Planet info
typedef struct Planets {
    char name[15];
    float mass;
    float radius;
    float period;
    float semi_major_axis;
    float temperature;
    float distance_light_year;
    float eccentricity; // manually set
    date_t perihelion_date; // date of last perihelion
    int days_since_perihelion;
    double mean_anomaly;
    double eccentric_anomaly;
    double radial_distance;
    double true_anomaly;
    coordinates_t coordinates;
    char symbol; // symbol for ASCII representation
} planet_t;

// API tutorial
struct MemoryStruct {
  char *memory;
  size_t size;
};

// Processing JSON info
cJSON *get_data_from_json(cJSON *json_item, char *info) {
    cJSON *json_info = cJSON_GetObjectItemCaseSensitive(json_item, info);
    return json_info;
}

// More API/JSON tutorial
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

// Setting the mean anomaly for planet
void set_mean_anomaly_ptr(planet_t * planet_ptr) {
    planet_ptr->mean_anomaly = 2*PI*planet_ptr->days_since_perihelion/planet_ptr->period;
}

// Setting the eccentric anomaly for planet
void set_eccentric_anomaly(planet_t* planet_ptr) { 
    // using approximation E≈M+esinM
    planet_ptr->eccentric_anomaly = (double) planet_ptr->mean_anomaly + planet_ptr->eccentricity * sin(planet_ptr->mean_anomaly);
}

// Setting the radial distance for planet
void set_radial_distance(planet_t* planet_ptr) {
    planet_ptr->radial_distance = planet_ptr->semi_major_axis * (1 - planet_ptr->eccentricity * cos(planet_ptr->eccentric_anomaly));
}

// Setting the true anomaly for planet
void set_true_anomaly(planet_t* planet_ptr) {
    // tan(ν)= sqrt(1−e**2) * sin(E) / cos(E)−e 

    double e = planet_ptr->eccentricity;
    double E = planet_ptr->eccentric_anomaly;

    // Calculate the true anomaly (nu)
    double true_anomaly = atan2(sqrt(1 - e * e) * sin(E), cos(E) - e);
    planet_ptr->true_anomaly = true_anomaly;

}

// Setting the coordinates for planet
void set_coordinates(planet_t* planet_ptr) {
    set_mean_anomaly_ptr(planet_ptr);
    set_eccentric_anomaly(planet_ptr); 
    set_radial_distance(planet_ptr);
    set_true_anomaly(planet_ptr);
    planet_ptr->coordinates.x = planet_ptr->radial_distance * cos(planet_ptr->true_anomaly);
    planet_ptr->coordinates.y = planet_ptr->radial_distance * sin(planet_ptr->true_anomaly);
}

// Function to calculate the number of days in a month
int daysInMonth(int month, int year) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            // Check for leap year
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                return 29;
            } else {
                return 28;
            }
        default:
            return -1; // Invalid month
    }
}

// Calculate number of days since/to perihelion
void set_days_since_perihelion(planet_t* planet_ptr, date_t* user_date) {

    int days1 = planet_ptr->perihelion_date.day + (planet_ptr->perihelion_date.month - 1) * daysInMonth(planet_ptr->perihelion_date.month, planet_ptr->perihelion_date.year) + planet_ptr->perihelion_date.year * 365;
    int days2 = user_date->day + (user_date->month - 1) * daysInMonth(user_date->month, user_date->year) + user_date->year * 365;

    planet_ptr->days_since_perihelion =  (days2 - days1);

}

// Draw orbital ellipse for a planet with specific scale
void draw_orbit(char grid[GRID_HEIGHT][GRID_WIDTH], planet_t* planet, double max_range) {
    double a = planet->semi_major_axis;  // semi-major axis
    double b = a * sqrt(1 - planet->eccentricity * planet->eccentricity);  // semi-minor axis
    
    // Draw ellipse using parametric equations
    for (double theta = 0; theta < 2 * PI; theta += 0.05) {
        double x = a * cos(theta);
        double y = b * sin(theta);
        
        // Convert to grid coordinates
        int grid_x = (int)((x + max_range) * GRID_WIDTH / (2 * max_range));
        int grid_y = (int)((y + max_range) * GRID_HEIGHT / (2 * max_range));
        
        if (grid_x >= 0 && grid_x < GRID_WIDTH && grid_y >= 0 && grid_y < GRID_HEIGHT) {
            if (grid[grid_y][grid_x] == ' ') {  // Only draw if cell is empty
                grid[grid_y][grid_x] = '/';
            }
        }
    }
}

// Draw solar system with specific scale
void draw_solar_system_with_scale(planet_t* planets[], int num_planets, double max_range) {
    // Initialize grid with spaces
    char grid[GRID_HEIGHT][GRID_WIDTH];
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            grid[i][j] = ' ';
        }
    }
    
    // Draw orbital paths first (only for planets within range)
    for (int i = 0; i < num_planets; i++) {
        if (planets[i]->semi_major_axis <= max_range) {
            draw_orbit(grid, planets[i], max_range);
        }
    }
    
    // Place sun at center
    int sun_x = (int)((0 + max_range) * GRID_WIDTH / (2 * max_range));
    int sun_y = (int)((0 + max_range) * GRID_HEIGHT / (2 * max_range));
    if (sun_x >= 0 && sun_x < GRID_WIDTH && sun_y >= 0 && sun_y < GRID_HEIGHT) {
        grid[sun_y][sun_x] = '*';
    }
    
    // Place planets (only those within range)
    for (int i = 0; i < num_planets; i++) {
        double planet_dist = sqrt(planets[i]->coordinates.x * planets[i]->coordinates.x + 
                                 planets[i]->coordinates.y * planets[i]->coordinates.y);
        
        if (planet_dist <= max_range) {
            int planet_x = (int)((planets[i]->coordinates.x + max_range) * GRID_WIDTH / (2 * max_range));
            int planet_y = (int)((planets[i]->coordinates.y + max_range) * GRID_HEIGHT / (2 * max_range));
            
            if (planet_x >= 0 && planet_x < GRID_WIDTH && planet_y >= 0 && planet_y < GRID_HEIGHT) {
                grid[planet_y][planet_x] = planets[i]->symbol;
            }
        }
    }
    
    // Print the grid
    printf("Scale: %.1f AU across\n\n", 2 * max_range);
    
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
    
    // Print legend for visible planets only
    printf("\nVisible: * = Sun, / = Orbital paths\n");
    for (int i = 0; i < num_planets; i++) {
        double planet_dist = sqrt(planets[i]->coordinates.x * planets[i]->coordinates.x + 
                                 planets[i]->coordinates.y * planets[i]->coordinates.y);
        if (planet_dist <= max_range) {
            printf("%c = %s ", planets[i]->symbol, planets[i]->name);
        }
    }
    printf("\n\n");
}

// Main dual-view function
void draw_solar_system_dual_view(planet_t* planets[], char* date) {
    printf("\nSolar System on %s\n", date);
    printf("==================================================\n");
    
    printf("\n=== INNER SOLAR SYSTEM ===\n");
    draw_solar_system_with_scale((planet_t* []){planets[0], planets[1], planets[2], planets[3]}, 4, 3.5);

    printf("\n=== OUTER SOLAR SYSTEM ===\n");
    draw_solar_system_with_scale((planet_t* []){planets[4], planets[5], planets[6], planets[7]}, 4, 35.0);
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

    mercury->perihelion_date = (date_t){3, 6, 2025}; // source https://ssd.jpl.nasa.gov/horizons/app.html#/ -> by enabling the "heliocentric range & range rate" setting in the output, and recording what times that reaches a minimum
    venus->perihelion_date = (date_t){20, 2, 2025}; // source https://ssd.jpl.nasa.gov/horizons/app.html#/
    earth->perihelion_date = (date_t){4, 1, 2025}; // source https://www.timeanddate.com/astronomy/perihelion-aphelion-solstice.html
    mars->perihelion_date = (date_t){9, 5, 2024}; // source https://ssd.jpl.nasa.gov/horizons/app.html#/
    jupiter->perihelion_date = (date_t){21, 1, 2023}; // source wikipedia (confirmed by https://ssd.jpl.nasa.gov/horizons/app.html#/)
    saturn->perihelion_date = (date_t){29, 11, 2032}; // source wikipedia (confirmed by https://ssd.jpl.nasa.gov/horizons/app.html#/)
    uranus->perihelion_date = (date_t){19, 8, 2050}; // source wikipedia (too lazy to confirm)
    neptune->perihelion_date = (date_t){4, 9, 2042}; // source wikipedia (too lazy to confirm)


    // Symbols for planets
    mercury->symbol = 'M';
    venus->symbol = 'V';
    earth->symbol = 'E';
    mars->symbol = 'R';  // 'M' is taken by Mercury
    jupiter->symbol = 'J';
    saturn->symbol = 'S';
    uranus->symbol = 'U';
    neptune->symbol = 'N';

    char user_date[11];
    printf("Enter a date in the dd/mm/yyyy format:\n");
    scanf("%10s", user_date);
    // char user_date[11] = "13/06/2025"; 

    
    // Convert user_date to a struct tm
    int day, month, year;
    sscanf(user_date, "%d/%d/%d", &day, &month, &year);
    
    // write some validations for me
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1000) {
        fprintf(stderr, "Invalid date. Please enter a valid date in the dd/mm/yyyy format.\n");
        return 1;
    }

    date_t user_date_conv = {day, month, year};

    // manual date before prod
    // date_t user_date_conv = {13, 7, 2025};

    planet_t* planets[] = {mercury,venus,earth,mars,jupiter,saturn,uranus,neptune};
        
    for (int i = 0; i < 8; i++) {
        set_days_since_perihelion(planets[i], &user_date_conv);
        set_coordinates(planets[i]);
        // printf("\n\nEccentric Anomaly (E) for %s: %f radians\n", planets[i]->name, planets[i]->eccentric_anomaly);
        // printf("Radial Distance (r) for %s: %f AU\n", planets[i]->name, planets[i]->radial_distance);
        // printf("cos(E): %f, sin(E): %f\n", cos(planets[i]->eccentric_anomaly), sin(planets[i]->eccentric_anomaly));
        // printf("At %s, %s is located at (%f,%f) relative to the sun (0,0)\n\n", user_date, planets[i]->name, planets[i]->coordinates.x, planets[i]->coordinates.y);
    }

    draw_solar_system_dual_view(planets, user_date);

    free(mercury);
    free(venus);
    free(earth);
    free(mars);
    free(jupiter);
    free(saturn);
    free(uranus);
    free(neptune);

    return 0;
}