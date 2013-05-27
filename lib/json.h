/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#ifndef ATL_JSON_H
#define ATL_JSON_H

struct stream;
struct faction;
struct cJSON;

struct cJSON * json_report(const struct faction *f);
void json_write(struct cJSON *json, struct stream *out);
#endif
