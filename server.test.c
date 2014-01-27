#include <atlantis.h>
#include <region.h>
#include <faction.h>
#include <game.h>
#include <ship.h>
#include <building.h>
#include <unit.h>

#include <rtl.h>

#include "json.h"

#include <stream.h>
#include <memstream.h>
#include <filestream.h>
#include <quicklist.h>

#include <CuTest.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

static void test_json_report(CuTest * tc) {
    region * r;
    faction * f;
    unit * u;
    cJSON * json;

    cleargame(true);
    turn = 0;
    r = create_region(0, 1, 1, create_terrain("plain"));
    f = addplayer(r, 0, 0);
    u = r->units;
    update_world(0, 0, 1, 1);

    json = json_report(f);
    CuAssertIntEquals(tc, turn, cJSON_GetObjectItem(json, "turn")->valueint);
    free(json);
}

static void test_json_write(CuTest * tc) {
    char buf[256];
    cJSON * json = cJSON_CreateObject();
    stream strm;
    mstream_init(&strm);
    cJSON_AddNumberToObject(json, "turn", 1);
    json_write(json, &strm);
    strm.api->rewind(strm.handle);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "{", buf);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "\t\"turn\":\t1", buf);
    strm.api->readln(strm.handle, buf, sizeof(buf));
    CuAssertStrEquals(tc, "}", buf);
    cJSON_Delete(json);
    mstream_done(&strm);
}

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_json_report);
    SUITE_ADD_TEST(suite, test_json_write);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return suite->failCount;
}

