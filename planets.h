#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "cJSON.h"



static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
