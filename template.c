#include <filestream.h>
#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>

static int json_getint(cJSON *json, const char *key, int def) {
  json = cJSON_GetObjectItem(json, key);
  return json ? json->valueint : def;
}

static const char * json_getstr(cJSON *json, const char *key, const char *def) {
  json = cJSON_GetObjectItem(json, key);
  return json ? json->valuestring : def;
}

void print_template(cJSON *json, FILE *F) {
    int r;
    int fno = 0;
    const char * passwd = "mypassword";
    cJSON *faction, *regions = cJSON_GetObjectItem(json, "regions");

    faction = cJSON_GetObjectItem(json, "faction");
    if (faction) {
        fno = cJSON_GetObjectItem(faction, "id")->valueint;
    }
    fprintf(F, "FACTION %d %s\n", fno, passwd);
    for (r = 0 ; r != cJSON_GetArraySize(regions) ; ++r) {
        int u;
        cJSON *region = cJSON_GetArrayItem(regions, r);
        cJSON *units = cJSON_GetObjectItem(region, "units");

        fprintf(F, ";REGION %s, %s (%d,%d), $%d\n",
                cJSON_GetObjectItem(region, "name")->valuestring,
                cJSON_GetObjectItem(region, "terrain")->valuestring,
                cJSON_GetObjectItem(region, "x")->valueint,
                cJSON_GetObjectItem(region, "y")->valueint,
                json_getint(region, "money", 0));
        for (u = 0 ; u != cJSON_GetArraySize(units) ; ++u) {
            cJSON *chld, *unit = cJSON_GetArrayItem(units, u);

            chld = cJSON_GetObjectItem(unit, "faction");
            if (chld && fno==chld->valueint) {
                fprintf(F, "UNIT %d\n", cJSON_GetObjectItem(unit, "id")->valueint);
                fprintf(F, ";%s, %d, $%d\n",
                        cJSON_GetObjectItem(unit, "name")->valuestring,
                        cJSON_GetObjectItem(unit, "number")->valueint,
                        json_getint(unit, "money", 0));
                fprintf(F, "    %s\n", json_getstr(unit, "default", ""));
            }
        }
        fputc('\n', F);
    }
}

int main (int argc, char **argv) {
    FILE * F;
    long len;
    char *data;
    cJSON *json;
    if (argc>=1) {
        F = fopen(argv[1], "rb");
        if (!F) {
            return -1;
        }
        fseek(F,0,SEEK_END);
        len=ftell(F);
        fseek(F,0,SEEK_SET);
        data = (char *)malloc(len+1);
        if (data) {
            fread(data,1,len,F);
        }
        fclose(F);

        json = cJSON_Parse(data);
        print_template(json, stdout);
        free(data);
        cJSON_Delete(json);
    }
    return 0;
}
