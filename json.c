#include "rtl.h"
#include "atlantis.h"
#include "faction.h"
#include "region.h"
#include "storage/stream.h"
#include <cJSON.h>
#include <string.h>
#include <stdlib.h>

cJSON * json_report(faction * f) {
  cJSON * root;

  root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "turn", turn);
  return root;
}

void json_write(cJSON * json, stream * out) {
  char *tok, *rendered = cJSON_Print(json);
  
  tok = strtok(rendered, "\n\r");
  while (tok) {
    if (tok[0]) {
      out->api->writeln(out->handle, tok);
    }
    tok = strtok(NULL, "\n\r");
  }
  free(rendered);
}
