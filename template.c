#include <filestream.h>
#include <cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <bool.h>

static bool auto_tax = false;

static int json_getint(cJSON *json, const char *key, int def) {
  json = cJSON_GetObjectItem(json, key);
  return json ? json->valueint : def;
}

static const char * json_getstr(cJSON *json, const char *key, const char *def) {
  json = cJSON_GetObjectItem(json, key);
  return json ? json->valuestring : def;
}

static int unit_get_item(cJSON *unit, const char *name) {
    cJSON *chld, *items = cJSON_GetObjectItem(unit, "items");
    if (items && items->type==cJSON_Array) {
        for (chld = items->child; chld; chld = chld->next) {
            if (chld->type==cJSON_Object && strcmp(json_getstr(chld, "name", ""), name)==0) {
                return json_getint(chld, "count", 0);
            }
        }
    }
    return 0;
}

static int unit_get_skill(cJSON *unit, const char *name) {
    cJSON *chld, *items = cJSON_GetObjectItem(unit, "skills");
    if (items && items->type==cJSON_Array) {
        for (chld = items->child; chld; chld = chld->next) {
            if (chld->type==cJSON_Object && strcmp(json_getstr(chld, "skill", ""), name)==0) {
                return json_getint(chld, "level", 0);
            }
        }
    }
    return 0;
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
        int u, money = 0, people = 0;
        cJSON *region = cJSON_GetArrayItem(regions, r);
        cJSON *units = cJSON_GetObjectItem(region, "units");

        fprintf(F, ";REGION %s, %s (%d,%d), $%d\n",
                json_getstr(region, "name", "no name"),
                cJSON_GetObjectItem(region, "terrain")->valuestring,
                cJSON_GetObjectItem(region, "x")->valueint,
                cJSON_GetObjectItem(region, "y")->valueint,
                json_getint(region, "money", 0));
        for (u = 0 ; u != cJSON_GetArraySize(units) ; ++u) {
            cJSON *chld, *unit = cJSON_GetArrayItem(units, u);
            chld = cJSON_GetObjectItem(unit, "faction");
            if (chld && fno==chld->valueint) {
                money += json_getint(unit, "money", 0);
                people += json_getint(unit, "number", 0);
            }
        }
        fprintf(F, ";money: %d  people: %d\n", money, people);
        for (u = 0 ; u != cJSON_GetArraySize(units) ; ++u) {
            cJSON *chld, *unit = cJSON_GetArrayItem(units, u);

            chld = cJSON_GetObjectItem(unit, "faction");
            if (chld && fno==chld->valueint) {
                int number = cJSON_GetObjectItem(unit, "number")->valueint;
                fprintf(F, "UNIT %d\n", cJSON_GetObjectItem(unit, "id")->valueint);
                fprintf(F, ";%s, %d, $%d\n",
                        cJSON_GetObjectItem(unit, "name")->valuestring,
                        number,
                        json_getint(unit, "money", 0));
                if (auto_tax) {
                    const char * weapons[] = {"longbow", "sword", "crossbow", 0};
                    int i, total = 0;
                    for (i=0;weapons[i];++i) {
                        const char * weapon = weapons[i];
                        int items = unit_get_item(unit, weapon);
                        int level = unit_get_skill(unit, weapon);
                        if (level>0) {
                            total += items;
                        }
                        if (items>0) {
                            fprintf(F, ";%s: %d\n", weapon, items);
                        }
                    }
                    if (total>0) {
                        fputs("    tax\n", F);
                    }
                }
                fprintf(F, "    %s\n", json_getstr(unit, "default", ""));
            }
        }
        fputc('\n', F);
    }
}

int main (int argc, char **argv) {
    FILE *in = stdin, *out = stdout;
    long len;
    char *data;
    cJSON *json;
    int i = 0;
    
    while (++i!=argc) {
        if (argv[i][0]=='-') {
            switch (argv[i][1]) {
            case 'a':
                auto_tax = true;
                break;
            }
        } else {
            if (in==stdin) {
                in = fopen(argv[i], "r");
                if (!in || errno) {
                    perror("could not open input file");
                    return errno;
                }
            }
            else {
                out = fopen(argv[i], "w");
                if (!out || errno) {
                    perror("could not open output file");
                    return errno;
                }
            }
        }
    }
    fseek(in,0,SEEK_END);
    len=ftell(in);
    fseek(in,0,SEEK_SET);
    data = (char *)malloc(len+1);
    if (data) {
        fread(data,1,len,in);
    }
    if (in!=stdin) fclose(in);

    json = cJSON_Parse(data);
    if (json) {
        print_template(json, out);
        cJSON_Delete(json);
    } else {
        perror("could not parse JSON data");
    }
    free(data);
    if (out!=stdout) fclose(out);

    return 0;
}
