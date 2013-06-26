/*
 * Atlantis v1.0 13 September 1993
 * Copyright 1993 by Russell Wallace
 *
 * This program may be freely used, modified and distributed. It may not be
 * sold or used commercially without prior written permission from the author.
 */

#include "atlantis.h"
#include "faction.h"
#include "keywords.h"
#include "region.h"
#include "building.h"
#include "ship.h"
#include "unit.h"
#include "battle.h"
#include "settings.h"
#include "spells.h"
#include "skills.h"
#include "items.h"
#include "parser.h"
#include "report.h"
#include "combat.h"
#include "game.h"

#include "rtl.h"
#include "bool.h"

#include "rtl.h"

#include <storage.h>
#include <binarystore.h>
#include <textstore.h>
#include <stream.h>
#include <quicklist.h>
#include <filestream.h>
#include <mtrand.h>
#include <cJSON.h>

int ignore_password = 0;

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <stddef.h>
#include <limits.h>

#define VER_NOHEADER 0 // no version header
#define VER_HEADER 1 // has a version header
#define VER_STACKS 2 // can have stacks

#define VER_CURRENT VER_STACKS

static void (*store_init)(struct storage *, FILE *) = binstore_init;
static void (*store_done)(struct storage *) = binstore_done;

typedef struct order {
    struct order *next;
    struct unit *unit;
    int qty;
} order;

char buf2[256];

int turn;

const char *keywords[] = {
    "accept",
    "address",
    "admit",
    "ally",
    "attack",
    "behind",
    "board",
    "build",
    "building",
    "cast",
    "clipper",
    "combat",
    "demolish",
    "display",
    "east",
    "end",
    "enter",
    "entertain",
    "faction",
    "find",
    "form",
    "galleon",
    "give",
    "guard",
    "leave",
    "longboat",
    "mir",
    "move",
    "name",
    "north",
    "password",
    "pay",
    "produce",
    "promote",
    "quit",
    "recruit",
    "research",
    "reshow",
    "sail",
    "ship",
    "sink",
    "south",
    "stack",
    "study",
    "tax",
    "teach",
    "transfer",
    "unit",
    "unstack",
    "west",
    "work",
    "ydd",
};

const char *terrainnames[] = {
    "ocean",
    "plain",
    "mountain",
    "forest",
    "swamp",
};

const char *regionnames[] = {
    "Aberaeron",
    "Aberdaron",
    "Aberdovey",
    "Abernethy",
    "Abersoch",
    "Abrantes",
    "Adrano",
    "AeBrey",
    "Aghleam",
    "Akbou",
    "Aldan",
    "Alfaro",
    "Alghero",
    "Almeria",
    "Altnaharra",
    "Ancroft",
    "Anshun",
    "Anstruther",
    "Antor",
    "Arbroath",
    "Arcila",
    "Ardfert",
    "Ardvale",
    "Arezzo",
    "Ariano",
    "Arlon",
    "Avanos",
    "Aveiro",
    "Badalona",
    "Baechahoela",
    "Ballindine",
    "Balta",
    "Banlar",
    "Barika",
    "Bastak",
    "Bayonne",
    "Bejaia",
    "Benlech",
    "Beragh",
    "Bergland",
    "Berneray",
    "Berriedale",
    "Binhai",
    "Birde",
    "Bocholt",
    "Bogmadie",
    "Braga",
    "Brechlin",
    "Brodick",
    "Burscough",
    "Calpio",
    "Canna",
    "Capperwe",
    "Caprera",
    "Carahue",
    "Carbost",
    "Carnforth",
    "Carrigaline",
    "Caserta",
    "Catrianchi",
    "Clatter",
    "Coilaco",
    "Corinth",
    "Corofin",
    "Corran",
    "Corwen",
    "Crail",
    "Cremona",
    "Crieff",
    "Cromarty",
    "Cumbraes",
    "Daingean",
    "Darm",
    "Decca",
    "Derron",
    "Derwent",
    "Deveron",
    "Dezhou",
    "Doedbygd",
    "Doramed",
    "Dornoch",
    "Drammes",
    "Dremmer",
    "Drense",
    "Drimnin",
    "Drumcollogher",
    "Drummore",
    "Dryck",
    "Drymen",
    "Dunbeath",
    "Duncansby",
    "Dunfanaghy",
    "Dunkeld",
    "Dunmanus",
    "Dunster",
    "Durness",
    "Duucshire",
    "Elgomaar",
    "Ellesmere",
    "Ellon",
    "Enfar",
    "Erisort",
    "Eskerfan",
    "Ettrick",
    "Fanders",
    "Farafra",
    "Ferbane",
    "Fetlar",
    "Flock",
    "Florina",
    "Formby",
    "Frainberg",
    "Galloway",
    "Ganzhou",
    "Geal Charn",
    "Gerr",
    "Gifford",
    "Girvan",
    "Glenagallagh",
    "Glenanane",
    "Glin",
    "Glomera",
    "Glormandia",
    "Gluggby",
    "Gnackstein",
    "Gnoelhaala",
    "Golconda",
    "Gourock",
    "Graevbygd",
    "Grandola",
    "Gresberg",
    "Gresir",
    "Greverre",
    "Griminish",
    "Grisbygd",
    "Groddland",
    "Grue",
    "Gurkacre",
    "Haikou",
    "Halkirk",
    "Handan",
    "Hasmerr",
    "Helmsdale",
    "Helmsley",
    "Helsicke",
    "Helvete",
    "Hoersalsveg",
    "Hullevala",
    "Ickellund",
    "Inber",
    "Inverie",
    "Jaca",
    "Jahrom",
    "Jeormel",
    "Jervbygd",
    "Jining",
    "Jotel",
    "Kaddervar",
    "Karand",
    "Karothea",
    "Kashmar",
    "Keswick",
    "Kielder",
    "Killorglin",
    "Kinbrace",
    "Kintore",
    "Kirriemuir",
    "Klen",
    "Knesekt",
    "Kobbe",
    "Komarken",
    "Kovel",
    "Krod",
    "Kursk",
    "Lagos",
    "Lamlash",
    "Langholm",
    "Larache",
    "Larkanth",
    "Larmet",
    "Lautaro",
    "Leighlin",
    "Lervir",
    "Leven",
    "Licata",
    "Limavady",
    "Lingen",
    "Lintan",
    "Liscannor",
    "Locarno",
    "Lochalsh",
    "Lochcarron",
    "Lochinver",
    "Lochmaben",
    "Lom",
    "Lorthalm",
    "Louer",
    "Lurkabo",
    "Luthiir",
    "Lybster",
    "Lynton",
    "Mallaig",
    "Mataro",
    "Melfi",
    "Melvaig",
    "Menter",
    "Methven",
    "Moffat",
    "Monamolin",
    "Monzon",
    "Morella",
    "Morgel",
    "Mortenford",
    "Mullaghcarn",
    "Mulle",
    "Murom",
    "Nairn",
    "Navenby",
    "Nephin Beg",
    "Niskby",
    "Nolle",
    "Nork",
    "Olenek",
    "Oloron",
    "Oranmore",
    "Ormgryte",
    "Orrebygd",
    "Palmi",
    "Panyu",
    "Partry",
    "Pauer",
    "Penhalolen",
    "Perkel",
    "Perski",
    "Planken",
    "Plattland",
    "Pleagne",
    "Pogelveir",
    "Porthcawl",
    "Portimao",
    "Potenza",
    "Praestbygd",
    "Preetsome",
    "Presu",
    "Prettstern",
    "Rantlu",
    "Rappbygd",
    "Rath Luire",
    "Rethel",
    "Riggenthorpe",
    "Rochfort",
    "Roddendor",
    "Roin",
    "Roptille",
    "Roter",
    "Rueve",
    "Sagunto",
    "Saklebille",
    "Salen",
    "Sandwick",
    "Sarab",
    "Sarkanvale",
    "Scandamia",
    "Scarinish",
    "Scourie",
    "Serov",
    "Shanyin",
    "Siegen",
    "Sinan",
    "Sines",
    "Skim",
    "Skokholm",
    "Skomer",
    "Skottskog",
    "Sledmere",
    "Sorisdale",
    "Spakker",
    "Stackforth",
    "Staklesse",
    "Stinchar",
    "Stoer",
    "Strichen",
    "Stroma",
    "Stugslett",
    "Suide",
    "Tabuk",
    "Tarraspan",
    "Tetuan",
    "Thurso",
    "Tiemcen",
    "Tiksi",
    "Tolsta",
    "Toppola",
    "Torridon",
    "Trapani",
    "Tromeforth",
    "Tudela",
    "Turia",
    "Uxelberg",
    "Vaila",
    "Valga",
    "Verguin",
    "Vernlund",
    "Victoria",
    "Waimer",
    "Wett",
    "Xontormia",
    "Yakleks",
    "Yuci",
    "Zaalsehuur",
    "Zamora",
    "Zapulla",
};

const keyword_t directions[MAXDIRECTIONS] = { K_NORTH, K_SOUTH, K_EAST, K_WEST, K_MIR, K_YDD };

char foodproductivity[] = {
    0,
    15,
    12,
    12,
    12,
};

int maxfoodoutput[] = {
    0,
    100000,
    20000,
    20000,
    10000,
};

char productivity[NUMTERRAINS][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {1, 0, 1, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
};

int maxoutput[NUMTERRAINS][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 200},
    {200, 0, 200, 0},
    {0, 200, 0, 0},
    {0, 100, 0, 0},
};

int shipcapacity[] = {
    200,
    800,
    1800,
};

int shipcost[] = {
    100,
    200,
    300,
};

char itemskill[] = {
    SK_MINING,
    SK_LUMBERJACK,
    SK_QUARRYING,
    SK_HORSE_TRAINING,
    SK_WEAPONSMITH,
    SK_WEAPONSMITH,
    SK_WEAPONSMITH,
    SK_ARMORER,
    SK_ARMORER,
};

char rawmaterial[] = {
    0,
    0,
    0,
    0,
    I_IRON,
    I_WOOD,
    I_WOOD,
    I_IRON,
    I_IRON,
};

int spelllevel[] = {
    4,
    2,
    1,
    1,
    2,
    3,
    2,
    2,
    1,
    5,
    4,
    3,
    3,
    3,
    3,
    4,
    3,
    4,
    3,
    2,
    4,
    3,
    5,
    3,
};

char iscombatspell[] = {
    1,
    1,
    0,
    1,
    1,
    1,
    0,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
};

const char *spelldata[] = {
    "This spell creates a black whirlwind of energy which destroys all life, "
    "leaving frozen corpses with faces twisted into expressions of horror. Cast "
    "in battle, it kills from 2 to 1250 enemies.",

    "This spell creates an aura of fear which causes enemy troops in battle to "
    "panic. Each time it is cast, it demoralizes between 2 and 100 troops for "
    "the duration of the battle. Demoralized troops are at a -1 to their "
    "effective skill.",

    "This ritual spell causes pestilence to contaminate the water supply of the "
    "region in which it is cast. It causes from 2 to 50 peasants to die from "
    "drinking the contaminated water. Any units which end the month in the "
    "affected region will know about the deaths, however only units which have "
    "Observation skill higher than the caster's Stealth skill will know who "
    "was responsible. The spell costs $50 to cast.",

    "This spell, cast in battle, creates a flash of light which dazzles from 2 "
    "to 50 enemy troops. Dazzled troops are at a -1 to their effective skill "
    "for their next attack.",

    "This spell enables the caster to hurl balls of fire. Each time it is cast "
    "in battle, it will incinerate between 2 and 50 enemy troops.",

    "This spell disrupts the metabolism of living organisms, sending an "
    "invisible wave of death across a battlefield. Each time it is cast, it "
    "will kill between 2 and 250 enemy troops.",

    "This spell enables the caster to attempt to heal the injured after a "
    "battle. It is used automatically, and does not require use of either the "
    "COMBAT or CAST command. If one's side wins a battle, a number of "
    "casualties on one's side, between 2 and 50, will be healed. (If this "
    "results in all the casualties on the winning side being healed, the winner "
    "is still eligible for combat experience.)",

    "This spell boosts the morale of one's troops in battle. Each time it is "
    "cast, it cancels the effect of the Cause Fear spell on a number of one's "
    "own troops ranging from 2 to 100.",

    "This spell enables the caster to throw bolts of lightning to strike down "
    "enemies in battle. It kills from 2 to 10 enemies.",

    "This spell allows one to create an Amulet of Darkness. This amulet allows "
    "its possessor to cast the Black Wind spell in combat, without having to "
    "know the spell; the only requirement is that the user must have the Magic "
    "skill at 1 or higher. The Black Wind spell creates a black whirlwind of "
    "energy which destroys all life. Cast in battle, it kills from 2 to 1250 "
    "people. The amulet costs $1000 to make.",

    "This spell allows one to create an Amulet of Death. This amulet allows its "
    "possessor to cast the Hand of Death spell in combat, without having to "
    "know the spell; the only requirement is that the user must have the Magic "
    "skill at 1 or higher. The Hand of Death spell disrupts the metabolism of "
    "living organisms, sending an invisible wave of death across a battlefield. "
    "Each time it is cast, it will kill between 2 and 250 enemy troops. The "
    "amulet costs $800 to make.",

    "This spell allows one to create an Amulet of Healing. This amulet allows "
    "its possessor to attempt to heal the injured after a battle. It is used "
    "automatically, and does not require the use of either the COMBAT or CAST "
    "command; the only requirement is that the user must have the Magic skill "
    "at 1 or higher. If the user's side wins a battle, a number of casualties "
    "on that side, between 2 and 50, will be healed. (If this results in all "
    "the casualties on the winning side being healed, the winner is still "
    "eligible for combat experience.) The amulet costs $600 to make.",

    "This spell allows one to create an Amulet of True Seeing. This allows its "
    "possessor to see units which are hidden by Rings of Invisibility. (It has "
    "no effect against units which are hidden by the Stealth skill.) The amulet "
    "costs $600 to make.",

    "This spell allows one to create a Cloak of Invulnerability. This cloak "
    "protects its wearer from injury in battle; any attack with a normal weapon "
    "which hits the wearer has a 99.99% chance of being deflected. This benefit "
    "is gained instead of, rather than as well as, the protection of any armor "
    "worn; and the cloak confers no protection against magical attacks. The "
    "cloak costs $600 to make.",

    "This spell allows one to create a Ring of Invisibility. This ring renders "
    "its wearer invisible to all units not in the same faction, regardless of "
    "Observation skill. For a unit of many people to remain invisible, it must "
    "possess a Ring of Invisibility for each person. The ring costs $600 to "
    "make.",

    "This spell allows one to create a Ring of Power. This ring doubles the "
    "effectiveness of any spell the wearer casts in combat, or any magic item "
    "the wearer uses in combat. The ring costs $800 to make.",

    "This spell allows one to create a Runesword. This is a black sword with "
    "magical runes etched along the blade. To use it, one must have both the "
    "Sword and Magic skills at 1 or higher. It confers a bonus of 2 to the "
    "user's Sword skill in battle, and also projects an aura of power that has "
    "a 50% chance of cancelling any Fear spells cast by an enemy magician. The "
    "sword costs $600 to make.",

    "This spell allows one to create a Shieldstone. This is a small black "
    "stone, engraved with magical runes, that creates an invisible shield of "
    "energy that deflects hostile magic in battle. The stone is used "
    "automatically, and does not require the use of either the COMBAT or CAST "
    "commands; the only requirement is that the user must have the Magic skill "
    "at 1 or higher. Each round of combat, it adds one layer to the shielding "
    "around one's own side. When a hostile magician casts a spell, provided "
    "there is at least one layer of shielding present, there is a 50% chance of "
    "the spell being deflected. If the spell is deflected, nothing happens. If "
    "it is not, then it has full effect, and one layer of shielding is removed. "
    "The stone costs $800 to make.",

    "This spell allows one to create a Staff of Fire. This staff allows its "
    "possessor to cast the Fireball spell in combat, without having to know the "
    "spell; the only requirement is that the user must have the Magic skill at "
    "1 or higher. The Fireball spell enables the caster to hurl balls of fire. "
    "Each time it is cast in battle, it will incinerate between 2 and 50 enemy "
    "troops. The staff costs $600 to make.",
    
    "This spell allows one to create a Staff of Lightning. This staff allows "
    "its possessor to cast the Lightning Bolt spell in combat, without having "
    "to know the spell; the only requirement is that the user must have the "
    "Magic skill at 1 or higher. The Lightning Bolt spell enables the caster to "
    "throw bolts of lightning to strike down enemies. It kills from 2 to 10 "
    "enemies. The staff costs $400 to make.",
    
    "This spell allows one to create a Wand of Teleportation. This wand allows "
    "its possessor to cast the Teleport spell, without having to know the "
    "spell; the only requirement is that the user must have the Magic skill at "
    "1 or higher. The Teleport spell allows the caster to move himself and "
    "others across vast distances without traversing the intervening space. The "
    "command to use it is CAST TELEPORT target-unit unit-no ... The target unit "
    "is a unit in the region to which the teleport is to occur. If the target "
    "unit is not in your faction, it must be in a faction which has issued an "
    "ADMIT command for you that month. After the target unit comes a list of "
    "one or more units to be teleported into the target unit's region (this may "
    "optionally include the caster). Any units to be teleported, not in your "
    "faction, must be in a faction which has issued an ACCEPT command for you "
    "that month. The total weight of all units to be teleported (including "
    "people, equipment and horses) must not exceed 10000. If the target unit is "
    "in a building or on a ship, the teleported units will emerge there, "
    "regardless of who owns the building or ship. The caster spends the month "
    "preparing the spell and the teleport occurs at the end of the month, so "
    "any other units to be transported can spend the month doing something "
    "else. The wand costs $800 to make, and $50 to use.",
    
    "This spell creates an invisible shield of energy that deflects hostile "
    "magic. Each round that it is cast in battle, it adds one layer to the "
    "shielding around one's own side. When a hostile magician casts a spell, "
    "provided there is at least one layer of shielding present, there is a 50% "
    "chance of the spell being deflected. If the spell is deflected, nothing "
    "happens. If it is not, then it has full effect, and one layer of shielding "
    "is removed.",
    
    "This spell allows the caster to incinerate whole armies with fire brighter "
    "than the sun. Each round it is cast, it kills from 2 to 6250 enemies.",
    
    "This spell allows the caster to move himself and others across vast "
    "distances without traversing the intervening space. The command to use it "
    "is CAST TELEPORT target-unit unit-no ... The target unit is a unit in the "
    "region to which the teleport is to occur. If the target unit is not in "
    "your faction, it must be in a faction which has issued an ADMIT command "
    "for you that month. After the target unit comes a list of one or more "
    "units to be teleported into the target unit's region (this may optionally "
    "include the caster). Any units to be teleported, not in your faction, must "
    "be in a faction which has issued an ACCEPT command for you that month. The "
    "total weight of all units to be teleported (including people, equipment "
    "and horses) must not exceed 10000. If the target unit is in a building or "
    "on a ship, the teleported units will emerge there, regardless of who owns "
    "the building or ship. The caster spends the month preparing the spell and "
    "the teleport occurs at the end of the month, so any other units to be "
    "transported can spend the month doing something else. The spell costs $50 "
    "to cast.",
};

typedef struct strlist {
    struct strlist *next;
    char s[1];
} strlist;

static void freestrlist(strlist * slist) {
    while (slist) {
        strlist * sl = slist;
        slist = sl->next;
        free(sl);
    }
}

int atoip(const char *s)
{
    int n;

    n = atoi(s);
    return (n < 0) ? 0 : n;
}

void nstrcpy(char *to, const char *from, size_t n)
{
    n--;

    do
        if ((*to++ = *from++) == 0)
            return;
    while (--n);

    *to = 0;
}

void rnd_seed(unsigned long x)
{
    init_genrand(x);
}

static void removelist(quicklist **qlp, void *data) {
    ql_iter qli;
    for (qli=qli_init(qlp);qli_more(qli);) {
        void *x = qli_get(qli);
        if (x==data) {
            qli_delete(&qli);
            break;
        }
        qli_next(&qli);
    }
}


strlist *makestrlist(char *s)
{
    strlist *S;

    S = (strlist *)malloc(sizeof(strlist) + strlen(s));
    if (S) {
        S->next = 0;
        strcpy(S->s, s);
    }
    return S;
}

void addevent(faction * f, const char *s)
{
    ql_push(&f->events, _strdup(s));
}

void addmessage(faction * f, const char *s)
{
    ql_push(&f->messages, _strdup(s));
}

void addstrlist(strlist ** SP, char *s)
{
    while (*SP) SP = &(*SP)->next;
    *SP = makestrlist(s);
}

FILE * cfopen(const char *filename, const char *mode)
{
    FILE * F = fopen(filename, mode);

    if (F == 0) {
        printf("Can't open file %s in mode %s.\n", filename, mode);
        exit(1);
    }
    return F;
}

int transform(int *x, int *y, int direction)
{
    keyword_t kwd;
    assert(x || !"invalid reference to X coordinate");
    assert(y || !"invalid reference to Y coordinate");

    kwd = (direction<MAXDIRECTIONS) ? directions[direction] : MAXKEYWORDS;
    if (kwd==K_NORTH) {
        --*y;
    }
    else if (kwd==K_SOUTH) {
        ++*y;
    }
    else if (kwd==K_WEST) {
        --*x;
    }
    else if (kwd==K_EAST) {
        ++*x;
    }
    else if (kwd==K_MIR) {
        --*x;
        --*y;
    }
    else if (kwd==K_YDD) {
        ++*x;
        ++*y;
    }
    else {
        return EINVAL;
    }
    if (config.width && config.height) {
        if (*x<0) *x+=config.width;
        if (*y<0) *y+=config.height;
        if (*x>=config.width) *x-=config.width;
        if (*y>=config.height) *y-=config.height;
    }
    return 0;
}

int effskill(const unit * u, int i)
{
    int n, j, result;

    n = 0;
    if (u->number)
        n = u->skills[i] / u->number;
    j = 30;
    result = 0;

    while (j <= n) {
        n -= j;
        j += 30;
        result++;
    }

    return result;
}

int cansee(const faction * f, region * r, const unit * ux)
{
    int n, o;
    int cansee;
    unit *u;

    if (ux->faction == f)
        return 2;

    cansee = 0;
    if (ux->guard || ux->building || ux->ship)
        cansee = 1;

    n = effskill(ux, SK_STEALTH);

    for (u = r->units_;u;u=u->next) {
        if (u->faction != f)
            continue;

        if (ux->items[I_RING_OF_INVISIBILITY] &&
            ux->items[I_RING_OF_INVISIBILITY] == ux->number &&
            !u->items[I_AMULET_OF_TRUE_SEEING])
            continue;

        o = effskill(u, SK_OBSERVATION);
        if (o > n)
            return 2;
        if (o >= n)
            cansee = 1;
    }

    return cansee;
}

faction *findfaction(int n)
{
    ql_iter fli;

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (f->no == n) return f;
    }
    return 0;
}

faction *getfaction(void)
{
    return findfaction(atoi(getstr()));
}

region *findregion(int x, int y)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        if (r->x == x && r->y == y)
            return r;
    }

    return 0;
}

building *findbuilding(int n)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        ql_iter qli;

        for (qli=qli_init(&r->buildings);qli_more(qli);) {
            building *b = (building *)qli_next(&qli);
            if (b->no == n)
                return b;
        }
    }
    return 0;
}

building *getbuilding(region * r)
{
    int n;
    ql_iter qli;

    n = atoi(getstr());

    for (qli=qli_init(&r->buildings);qli_more(qli);) {
        building *b = (building *)qli_next(&qli);
        if (b->no == n) {
            return b;
        }
    }

    return 0;
}

ship *findship(int n)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        ql_iter sli;

        for (sli = qli_init(&r->ships); qli_more(sli);) {
            ship *sh = (ship *)qli_next(&sli);
            if (sh->no == n)
                return sh;
        }
    }
    return 0;
}

ship *getship(region * r)
{
    int n;
    ql_iter sli;

    n = atoi(getstr());

    for (sli = qli_init(&r->ships); qli_more(sli);) {
        ship *sh = (ship *)qli_next(&sli);
        if (sh->no == n) {
            return sh;
        }
    }

    return 0;
}

unit *findunitg(int n)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        for (u = r->units_;u;u=u->next) {
            if (u->no == n) {
                return u;
            }
        }
    }

    return 0;
}

faction * addplayer(region * r, const char * email, int no)
{
    faction * f;
    unit * u;
    int i;
    char msg[1024];
    char name[NAMESIZE];
    
    if (no==0) ++no;
    while (findfaction(no)) ++no;

    f = create_faction(no);
    f->lastorders = turn;
    sprintf(name, "Faction %d", f->no);
    faction_setname(f, name);
    faction_setaddr(f, email);

    for (i=0;i!=4;++i) {
        sprintf(buf2+i*4, "%4x", genrand_int32());
    }
    buf2[16] = 0;
    faction_setpassword(f, buf2);
    sprintf(msg, "Your password is '%s'.", buf2);
    addmessage(f, msg);

    while (findunitg(nextunitid)) ++nextunitid;
    u = create_unit(f, nextunitid++);
    region_addunit(r, u, 0, 0);
    strcpy(u->lastorder, "work");
    u->combatspell = -1;
    u->number = 1;
    u->money = STARTMONEY;
    u->isnew = true;

    f->origin_x = r->x;
    f->origin_y = r->y;
    return f;
}

void addplayers(region *r, stream *strm)
{
    int no = 0;
    char buf[1024];
    for (;;) {
        faction *f;
        if (strm->api->readln(strm->handle, buf, sizeof(buf))!=0) {
            break;
        }

        f = addplayer(r, buf, no);
        no = f->no+1;
    }
}

void connecttothis(region * r, int x, int y, int from, int to)
{
    region *r2;

    r2 = findregion(x, y);

    if (r2) {
        r->connect[from] = r2;
        r2->connect[to] = r;
    }
}

void connectregion(region * r)
{
    int d;
    for (d=0;d!=MAXDIRECTIONS;++d) {
        if (!r->connect[d]) {
            int x = r->x, y = r->y;
            transform(&x, &y, d);
            connecttothis(r, x, y, d, d ^ 1);
        }
    }
}
void connectregions(void)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        connectregion(r);
    }
}

char newblock[BLOCKSIZE][BLOCKSIZE];

void transmute(int from, int to, int n, int count)
{
    int i, x, y;

    do {
        i = 0;

        do {
            x = genrand_int31() % BLOCKSIZE;
            y = genrand_int31() % BLOCKSIZE;
            i += count;
        }
        while (i <= 10 &&
               !(newblock[x][y] == from &&
                 ((x != 0 && newblock[x - 1][y] == to) ||
                  (x != BLOCKSIZE - 1 && newblock[x + 1][y] == to) ||
                  (y != 0 && newblock[x][y - 1] == to) ||
                  (y != BLOCKSIZE - 1 && newblock[x][y + 1] == to))));

        if (i > 10)
            break;

        newblock[x][y] = (char) to;
    }
    while (--n);
}

void seed(terrain_t to, int n)
{
    int x, y;

    do {
        x = genrand_int31() % BLOCKSIZE;
        y = genrand_int31() % BLOCKSIZE;
    }
    while (newblock[x][y] != T_PLAIN);

    newblock[x][y] = (char) to;
    transmute(T_PLAIN, to, n, 1);
}

bool regionnameinuse(const char *s)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        const char * rname = region_getname(r);
        if (rname && strcmp(rname, s)==0)
            return true;
    }
    return false;
}

int blockcoord(int x)
{
    return (x / (BLOCKSIZE + BLOCKBORDER * 2)) * (BLOCKSIZE +
                                                  BLOCKBORDER * 2);
}

void initregion(region *r) {
    if (r->terrain != T_OCEAN) {
        int i, n = 0;
        ql_iter rli;

        for (rli = qli_init(&regions); qli_more(rli);) {
            region *r2 = (region *)qli_next(&rli);
            const char * rname = region_getname(r2);
            if (rname)
                n++;
        }
        i = genrand_int31() % (sizeof regionnames / sizeof(char *));
        if (n < sizeof regionnames / sizeof(char *))
            while (regionnameinuse(regionnames[i]))
                i = genrand_int31() % (sizeof regionnames /
                                sizeof(char *));

        region_setname(r, regionnames[i]);
        r->peasants = maxfoodoutput[r->terrain] / 50;
        r->money = r->peasants * 3 / 2;
    }
}

faction * autoplayer(int bx, int by, int no, const char * email, const char * name)
{
    int d, x, y, maxtries = 10;
    do {
        x = genrand_int31() % BLOCKSIZE;
        y = genrand_int31() % BLOCKSIZE;
    } while (--maxtries && newblock[x][y]!=T_OCEAN);
    if (maxtries) {
        faction * f;
        region * r;

        newblock[x][y] = T_FOREST;
        r = create_region(bx + x, by + y, T_FOREST);
        initregion(r);
        connectregion(r);
        f = addplayer(r, email, no);
        for (d=0;d!=MAXDIRECTIONS;++d) {
            region * rc = r->connect[d];
            if (!rc) {
                int cx = r->x, cy = r->y;
                terrain_t t = (terrain_t)(genrand_int31() % NUMTERRAINS);
                transform(&cx, &cy, d);
                rc = create_region(cx, cy, t);
                r->connect[d] = rc;
                initregion(rc);
            }
        }
        if (name) {
            faction_setname(f, name);
        }
        return f;
    }
    return 0;
}

void autoblock(int bx, int by)
{
    int x, y;
    for (x = 0; x != BLOCKSIZE; ++x) {
        for (y = 0; y != BLOCKSIZE; ++y) {
            int rx = BLOCKBORDER + bx * BLOCKSIZE + x;
            int ry = BLOCKBORDER + by * BLOCKSIZE + y;
            region * r = findregion(rx, ry);
            if (!r) {
                terrain_t t = (terrain_t)newblock[x][y];
                r = create_region(rx, ry, t);
                initregion(r);
            }
        }
    }
}

int autoworld(const char * playerfile)
{
    char *name, *email;
    FILE *F = fopen(playerfile, "r");
    int no = 1, bx = 0, by = 0, block = 0;
    char buf[1024];

    if (!F) {
        return -1;
    }
    memset(newblock, T_OCEAN, sizeof newblock);
    while (!feof(F)) {
        int perblock = MAXPERBLOCK;
        while (fgets(buf, sizeof(buf), F)) {
            email = strtok(buf, ";\n");
            if (email) {
                name = strtok(0, ";\n");
                for (;;) {
                    if (perblock-- && autoplayer(BLOCKBORDER + bx * BLOCKSIZE, BLOCKBORDER + by * BLOCKSIZE, no, email, name)) {
                        ++no;
                        ++block;
                        break;
                    }
                    perblock = MAXPERBLOCK;
                    autoblock(bx, by);
                    memset(newblock, T_OCEAN, sizeof newblock);
                    do {
                        block = 0;
                        if (bx>0) {
                            --bx;
                            ++by;
                        } else {
                            bx = by+1;
                            by = 0;
                        }
                    } while (bx>=BLOCKMAX || by>=BLOCKMAX);
                }
            }
        }
    }
    if (block) {
        autoblock(bx, by);
    }
    fclose(F);
    return 0;
}

void makeblock(int x1, int y1)
{
    int x, y;
    region *r;

    if (x1 < 0)
        while (x1 != blockcoord(x1))
            x1--;

    if (y1 < 0)
        while (y1 != blockcoord(y1))
            y1--;

    x1 = blockcoord(x1);
    y1 = blockcoord(y1);

    memset(newblock, T_OCEAN, sizeof newblock);
    newblock[BLOCKSIZE / 2][BLOCKSIZE / 2] = T_PLAIN;
    transmute(T_OCEAN, T_PLAIN, 31, 0);
    seed(T_MOUNTAIN, 1);
    seed(T_MOUNTAIN, 1);
    seed(T_FOREST, 1);
    seed(T_FOREST, 1);
    seed(T_SWAMP, 1);
    seed(T_SWAMP, 1);

    for (x = 0; x != BLOCKSIZE + BLOCKBORDER * 2; x++) {
        for (y = 0; y != BLOCKSIZE + BLOCKBORDER * 2; y++) {
            int t = T_OCEAN;

            if (x >= BLOCKBORDER && x < BLOCKBORDER + BLOCKSIZE &&
                y >= BLOCKBORDER && y < BLOCKBORDER + BLOCKSIZE) {
                t = newblock[x - BLOCKBORDER][y - BLOCKBORDER];
            }
            r = create_region(x1 + x, y1 + y, (terrain_t)t);
            initregion(r);
        }
    }
}

const char *gamedate(void)
{
    static char buf[40];
    static char *monthnames[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
    };

    if (turn == 0)
        strcpy(buf, "In the Beginning");
    else
        sprintf(buf, "%s, Year %d", monthnames[(turn - 1) % 12],
                ((turn - 1) / 12) + 1);
    return buf;
}

void sparagraph(strlist ** SP, const char *s, int indent, int mark)
{
    int i, j, width;
    int firstline;
    static char buf[128];

    width = 79 - indent;
    firstline = 1;

    for (;;) {
        i = 0;

        do {
            j = i;
            while (s[j] && s[j] != ' ')
                j++;
            if (j > width)
                break;
            i = j + 1;
        }
        while (s[j]);

        for (j = 0; j != indent; j++)
            buf[j] = ' ';

        if (firstline && mark && indent > 2)
            buf[indent - 2] = (char) mark;

        for (j = 0; j != i - 1; j++)
            buf[indent + j] = s[j];
        buf[indent + j] = 0;

        addstrlist(SP, buf);

        if (s[i - 1] == 0)
            break;

        s += i;
        firstline = 0;
    }
}

void spskill(const unit * u, int i, int *dh, int days)
{
    if (!u->skills[i])
        return;

    scat(", ");

    if (!*dh) {
        scat("skills: ");
        *dh = 1;
    }

    scat(skillnames[i]);
    scat(" ");
    icat(effskill(u, i));

    if (days) {
        assert(u->number);
        scat(" [");
        icat(u->skills[i] / u->number);
        scat("]");
    }
}

void spunit(strlist ** SP, const faction * f, region * r, const unit * u, int indent,
            bool battle)
{
    const char * sc;
    int i;
    int dh;

    strcpy(buf, unitid(u));

    if (battle || cansee(f, r, u) == 2) {
        scat(", faction ");
        scat(factionid(u->faction));
    }

    if (u->number != 1) {
        scat(", number: ");
        icat(u->number);
    }

    if (u->behind && (battle || u->faction == f))
        scat(", behind");

    if (u->guard)
        scat(", on guard");

    if (u->faction == f && !battle && u->money) {
        scat(", $");
        icat(u->money);
    }

    dh = 0;

    if (battle)
        for (i = SK_TACTICS; i <= SK_LONGBOW; i++)
            spskill(u, i, &dh, 0);
    else if (u->faction == f)
        for (i = 0; i != MAXSKILLS; i++)
            spskill(u, i, &dh, 1);

    dh = 0;

    for (i = 0; i != MAXITEMS; i++)
        if (u->items[i]) {
            scat(", ");

            if (!dh) {
                scat("has: ");
                dh = 1;
            }

            if (u->items[i] == 1)
                scat(itemnames[i][0]);
            else {
                icat(u->items[i]);
                scat(" ");
                scat(itemnames[i][1]);
            }
        }

    if (u->faction == f) {
        dh = 0;

        for (i = 0; i != MAXSPELLS; i++)
            if (u->spells[i]) {
                scat(", ");

                if (!dh) {
                    scat("spells: ");
                    dh = 1;
                }

                scat(spellnames[i]);
            }

        if (!battle) {
            scat(", default: \"");
            scat(u->lastorder);
            scat("\"");
        }

        if (u->combatspell >= 0) {
            scat(", combat spell: ");
            scat(spellnames[u->combatspell]);
        }
    }

    i = 0;

    sc = unit_getdisplay(u);
    if (sc) {
        scat("; ");
        scat(sc);
        i = sc[strlen(sc) - 1];
    }

    if (i != '!' && i != '?')
        scat(".");

    sparagraph(SP, buf, indent, (u->faction == f) ? '*' : '-');
}

void reportevent(region * r, char *s)
{
    ql_iter fli;

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        unit *u;
        for (u = r->units_;u;u=u->next) {
            if (u->faction == f && u->number) {
                addevent(f, s);
                break;
            }
        }
    }
}

void leave(region * r, unit * ux)
{
    building *b;
    ship *sh;

    if (ux->building) {
        b = ux->building;
        ux->building = 0;

        if (ux->owner) {
            unit *u;
            ux->owner = false;

            for (u = r->units_;u;u=u->next) {
                if (u->faction == u->faction && u->building == b) {
                    u->owner = true;
                    return;
                }
            }
            for (u = r->units_;u;u=u->next) {
                if (u->building == b) {
                    u->owner = true;
                    return;
                }
            }
        }
    }

    if (ux->ship) {
        sh = ux->ship;
        ux->ship = 0;

        if (ux->owner) {
            unit *u;
            ux->owner = false;

            for (u = r->units_;u;u=u->next) {
                if (u->faction == ux->faction && u->ship == sh) {
                    u->owner = true;
                    return;
                }
            }
            for (u = r->units_;u;u=u->next) {
                if (u->ship == sh) {
                    u->owner = true;
                    return;
                }
            }
        }
    }
}

void removeempty(void)
{
    int i;
    ql_iter rli, uli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        for (u = r->units_;u;u=u->next) {
            if (!u->number) {
                ql_iter qli;

                unit *u3;
                for (u3 = r->units_;u3;u3=u3->next) {
                    if (u3->faction == u->faction) {
                        u3->money += u->money;
                        u->money = 0;
                        for (i = 0; i != MAXITEMS; i++)
                            u3->items[i] += u->items[i];
                        break;
                    }
                }
                if (r->terrain != T_OCEAN) {
                    r->money += u->money;
                }
                leave(r, u);
                free_unit(u);
                qli_delete(&uli);
            } else {
                qli_next(&uli);
            }
        }

        if (r->terrain == T_OCEAN) {
            ql_iter sli;
            for (sli = qli_init(&r->ships); qli_more(sli);) {
                ship *sh = (ship *)qli_get(sli);
                unit *u;

                for (u=r->units_;u;u=u->next) {
                    if (u->ship == sh) {
                        break;
                    }
                }
                if (!u) {
                    free_ship(sh);
                    qli_delete(&sli);
                } else {
                    qli_next(&sli);
                }
            }
        }
    }
}

void destroyfaction(faction * f)
{
    ql_iter rli;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;

        for (u=r->units_;u;u=u->next) {
            if (u->faction == f) {
                if (r->terrain != T_OCEAN)
                    r->peasants += u->number;

                u->number = 0;
            }
        }
    }
}

void togglerf(unit * u, const char *s, quicklist ** ql, faction *f)
{
    if (f) {
        if (f != u->faction) {
            if (!ql_set_remove(ql, f)) {
                ql_set_insert(ql, f);
            }
        }
    } else {
        mistakes(u, s, "Faction not found");
    }
}

bool iscoast(region * r)
{
    int i;

    for (i = 0; i != MAXDIRECTIONS; i++)
        if (r->connect[i]->terrain == T_OCEAN)
            return true;

    return false;
}

int distribute(int old, int nyu, int n)
{
    int i;
    int t;

    assert(nyu <= old);

    if (old == 0)
        return 0;

    t = (n / old) * nyu;
    for (i = (n % old); i; i--)
        if (genrand_int31() % old < nyu)
            t++;

    return t;
}

int armedmen(unit * u)
{
    int n;

    n = 0;
    if (effskill(u, SK_SWORD))
        n += u->items[I_SWORD];
    if (effskill(u, SK_CROSSBOW))
        n += u->items[I_CROSSBOW];
    if (effskill(u, SK_LONGBOW))
        n += u->items[I_LONGBOW];
    return MIN(n, u->number);
}

void getmoney(region * r, unit * ux, int n)
{
    int i;
    unit *u;

    n -= ux->money;

    for (u=r->units_;u;u=u->next) {
        if (u->faction == ux->faction && u != ux) {
            i = MIN(u->money, n);
            u->money -= i;
            ux->money += i;
            n -= i;
        }
    }
}

bool isallied(const unit * u, const unit * u2)
{
    if (!u2)
        return u->guard;

    if (u->faction == u2->faction)
        return true;

    return !!ql_set_find(&u->faction->allies.factions, 0, u2->faction);
}

bool accepts(const unit * u, const unit * u2)
{
    if (isallied(u, u2)) {
        return true;
    }
    return !!ql_set_find(&u->faction->accept, 0, u2->faction);
}

bool admits(const unit * u, const unit * u2)
{
    if (isallied(u, u2))
        return true;

    return !!ql_set_find(&u->faction->admit, 0, u2->faction);
}

unit *buildingowner(region * r, const building * b)
{
    unit *u, *res = 0;

    for (u=r->units_;u;u=u->next) {
        if (u->building == b) {
            if (u->owner) {
                return u;
            }
            res = u;
        }
    }
    return res;
}

unit *shipowner(region * r, const ship * sh)
{
    unit *res = 0;
    unit *u;

    for (u=r->units_;u;u=u->next) {
        if (u->ship == sh) {
            if (u->owner) {
                return u;
            }
            res = u;
        }
    }
    return res;
}

bool mayenter(region * r, const unit * u, const building * b)
{
    unit *u2;

    u2 = buildingowner(r, b);
    return u2 == 0 || admits(u2, u);
}

bool mayboard(region * r, unit * u, ship * sh)
{
    unit *u2;

    u2 = shipowner(r, sh);
    return u2 == 0 || admits(u2, u);
}

int getbuf(stream * strm, char *buf, size_t size)
{
    int err;
    do {
        err = strm->api->readln(strm->handle, buf, size);
    } while (err==0 && buf[0]=='#');
    return err;
}

void read_orders(stream * strm)
{
    int i, j;
    faction *f;
    unit *u;
    char buf[1024];

    while (getbuf(strm, buf, sizeof(buf))==0) {
        int kwd = igetkeyword(buf);
        const char * passwd;
        if (kwd == K_FACTION) {
            bool check;
NEXTPLAYER:
            i = atoi(getstr());
            f = findfaction(i);
            if (!f) {
                printf("Invalid faction %d.\n", i);
                continue;
            }
            passwd = getstr();
            if (passwd[0]=='\"') {
              strcpy(buf2, passwd+1);
              passwd = strtok(buf2, "\"");
            }
            check = faction_checkpassword(f, passwd);
            if (ignore_password && !check) {
                faction_setpassword(f, passwd);
            }
            if (!faction_checkpassword(f, passwd)) {
                sprintf(buf2, "Invalid password '%s'.", passwd);
                ql_push(&f->mistakes, _strdup(buf2));
            } else {
                ql_iter rli;
                f->lastorders = turn;

                for (rli = qli_init(&regions); qli_more(rli);) {
                    region *r = (region *)qli_next(&rli);
                    unit *u;
                    for (u=r->units_;u;u=u->next) {
                        if (u->faction == f) {
                            ql_free(u->orders);
                            u->orders = 0;
                        }
                    }
                }
                for (;;) {
                    int kwd;
                    if (getbuf(strm, buf, sizeof(buf))!=0) {
                        break;
                    }
                    kwd = igetkeyword(buf);
                    if (kwd == K_FACTION) {
                        goto NEXTPLAYER;
                    }
                    if (kwd == K_UNIT) {
NEXTUNIT:
                        i = atoi(getstr());
                        u = (i>0) ? findunitg(i) : 0;

                        if (u && u->faction == f) {

                            for (;;) {
                                if (getbuf(strm, buf, sizeof(buf))!=0) {
                                    break;
                                } else {
                                    kwd = igetkeyword(buf);
                                    if (kwd == K_UNIT) {
                                        goto NEXTUNIT;
                                    }

                                    if (kwd == K_FACTION) {
                                        goto NEXTPLAYER;
                                    }

                                    i = 0;
                                    j = 0;

                                    for (;;) {
                                        while (buf[i] == ' ' || buf[i] == '\t')
                                            i++;

                                        if (buf[i] == 0 || buf[i] == ';')
                                            break;

                                        if (buf[i] == '"') {
                                            i++;

                                            for (;;) {
                                                while (buf[i] == '_' ||
                                                       buf[i] == ' ' ||
                                                       buf[i] == '\t')
                                                    i++;

                                                if (buf[i] == 0 ||
                                                    buf[i] == '"')
                                                    break;

                                                while (buf[i] != 0 &&
                                                       buf[i] != '"' &&
                                                       buf[i] != '_' &&
                                                       buf[i] != ' ' &&
                                                       buf[i] != '\t')
                                                    buf2[j++] = buf[i++];

                                                buf2[j++] = '_';
                                            }

                                            if (buf[i] != 0)
                                                i++;

                                            if (j && (buf2[j - 1] == '_'))
                                                j--;
                                        } else {
                                            for (;;) {
                                                while (buf[i] == '_')
                                                    i++;

                                                if (buf[i] == 0 ||
                                                    buf[i] == ';' ||
                                                    buf[i] == '"' ||
                                                    buf[i] == ' ' ||
                                                    buf[i] == '\t')
                                                    break;

                                                while (buf[i] != 0 &&
                                                       buf[i] != ';' &&
                                                       buf[i] != '"' &&
                                                       buf[i] != '_' &&
                                                       buf[i] != ' ' &&
                                                       buf[i] != '\t')
                                                    buf2[j++] = buf[i++];

                                                buf2[j++] = '_';
                                            }

                                            if (j && (buf2[j - 1] == '_'))
                                                j--;
                                        }

                                        buf2[j++] = ' ';
                                    }

                                    if (j) {
                                        buf2[j - 1] = 0;
                                        ql_push(&u->orders, _strdup(buf2));
                                    }
                                }
                            }
                        } else {
                            sprintf(buf2, "%s %d", keywords[kwd], i);
                            mistake(f, buf2, "unit is not one of yours.");
                        }
                    }
                }
            }
        }

    }

}

void update_world(int minx, int miny, int maxx, int maxy) {

    config.width = maxx-minx+1;
    config.height = maxy-miny+1;
    if (minx!=0 || miny!=0) {
        ql_iter fli;
        ql_iter rli;
        for (rli = qli_init(&regions); qli_more(rli);) {
            region *r = (region *)qli_next(&rli);
            r->x -= minx;
            r->y -= miny;
        }
        for (fli = qli_init(&factions); qli_more(fli);) {
            faction *f = (faction *)qli_next(&fli);
            f->origin_x -= minx;
            f->origin_y -= miny;
        }
    }
}

void makeworld(void)
{
    int x, y, minx, miny, maxx, maxy;
    ql_iter rli;

    minx = INT_MAX;
    maxx = INT_MIN;
    miny = INT_MAX;
    maxy = INT_MIN;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        minx = MIN(minx, r->x);
        maxx = MAX(maxx, r->x);
        miny = MIN(miny, r->y);
        maxy = MAX(maxy, r->y);
    }

    for (x=minx-BLOCKBORDER;x<=maxx+BLOCKBORDER;++x) {
        for (y=miny-BLOCKBORDER;y<=maxy+BLOCKBORDER;++y) {
            region * r = findregion(x, y);
            if (!r) {
                r = create_region(x, y, T_OCEAN);
            }
        }
    }
    update_world(minx, miny, maxx, maxy);
    connectregions();
}

void writemap(FILE * F)
{
    int x, y, minx, miny, maxx, maxy;
    char buf[1024];
    ql_iter rli;

    minx = INT_MAX;
    maxx = INT_MIN;
    miny = INT_MAX;
    maxy = INT_MIN;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        minx = MIN(minx, r->x);
        maxx = MAX(maxx, r->x);
        miny = MIN(miny, r->y);
        maxy = MAX(maxy, r->y);
    }

    for (y = miny; y <= maxy; y++) {
        memset(buf, ' ', sizeof buf);
        buf[maxx - minx + 1] = 0;

        for (rli = qli_init(&regions); qli_more(rli);) {
            region *r = (region *)qli_next(&rli);
            if (r->y == y)
                buf[r->x - minx] = ".+MFS"[r->terrain];
        }
        for (x = 0; buf[x]; x++) {
            fputc(' ', F);
            fputc(buf[x], F);
        }

        fputc('\n', F);
    }
}

void writesummary(void)
{
    FILE * F;
    int inhabitedregions;
    int peasants;
    int peasantmoney;
    int nunits;
    int playerpop;
    int playermoney;
    ql_iter fli;
    ql_iter rli;

    F = cfopen("summary", "w");
    puts("Writing summary file...");

    inhabitedregions = 0;
    peasants = 0;
    peasantmoney = 0;

    nunits = 0;
    playerpop = 0;
    playermoney = 0;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        if (r->peasants || r->units_) {
            unit *u;
            inhabitedregions++;
            peasants += r->peasants;
            peasantmoney += r->money;

            for (u=r->units_;u;u=u->next) {
                nunits++;
                playerpop += u->number;
                playermoney += u->money;

                u->faction->nunits++;
                u->faction->number += u->number;
                u->faction->money += u->money;
            }
        }
    }
    fprintf(F, "Summary file for Atlantis, %s\n\n", gamedate());

    fprintf(F, "Regions: %d\n", ql_length(regions));
    fprintf(F, "Inhabited Regions: %d\n\n", inhabitedregions);

    fprintf(F, "Factions: %d\n", ql_length(factions));
    fprintf(F, "Units: %d\n\n", nunits);

    fprintf(F, "Player Population: %d\n", playerpop);
    fprintf(F, "Peasants: %d\n", peasants);
    fprintf(F, "Total Population: %d\n\n", playerpop + peasants);

    fprintf(F, "Player Wealth: $%d\n", playermoney);
    fprintf(F, "Peasant Wealth: $%d\n", peasantmoney);
    fprintf(F, "Total Wealth: $%d\n\n", playermoney + peasantmoney);

    writemap(F);

    if (factions)
        fputc('\n', F);

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        fprintf(F, "%s, units: %d, number: %d, $%d, address: %s, loc: %d,%d",
        factionid(f), f->nunits, f->number, f->money, faction_getaddr(f), f->origin_x, f->origin_y);
        if (f->lastorders==turn) fputc('\n', F);
        else fputs(", nmr\n",  F);
    }
    fclose(F);
}

int outi;
char outbuf[256];

void rnl(FILE * F)
{
    int i;
    int rc, vc;

    i = outi;
    while (i && isspace(outbuf[i - 1]))
        i--;
    outbuf[i] = 0;

    i = 0;
    rc = 0;
    vc = 0;

    while (outbuf[i]) {
        switch (outbuf[i]) {
        case ' ':
            vc++;
            break;

        case '\t':
            vc = (vc & ~7) + 8;
            break;

        default:
            while (rc / 8 != vc / 8) {
                if ((rc & 7) == 7)
                    fputc(' ', F);
                else
                    fputc('\t', F);
                rc = (rc & ~7) + 8;
            }

            while (rc != vc) {
                fputc(' ', F);
                rc++;
            }

            fputc(outbuf[i], F);
            rc++;
            vc++;
        }

        i++;
    }

    fputc('\n', F);
    outi = 0;
}

void rpc(int c)
{
    outbuf[outi++] = (char) c;
    assert(outi < sizeof outbuf);
}

void rps(const char *s)
{
    while (*s)
        rpc(*s++);
}

void centre(FILE * F, const char *s)
{
    int i;

    for (i = (79 - strlen(s)) / 2; i; i--)
        rpc(' ');
    rps(s);
    rnl(F);
}

void rpstrlist(FILE * F, strlist * S)
{
    while (S) {
        rps(S->s);
        rnl(F);
        S = S->next;
    }
}

void centrestrlist(FILE * F, const char *s, strlist * S)
{
    if (S) {
        rnl(F);
        centre(F, s);
        rnl(F);

        rpstrlist(F, S);
    }
}

void centreqstrlist(FILE * F, const char *s, quicklist *ql)
{
    strlist *S = 0;
    ql_iter qli;

    for (qli = qli_init(&ql);qli_more(qli);) {
        const char * str = (const char *)qli_next(&qli);
        sparagraph(&S, str, 0, 0);
    }
    centrestrlist(F, s, S);
    freestrlist(S);
}

void rparagraph(FILE * F, char const *s, int indent, int mark)
{
    strlist *S;

    S = 0;
    sparagraph(&S, s, indent, mark);
    rpstrlist(F, S);
    freestrlist(S);
}

void rpunit(FILE * F, const faction * f, region * r, const unit * u, int indent, bool battle)
{
    strlist *S;

    S = 0;
    spunit(&S, f, r, u, indent, battle);
    rpstrlist(F, S);
    freestrlist(S);
}

void report(faction * f)
{
    FILE * F;
    int i;
    int dh;
    int anyunits;
    ql_iter rli;

    sprintf(buf, "reports/%d-%d.r", turn, f->no);
    F = cfopen(buf, "w");

    printf("Writing report for %s...\n", factionid(f));

    centre(F, "Atlantis Turn Report");
    centre(F, factionid(f));
    centre(F, gamedate());

    centreqstrlist(F, "Mistakes", f->mistakes);
    centreqstrlist(F, "Messages", f->messages);

    if (f->battles || f->events) {
        ql_iter iter;
        rnl(F);
        centre(F, "Events During Turn");
        rnl(F);

        for (iter = qli_init(&f->battles); qli_more(iter); ) {
            battle * b = (battle *)qli_next(&iter);
            ql_iter qli = qli_init(&b->events);
            int i;
            char * s = (char *)qli_next(&qli);
            rps(s);
            rnl(F);
            rps("");
            rnl(F);
            for (i = 0; i!=2; ++i) {
                ql_iter uli;
                for (uli=qli_init(b->units+i);qli_more(uli);) {
                    unit *u = (unit *)qli_next(&uli);
                    rpunit(F, f, b->region, u, 4, true);
                }
                rps("");
                rnl(F);
            }
            while (qli_more(qli)) {
                strlist *S = 0;
                char * s = (char *)qli_next(&qli);
                sparagraph(&S, s, 0, 0);
                rpstrlist(F, S);
                rnl(F);
            }
        }

        if (f->battles && f->events)
            rnl(F);

        centreqstrlist(F, "Events", f->events);
    }

    for (i = 0; i != MAXSPELLS; i++)
        if (f->showdata[i])
            break;

    if (i != MAXSPELLS) {
        rnl(F);
        centre(F, "Spells Acquired");

        for (i = 0; i != MAXSPELLS; i++)
            if (f->showdata[i]) {
                rnl(F);
                centre(F, spellnames[i]);
                sprintf(buf, "Level %d", spelllevel[i]);
                centre(F, buf);
                rnl(F);

                rparagraph(F, spelldata[i], 0, 0);
            }
    }

    rnl(F);
    centre(F, "Current Status");

    if (f->allies.factions) {
        ql_iter qli;
        dh = 0;
        strcpy(buf, "You are allied to ");

        for (qli=qli_init(&f->allies.factions); qli_more(qli); ) {
            faction *rf = (faction *)qli_next(&qli);
            if (dh)
                scat(", ");
            dh = 1;
            scat(factionid(rf));
        }

        scat(".");
        rnl(F);
        rparagraph(F, buf, 0, 0);
    }

    anyunits = 0;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u = 0;
        int d;
        ql_iter qli;

        for (u=r->units_;u;u=u->next) {
            if (u->faction == f)
                break;
            u = 0;
        }
        if (!u) continue;

        anyunits = 0;
        sprintf(buf, "%s, %s", regionid(r, f), terrainnames[r->terrain]);
        for (d=0;d!=MAXDIRECTIONS;++d) {
            if (r->connect[d] && r->connect[d]->terrain!=T_OCEAN) {
                if (!anyunits) {
                    anyunits = 1;
                    scat(", exits: ");
                } else {
                    scat(", ");
                }
                scat(keywords[directions[d]]);
            }
        }
        scat(".");
        anyunits = 1;
        if (r->peasants) {
            scat(" peasants: ");
            icat(r->peasants);

            if (r->money) {
                scat(", $");
                icat(r->money);
            }
        }

        scat(".");
        rnl(F);
        rparagraph(F, buf, 0, 0);

        dh = 0;

        for (qli=qli_init(&r->buildings);qli_more(qli);) {
            building *b = (building *)qli_next(&qli);
            unit *u;
            sprintf(buf, "%s, size %d", buildingid(b), b->size);

            if (building_getdisplay(b)) {
                scat("; ");
                scat(building_getdisplay(b));
            }

            scat(".");

            if (dh)
                rnl(F);

            dh = 1;

            rparagraph(F, buf, 4, 0);

            for (u=r->units_;u;u=u->next) {
                if (u->building == b && u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                    break;
                }
            }
            for (u=r->units_;u;u=u->next) {
                if (u->building == b && !u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                }
            }
        }

        for (qli = qli_init(&r->ships); qli_more(qli);) {
            ship *sh = (ship *)qli_next(&qli);

            sprintf(buf, "%s, %s", shipid(sh), shiptypenames[sh->type]);
            if (sh->left)
                scat(", under construction");

            if (ship_getdisplay(sh)) {
                scat("; ");
                scat(ship_getdisplay(sh));
            }

            scat(".");

            if (dh)
                rnl(F);

            dh = 1;

            rparagraph(F, buf, 4, 0);

            for (u=r->units_;u;u=u->next) {
                if (u->ship == sh && u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                    break;
                }
            }

            for (u=r->units_;u;u=u->next) {
                if (u->ship == sh && !u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                }
            }
        }

        dh = 0;

        for (u=r->units_;u;u=u->next) {
            if (!u->building && !u->ship && cansee(f, r, u)) {
                if (!dh && (r->buildings || r->ships)) {
                    rnl(F);
                    dh = 1;
                }

                rpunit(F, f, r, u, 4, 0);
            }
        }
    }

    if (!anyunits) {
        rnl(F);
        rparagraph(F, "Unfortunately your faction has been wiped out. Please "
                   "contact the moderator if you wish to play again.", 0,
                   0);
    }

    fclose(F);
}

int norders;
order *oa;

int scramblecmp(const void *p1, const void *p2)
{
    return *((long *) p1) - *((long *) p2);
}

#define addptr(p,i) ((void *)(((char *)p) + i))

void scramble(void *v1, int n, int width)
{
    int i;
    void *v;

    v = malloc(n * (width + 4));
    if (v) {
        for (i = 0; i != n; i++) {
            *(long *) addptr(v, i * (width + 4)) = genrand_int32();
            memcpy(addptr(v, i * (width + 4) + 4), addptr(v1, i * width),
                   width);
        }

        qsort(v, n, width + 4, scramblecmp);

        for (i = 0; i != n; i++)
            memcpy(addptr(v1, i * width), addptr(v, i * (width + 4) + 4),
                   width);

        free(v);
    }
}

void freeorders(order *orders) {
    order **op;

    for (op = &orders; *op; ) {
        order *o = *op;
        *op = o->next;
        free(o);
    }
}

void expandorders(region * r, order * orders)
{
    ql_iter uli;
    order *o;
	unit *u;

    for (u=r->units_;u;u=u->next) {
        u->n = -1;
    }

    norders = 0;

    for (o = orders; o; o = o->next) {
        norders += o->qty;
    }

    oa = (order *)malloc(norders * sizeof(order));
    if (oa) {
        order **op;
        int i = 0;

        for (op = &orders; *op; ) {
            order *o = *op;
            int j;
            for (j = o->qty; j; j--) {
                oa[i].unit = o->unit;
                oa[i].unit->n = 0;
                i++;
            }
            *op = o->next;
            free(o);
        }
        scramble(oa, norders, sizeof(order));
    }
}

static void remove_ally(faction *f, const faction *f2) {
    if (f != f2) {
        ql_set_remove(&f->allies.factions, f2);
    }
}

void removenullfactions(void)
{
    ql_iter fli;

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction * f = (faction *)qli_get(fli);

        if (!f->alive) {
            printf("Removing %s.\n", faction_getname(f));
            ql_foreachx(factions, (ql_cbx)remove_ally, f);
            free_faction(f);
            qli_delete(&fli);
        } else {
            qli_next(&fli);
        }
    }
}

int itemweight(unit * u)
{
    int i;
    int n;

    n = 0;

    for (i = 0; i != MAXITEMS; i++)
        switch (i) {
        case I_STONE:
            n += u->items[i] * 50;
            break;

        case I_HORSE:
            break;

        default:
            n += u->items[i];
        }

    return n;
}

int horseweight(unit * u)
{
    int i;
    int n;

    n = 0;

    for (i = 0; i != MAXITEMS; i++)
        switch (i) {
        case I_HORSE:
            n += u->items[i] * 50;
            break;
        }

    return n;
}

int canmove(unit * u)
{
    return itemweight(u) - horseweight(u) - (u->number * 5) <= 0;
}

int canride(unit * u)
{
    return itemweight(u) - horseweight(u) + (u->number * 10) <= 0;
}

int cansail(region * r, ship * sh)
{
    int n;
    unit *u;

    n = 0;

    for (u=r->units_;u;u=u->next) {
        if (u->ship == sh)
            n += itemweight(u) + horseweight(u) + (u->number * 10);
    }
    return n <= shipcapacity[sh->type];
}

int spellitem(int i)
{
    if (i < SP_MAKE_AMULET_OF_DARKNESS
        || i > SP_MAKE_WAND_OF_TELEPORTATION)
        return -1;
    return i - SP_MAKE_AMULET_OF_DARKNESS + I_AMULET_OF_DARKNESS;
}

int cancast(unit * u, int i)
{
    if (u->spells[i])
        return u->number;

    if (!effskill(u, SK_MAGIC))
        return 0;

    switch (i) {
    case SP_BLACK_WIND:
        return u->items[I_AMULET_OF_DARKNESS];

    case SP_FIREBALL:
        return u->items[I_STAFF_OF_FIRE];

    case SP_HAND_OF_DEATH:
        return u->items[I_AMULET_OF_DEATH];

    case SP_LIGHTNING_BOLT:
        return u->items[I_STAFF_OF_LIGHTNING];

    case SP_TELEPORT:
        return MIN(u->number, u->items[I_WAND_OF_TELEPORTATION]);
    }

    return 0;
}

int magicians(faction * f)
{
    int n;
    ql_iter rli;

    n = 0;

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
	    for (u=r->units_;u;u=u->next) {
            if (u->skills[SK_MAGIC] && u->faction == f)
                n += u->number;
        }
    }
    return n;
}

region *movewhere(region * r)
{
    int x = r->x, y = r->y;
    int dir, kwd = getkeyword();

    transform(&x, &y, kwd);
    switch (kwd) {
    case K_NORTH:
        dir = 0;
        break;

    case K_SOUTH:
        dir = 1;
        break;

    case K_EAST:
        dir = 2;
        break;

    case K_WEST:
        dir = 3;
        break;
#if MAXDIRECTIONS > 5
    case K_MIR:
        dir = 4;
        break;

    case K_YDD:
        dir = 5;
        break;
#endif
    default:
        dir = -1;
    }

    if (dir>=0) {
        return r->connect[dir];
    }
    return 0;
}

void process_form(unit *u, region *r) {
    ql_iter oli;
    for (oli = qli_init(&u->orders); qli_more(oli); ) {
        char *s = (char *)qli_get(oli);
        unit *u2;

        if (igetkeyword(s) == K_FORM) {
            ql_delete(&oli.l, oli.i);

            while (findunitg(nextunitid)) ++nextunitid;
            u2 = create_unit(u->faction, nextunitid++);
            region_addunit(r, u2, 0, 0);

            u2->alias = atoi(getstr());
            if (u2->alias == 0)
                u2->alias = atoi(getstr());

            u2->building = u->building;
            u2->ship = u->ship;
            u2->behind = u->behind;
            u2->guard = u->guard;

            free(s);
            while (qli_more(oli)) {
                char *s = (char *)qli_get(oli);
                ql_delete(&oli.l, oli.i);
                if (igetkeyword(s) == K_END) {
                    free(s);
                    break;
                } else {
                    ql_push(&u2->orders, s);
                }
            }
        } else {
            qli_next(&oli);
        }
    }
}

static const char * getname(unit * u, const char *ord) {
    const char *s2 = getstr();
    int i;

    if (!s2[0]) {
        mistake(u->faction, ord, "No name given");
        return 0;
    }
    
    for (i = 0; s2[i]; i++) {
        if (s2[i] == '(') {
            mistake(u->faction, ord, "Names cannot contain parentheses");
            return 0;
        }
    }
    return s2;
}

int getseen(region *r, const faction *f, unit **uptr) {
    unit *u = 0;
    int j = getunit(r, f, &u);
    if (u && !cansee(f, r, u)) {
        j = U_NOTFOUND;
        u = 0;
    }
    *uptr = u;
    return j;
}

int lovar(int n)
{
    n /= 2;
    return (genrand_int32() % n + 1) + (genrand_int32() % n + 1);
}

void cmd_stack(unit *u, const char *s) {
    unit *stack;
    
    if (getseen(u->region, u->faction, &stack)==U_NOTFOUND) {
        mistakes(u, s, "Unit not found");
        return;
    }
    if (unit_getstack(u)!=u) {
        unit_unstack(u);
    }
    unit_stack(u, stack);
}

void cmd_unstack(unit *u) {
    unit_unstack(u);
}

void cmd_find(unit *u, const char *s) {
    faction *f;
    f = getfaction();
    if (f == 0) {
        mistakes(u, s, "Faction not found");
        return;
    }
    
    sprintf(buf2, "The address of %s is %s.", factionid(f),
            faction_getaddr(f));
    ql_push(&u->faction->messages, buf2);
}

void processorders(void)
{
    int i, j, k;
    int n, m;
    int taxed;
    int availmoney;
    int teaching;
    char *sx, *sn;
    faction *f;
    ql_iter rli, fli;
    building *b;
    ship *sh;
    unit *u2, *u3;
    static unit *uv[100];
    order *o, *taxorders, *recruitorders, *entertainorders, *workorders;
    static order *produceorders[MAXITEMS];

    /* FORM orders */

    puts("Processing FORM orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        for (u=r->units_;u;u=u->next) {
            process_form(u, r);
        }
    }
    /* Instant orders - diplomacy etc. */

    puts("Processing instant orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
		for (u=r->units_;u;u=u->next) {
            ql_iter oli;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                switch (igetkeyword(s)) {
                case -1:
                    mistakes(u, s, "Order not recognized");
                    break;

                case K_ACCEPT:
                    togglerf(u, s, &u->faction->accept, getfaction());
                    break;
    
                case K_PASSWORD:
                    s = getstr();
                    faction_setpassword(u->faction, s);
                    sprintf(buf2, "The faction's password was changed to '%s'.", s);
                    addmessage(u->faction, buf2);
                    break;

                case K_ADDRESS:
                    s = getstr();

                    if (!s[0]) {
                        mistakes(u, s, "No address given");
                        break;
                    }

                    faction_setaddr(u->faction, s);

                    printf("%s is changing address to %s.\n",
                           faction_getname(u->faction), faction_getaddr(u->faction));
                    break;

                case K_ADMIT:
                    togglerf(u, s, &u->faction->admit, getfaction());
                    break;

                case K_ALLY:
                    f = getfaction();

                    if (f == 0) {
                        mistakes(u, s, "Faction not found");
                        break;
                    }

                    if (f == u->faction)
                        break;

                    if (atoi(getstr())>0) {
                        ql_set_insert(&u->faction->allies.factions, f);
                    } else {
                        ql_set_remove(&u->faction->allies.factions, f);
                    }
                    break;

                case K_BEHIND:
                    u->behind = atoi(getstr()) != 0;
                    break;

                case K_COMBAT:
                    s = getstr();

                    if (!s[0]) {
                        u->combatspell = -1;
                        break;
                    }

                    i = findspell(s);

                    if (i < 0 || !cancast(u, i)) {
                        mistakes(u, s, "Spell not found");
                        break;
                    }

                    if (!iscombatspell[i]) {
                        mistakes(u, s, "Not a combat spell");
                        break;
                    }

                    u->combatspell = i;
                    break;

                case K_DISPLAY:
                    sn = 0;

                    switch (getkeyword()) {
                    case K_BUILDING:
                        if (!u->building) {
                            mistakes(u, s, "Not in a building");
                            break;
                        }

                        if (!u->owner) {
                            mistakes(u, s, "Building not owned by you");
                            break;
                        }

                        building_setdisplay(u->building, getstr());
                        break;

                    case K_SHIP:
                        if (!u->ship) {
                            mistakes(u, s, "Not in a ship");
                            break;
                        }

                        if (!u->owner) {
                            mistakes(u, s, "Ship not owned by you");
                            break;
                        }

                        ship_setdisplay(u->ship, getstr());
                        break;

                    case K_UNIT:
                        unit_setdisplay(u, getstr());
                        break;

                    default:
                        mistakes(u, s, "Order not recognized");
                        break;
                    }

                    if (!sn)
                        break;

                    sx = getstr();

                    i = strlen(sx);
                    if (i && sx[i - 1] == '.')
                        sx[i - 1] = 0;

                    nstrcpy(sn, sx, DISPLAYSIZE);
                    break;

                case K_GUARD:
                    if (atoi(getstr()) == 0)
                        u->guard = false;
                    break;

                case K_NAME:
                    switch (getkeyword()) {
                    case K_BUILDING:
                        if (!u->building) {
                            mistakes(u, s, "Not in a building");
                            break;
                        }

                        if (!u->owner) {
                            mistakes(u, s, "Building not owned by you");
                            break;
                        }

                        building_setname(u->building, getname(u, s));
                        break;

                    case K_FACTION:
                        faction_setname(u->faction, getname(u, s));
                        break;

                    case K_SHIP:
                        if (!u->ship) {
                            mistakes(u, s, "Not in a ship");
                            break;
                        }

                        if (!u->owner) {
                            mistakes(u, s, "Ship not owned by you");
                            break;
                        }

                        ship_setname(u->ship, getname(u, s));
                        break;

                    case K_UNIT:
                        unit_setname(u, getname(u, s));
                        break;

                    default:
                        mistakes(u, s, "Order not recognized");
                        break;
                    }
                    break;

                case K_RESHOW:
                    i = getspell();

                    if (i < 0 || !u->faction->seendata[i]) {
                        mistakes(u, s, "Spell not found");
                        break;
                    }

                    u->faction->showdata[i] = 1;
                    break;
                }
            }
        }
    }
    /* FIND orders */

    puts("Processing FIND orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;

	    for (u=r->units_;u;u=u->next) {
            ql_iter oli;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                switch (igetkeyword(s)) {
                case K_STACK:
                    cmd_stack(u, s);
                    break;
                case K_UNSTACK:
                    cmd_unstack(u);
                    break;
                case K_FIND:
                    cmd_find(u, s);
                    break;
                }
            }
        }
    }
    /* Leaving and entering buildings and ships */

    puts("Processing leaving and entering orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;

		for (u=r->units_;u;u=u->next) {
            ql_iter oli;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);

                switch (igetkeyword(s)) {
                case K_BOARD:
                    sh = getship(r);

                    if (!sh) {
                        mistakes(u, s, "Ship not found");
                        break;
                    }

                    if (!mayboard(r, u, sh)) {
                        mistakes(u, s, "Not permitted to board");
                        break;
                    }

                    leave(r, u);
                    u->ship = sh;
                    u->owner = 0;
                    if (shipowner(r, sh) == 0)
                        u->owner = 1;
                    break;

                case K_ENTER:
                    b = getbuilding(r);

                    if (!b) {
                        mistakes(u, s, "Building not found");
                        break;
                    }

                    if (!mayenter(r, u, b)) {
                        mistakes(u, s, "Not permitted to enter");
                        break;
                    }

                    leave(r, u);
                    u->building = b;
                    u->owner = 0;
                    if (buildingowner(r, b) == 0)
                        u->owner = 1;
                    break;

                case K_LEAVE:
                    if (r->terrain == T_OCEAN) {
                        mistakes(u, s, "Ship is at sea");
                        break;
                    }

                    leave(r, u);
                    break;

                case K_PROMOTE:
                    if (!u->building && !u->ship) {
                        mistakes(u, s, "No building or ship to transfer ownership of");
                        break;
                    }

                    if (!u->owner) {
                        mistakes(u, s, "Not owned by you");
                        break;
                    }

                    if (getseen(r, u->faction, &u2)!=U_UNIT) {
                        mistakes(u, s, "Unit not found");
                        break;
                    }

                    if (!accepts(u2, u)) {
                        mistakes(u, s, "Unit does not accept ownership");
                        break;
                    }

                    if (u->building) {
                        if (u2->building != u->building) {
                            mistakes(u, s, "Unit not in same building");
                            break;
                        }
                    } else if (u2->ship != u->ship) {
                        mistakes(u, s, "Unit not on same ship");
                        break;
                    }

                    u->owner = 0;
                    u2->owner = 1;
                    break;
                }
            }
        }
    }
    process_combat();

    /* Economic orders */

    puts("Processing economic orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        taxorders = 0;
        recruitorders = 0;

        /* DEMOLISH, GIVE, PAY, SINK orders */

        for (u=r->units_;u;u=u->next) {
            ql_iter oli;

            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                unit *u2;
                int i, j, sp;
                switch (igetkeyword(s)) {
                case K_DEMOLISH:
                    if (!u->building) {
                        mistakes(u, s, "Not in a building");
                        break;
                    }

                    if (!u->owner) {
                        mistakes(u, s, "Building not owned by you");
                        break;
                    }

                    b = u->building;

                    for (u2=r->units_;u;u=u->next) {
                        if (u2->building == b) {
                            u2->building = 0;
                            u2->owner = 0;
                        }
                    }
                    sprintf(buf, "%s demolishes %s.", unitid(u),
                            buildingid(b));
                    reportevent(r, buf);

                    removelist(&r->buildings, b);
                    break;

                case K_GIVE:
                    j = getseen(r, u->faction, &u2);

                    if (j!=U_UNIT && j!=U_NONE)  {
                        mistakes(u, s, "Unit not found");
                        break;
                    }

                    if (j==U_UNIT && !accepts(u2, u)) {
                        mistakes(u, s, "Unit does not accept your gift");
                        break;
                    }

                    s = getstr();
                    sp = findspell(s);

                    if (sp >= 0) {
                        if (j!=U_UNIT) {
                            mistakes(u, s, "Unit not found");
                            break;
                        }

                        if (!u->spells[sp]) {
                            mistakes(u, s, "Spell not found");
                            break;
                        }

                        if (spelllevel[sp] > (effskill(u2, SK_MAGIC) + 1) / 2) {
                            mistakes(u, s, "Recipient is not able to learn that spell");
                            break;
                        }

                        u2->spells[sp] = 1;

                        sprintf(buf, "%s gives ", unitid(u));
                        scat(unitid(u2));
                        scat(" the ");
                        scat(spellnames[sp]);
                        scat(" spell.");
                        addevent(u->faction, buf);
                        if (u->faction != u2->faction)
                            addevent(u2->faction, buf);

                        if (!u2->faction->seendata[sp]) {
                            u2->faction->seendata[sp] = true;
                            u2->faction->showdata[sp] = true;
                        }
                    } else {
                        n = atoip(s);
                        i = getitem();

                        if (i < 0) {
                            mistakes(u, s, "Item not recognized");
                            break;
                        }

                        if (n > u->items[i])
                            n = u->items[i];

                        if (n == 0) {
                            mistakes(u, s, "Item not available");
                            break;
                        }

                        u->items[i] -= n;

                        if (j!=U_UNIT) {
                            if (n == 1)
                                sprintf(buf, "%s discards 1 %s.",
                                        unitid(u), itemnames[i][0]);
                            else
                                sprintf(buf, "%s discards %d %s.",
                                        unitid(u), n, itemnames[i][1]);
                            addevent(u->faction, buf);
                            break;
                        }

                        u2->items[i] += n;

                        sprintf(buf, "%s gives ", unitid(u));
                        scat(unitid(u2));
                        scat(" ");
                        if (n == 1) {
                            scat("1 ");
                            scat(itemnames[i][0]);
                        } else {
                            icat(n);
                            scat(" ");
                            scat(itemnames[i][1]);
                        }
                        scat(".");
                        addevent(u->faction, buf);
                        if (u->faction != u2->faction) {
                            addevent(u2->faction, buf);
                        }
                    }

                    break;

                case K_PAY:
                    j = getseen(r, u->faction, &u2);

                    if (j==U_NOTFOUND) {
                        mistakes(u, s, "Unit not found");
                        break;
                    }

                    n = atoi(getstr());

                    if (n > u->money) {
                        n = u->money;
                    }
                    if (n <= 0) {
                        mistakes(u, s, "No money available");
                        break;
                    }

                    u->money -= n;

                    if (u2) {
                        u2->money += n;

                        sprintf(buf, "%s pays ", unitid(u));
                        scat(unitid(u2));
                        scat(" $");
                        icat(n);
                        scat(".");
                        if (u->faction != u2->faction)
                            addevent(u2->faction, buf);
                    } else if (j==U_PEASANTS) {
                        r->money += n;

                        sprintf(buf, "%s pays the peasants $%d.",
                                unitid(u), n);
                    } else {
                        sprintf(buf, "%s discards $%d.", unitid(u), n);
                    }
                    addevent(u->faction, buf);
                    break;

                case K_SINK:
                    if (!u->ship) {
                        mistakes(u, s, "Not on a ship");
                        break;
                    }

                    if (!u->owner) {
                        mistakes(u, s, "Ship not owned by you");
                        break;
                    }

                    if (r->terrain == T_OCEAN) {
                        mistakes(u, s, "Ship is at sea");
                        break;
                    }

                    sh = u->ship;

                    for (qli=qli_init(&r->units);qli_more(qli);) {
                        unit *u2 = (unit *)qli_next(&qli);
                        if (u2->ship == sh) {
                            u2->ship = 0;
                            u2->owner = 0;
                        }
                    }

                    sprintf(buf, "%s sinks %s.", unitid(u), shipid(sh));
                    reportevent(r, buf);

                    removelist(&r->ships, sh);
                    break;
                }
            }
        }

        /* TRANSFER orders */
        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            ql_iter oli;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                int j;

                switch (igetkeyword(s)) {
                case K_TRANSFER:
                    j = getseen(r, u->faction, &u2);

                    if (u2) {
                        if (u->skills[SK_MAGIC] || u2->skills[SK_MAGIC] || !accepts(u2, u)) {
                            mistakes(u, s, "Unit does not accept your gift");
                            break;
                        }
                    } else if (j==U_PEASANTS) {
                        mistakes(u, s, "Unit not found");
                        break;
                    }

                    n = atoi(getstr());

                    if (n > u->number)
                        n = u->number;

                    if (n <= 0) {
                        mistakes(u, s, "No people available");
                        break;
                    }

                    if (u->skills[SK_MAGIC] && u2) {
                        k = magicians(u2->faction);
                        if (u2->faction != u->faction)
                            k += n;
                        if (!u2->skills[SK_MAGIC])
                            k += u2->number;

                        if (k > 3) {
                            mistakes(u, s, "Only 3 magicians per faction");
                            break;
                        }
                    }

                    k = u->number - n;

                    for (i = 0; i != MAXSKILLS; i++) {
                        j = distribute(u->number, k, u->skills[i]);
                        if (u2)
                            u2->skills[i] += u->skills[i] - j;
                        u->skills[i] = j;
                    }

                    u->number = k;

                    if (u2) {
                        u2->number += n;

                        for (i = 0; i != MAXSPELLS; i++)
                            if (u->spells[i]
                                && effskill(u2,
                                            SK_MAGIC) / 2 >= spelllevel[i])
                                u2->spells[i] = 1;

                        sprintf(buf, "%s transfers ", unitid(u));
                        if (k) {
                            icat(n);
                            scat(" ");
                        }
                        scat("to ");
                        scat(unitid(u2));
                        if (u->faction != u2->faction)
                            addevent(u2->faction, buf);
                    } else {
                        r->peasants += n;

                        if (k)
                            sprintf(buf, "%s disbands %d.", unitid(u), n);
                        else
                            sprintf(buf, "%s disbands.", unitid(u));
                    }

                    addevent(u->faction, buf);
                    break;
                }
            }
        }
        /* TAX orders */
        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            ql_iter oli;
            taxed = 0;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                ql_iter qli;
                unit *u2 = 0;

                switch (igetkeyword(s)) {
                case K_TAX:
                    if (taxed)
                        break;

                    n = armedmen(u);

                    if (!n) {
                        mistakes(u, s,
                                 "Unit is not armed and combat trained");
                        break;
                    }

                    for (qli=qli_init(&r->units);qli_more(qli);) {
                        unit *u2 = (unit *)qli_next(&qli);
                        if (u2->guard && u2->number && !admits(u2, u)) {
                            sprintf(buf, "%s is on guard", unit_getname(u2));
                            mistakes(u, s, buf);
                            break;
                        }
                    }
                    if (u2)
                        break;

                    o = (order *)malloc(sizeof(order));
                    o->qty = n * TAXINCOME;
                    o->unit = u;
                    o->next = taxorders;
                    taxorders = o;
                    taxed = 1;
                    break;
                }
            }
        }

        /* Do taxation */

        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            u->n = -1;
        }
        norders = 0;

        for (o = taxorders; o; o = o->next)
            norders += o->qty / 10;

        oa = (order *)malloc(norders * sizeof(order));

        i = 0;

        for (o = taxorders; o; o = o->next) {
            for (j = o->qty / 10; j; j--) {
                oa[i].unit = o->unit;
                oa[i].unit->n = 0;
                i++;
            }
        }
        freeorders(taxorders);

        scramble(oa, norders, sizeof(order));

        for (i = 0; i != norders && r->money > 10; i++, r->money -= 10) {
            oa[i].unit->money += 10;
            oa[i].unit->n += 10;
        }

        free(oa);

        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            if (u->n >= 0) {
                sprintf(buf, "%s collects $%d in taxes.", unitid(u), u->n);
                addevent(u->faction, buf);
            }
        }
        /* GUARD 1, RECRUIT orders */

        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            ql_iter oli;
            availmoney = u->money;

            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                switch (igetkeyword(s)) {
                case K_GUARD:
                    if (atoi(getstr())) {
                        u->guard = true;
                    }
                    break;

                case K_RECRUIT:
                    if (availmoney < RECRUITCOST)
                        break;

                    n = atoi(getstr());
                    if (n<=0) break;

                    if (u->skills[SK_MAGIC] && magicians(u->faction) + n > 3) {
                        mistakes(u, s, "Only 3 magicians per faction");
                        break;
                    }

                    n = MIN(n, availmoney / RECRUITCOST);

                    o = (order *)malloc(sizeof(order));
                    o->qty = n;
                    o->unit = u;
                    o->next = recruitorders;
                    recruitorders = o;

                    availmoney -= o->qty * RECRUITCOST;
                    break;
                }
            }
        }

        /* Do recruiting */
        expandorders(r, recruitorders);

        for (i = 0, n = r->peasants / RECRUITFRACTION; i != norders && n;
             i++, n--) {
            oa[i].unit->number++;
            r->peasants--;
            oa[i].unit->money -= RECRUITCOST;
            r->money += RECRUITCOST;
            oa[i].unit->n++;
        }

        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            if (u->n >= 0) {
                sprintf(buf, "%s recruits %d.", unitid(u), u->n);
                addevent(u->faction, buf);
            }
        }
    }

    /* QUIT orders */

    puts("Processing QUIT orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        ql_iter uli;
        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            ql_iter oli;
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                switch (igetkeyword(s)) {
                case K_QUIT:
                    if (atoi(getstr()) != u->faction->no) {
                        mistakes(u, s, "Correct faction number not given");
                        break;
                    }

                    destroyfaction(u->faction);
                    break;
                }
            }
        }
    }
    /* Remove players who haven't sent in orders */

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (turn - f->lastorders > ORDERGAP) {
            destroyfaction(f);
        }
    }

    /* Set production orders */

    puts("Setting production orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        ql_iter uli;

        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_next(&uli);
            ql_iter oli;
            strcpy(u->thisorder, u->lastorder);
            for (oli = qli_init(&u->orders); qli_more(oli); ) {
                const char *s = (const char *)qli_next(&oli);
                switch (igetkeyword(s)) {
                case K_BUILD:
                case K_CAST:
                case K_ENTERTAIN:
                case K_MOVE:
                case K_PRODUCE:
                case K_RESEARCH:
                case K_SAIL:
                case K_STUDY:
                case K_TEACH:
                case K_WORK:
                    nstrcpy(u->thisorder, s, sizeof u->thisorder);
                    break;
                }
            }

            switch (igetkeyword(u->thisorder)) {
            case K_MOVE:
            case K_SAIL:
                break;

            default:
                strcpy(u->lastorder, u->thisorder);
                _strlwr(u->lastorder);
            }
        }
    }

    /* MOVE orders */

    puts("Processing MOVE orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        ql_iter uli;
        for (uli=qli_init(&r->units);qli_more(uli);) {
            unit *u = (unit *)qli_get(uli);
            region *r2;

            switch (igetkeyword(u->thisorder)) {
            case K_MOVE:
                r2 = movewhere(r);

                if (!r2) {
                    mistakeu(u, "Direction not recognized");
                    qli_next(&uli);
                    break;
                }

                if (r->terrain == T_OCEAN) {
                    mistakeu(u, "Currently at sea");
                    qli_next(&uli);
                    break;
                }

                if (r2->terrain == T_OCEAN) {
                    sprintf(buf, "%s discovers that (%d,%d) is ocean.",
                            unitid(u), r2->x, r2->y);
                    addevent(u->faction, buf);
                    qli_next(&uli);
                    break;
                }

                if (!canmove(u)) {
                    mistakeu(u, "Carrying too much weight to move");
                    qli_next(&uli);
                    break;
                }

                leave(r, u);
                qli_delete(&uli);
                region_addunit(r2, u, 0, 0);
                u->thisorder[0] = 0;

                sprintf(buf, "%s ", unitid(u));
                if (canride(u))
                    scat("rides");
                else
                    scat("walks");
                scat(" from ");
                scat(regionid(r, u->faction));
                scat(" to ");
                scat(regionid(r2, u->faction));
                scat(".");
                addevent(u->faction, buf);
                break;
            default:
                qli_next(&uli);
            }
        }
    }
    /* SAIL orders */

    puts("Processing SAIL orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        for (u=r->units_;u;u=u->next) {
            unit *u2;
            region *r2;
			ql_iter qli;

            switch (igetkeyword(u->thisorder)) {
            case K_SAIL:
                r2 = movewhere(r);

                if (!r2) {
                    mistakeu(u, "Direction not recognized");
                    break;
                }

                if (!u->ship) {
                    mistakeu(u, "Not on a ship");
                    break;
                }

                if (!u->owner) {
                    mistakeu(u, "Ship not owned by you");
                    break;
                }

                if (r2->terrain != T_OCEAN && !iscoast(r2)) {
                    sprintf(buf, "%s discovers that (%d,%d) is inland.",
                            unitid(u), r2->x, r2->y);
                    addevent(u->faction, buf);
                    break;
                }

                if (u->ship->left) {
                    mistakeu(u, "Ship still under construction");
                    break;
                }

                if (!cansail(r, u->ship)) {
                    mistakeu(u, "Too heavily loaded to sail");
                    break;
                }

                
                for (qli=qli_init(&r->ships);qli_more(qli);) {
                    ship *sh = (ship *)qli_get(qli);
                    if (sh==u->ship) {
                        qli_delete(&qli);
                        break;
                    }
                    qli_next(&qli);
                }
                ql_push(&r2->ships, u->ship);

		        for (u2=r->units_;u2;u2=u2->next) {
                    if (u2->ship == u->ship) {
                        qli_delete(&qli);
                        region_addunit(r2, u2, 0, 0);
                        u2->thisorder[0] = 0;
                    } else {
                        qli_next(&qli);
                    }
                }

                u->thisorder[0] = 0;
                break;
            }
            qli_next(&uli);
        }
    }
    /* Do production orders */

    puts("Processing production orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        ship_t stype;
        if (r->terrain == T_OCEAN)
            continue;

        entertainorders = 0;
        workorders = 0;
        memset(produceorders, 0, sizeof produceorders);

        for (u=r->units_;u;u=u->next) {
            switch (igetkeyword(u->thisorder)) {
            case K_BUILD:
                switch (i = getkeyword()) {
                case K_BUILDING:
                    if (!effskill(u, SK_BUILDING)) {
                        mistakeu(u, "You don't have the skill");
                        break;
                    }

                    if (!u->items[I_STONE]) {
                        mistakeu(u, "No stone available");
                        break;
                    }

                    b = getbuilding(r);

                    if (!b) {
                        b = (building *)malloc(sizeof(building));
                        if (b) {
                            memset(b, 0, sizeof(building));

                            do {
                                b->no++;
                                sprintf(buf2, "Building %d", b->no);
                                building_setname(b, buf2);
                            }
                            while (findbuilding(b->no));

                            ql_push(&r->buildings, b);
                        }
                        leave(r, u);
                        u->building = b;
                        u->owner = 1;
                    }

                    if (b) {
                        n = u->number * effskill(u, SK_BUILDING);
                        n = MIN(n, u->items[I_STONE]);
                        b->size += n;
                        u->items[I_STONE] -= n;

                        u->skills[SK_BUILDING] += n * 10;

                        sprintf(buf, "%s adds %d to %s.", unitid(u), n,
                                buildingid(b));
                        addevent(u->faction, buf);
                    }
                    break;

                case K_SHIP:
                    if (!effskill(u, SK_SHIPBUILDING)) {
                        mistakeu(u, "You don't have the skill");
                        break;
                    }

                    if (!u->items[I_WOOD]) {
                        mistakeu(u, "No wood available");
                        break;
                    }

                    sh = getship(r);

                    if (sh == 0) {
                        mistakeu(u, "Ship not found");
                        break;
                    }

                    if (!sh->left) {
                        mistakeu(u, "Ship is already complete");
                        break;
                    }

                  BUILDSHIP:
                    n = u->number * effskill(u, SK_SHIPBUILDING);
                    n = MIN(n, sh->left);
                    n = MIN(n, u->items[I_WOOD]);
                    sh->left -= n;
                    u->items[I_WOOD] -= n;

                    u->skills[SK_SHIPBUILDING] += n * 10;

                    sprintf(buf, "%s adds %d to %s.", unitid(u), n,
                            shipid(sh));
                    addevent(u->faction, buf);
                    break;

                case K_LONGBOAT:
                    stype = SH_LONGBOAT;
                    goto CREATESHIP;

                case K_CLIPPER:
                    stype = SH_CLIPPER;
                    goto CREATESHIP;

                case K_GALLEON:
                    stype = SH_GALLEON;
                    goto CREATESHIP;

                  CREATESHIP:
                    if (!effskill(u, SK_SHIPBUILDING)) {
                        mistakeu(u, "You don't have the skill");
                        break;
                    }

                    n = 0;
                    do {
                        n++;
                    }
                    while (findship(n));

                    sh = create_ship(n, stype);
                    sh->left = shipcost[i];
                    sprintf(buf2, "Ship %d", n);
                    ship_setname(sh, buf2);
                    ql_push(&r->ships, sh);

                    leave(r, u);
                    u->ship = sh;
                    u->owner = 1;
                    goto BUILDSHIP;

                default:
                    mistakeu(u, "Order not recognized");
                }

                break;

            case K_ENTERTAIN:
                o = (order *)malloc(sizeof(order));
                o->unit = u;
                o->qty =
                    u->number * effskill(u,
                                         SK_ENTERTAINMENT) *
                    ENTERTAININCOME;
                o->next = entertainorders;
                entertainorders = o;
                break;

            case K_PRODUCE:
                i = getitem();

                if (i < 0 || i > I_PLATE_ARMOR) {
                    mistakeu(u, "Item not recognized");
                    break;
                }

                n = effskill(u, itemskill[i]);

                if (n == 0) {
                    mistakeu(u, "You don't have the skill");
                    break;
                }

                if (i == I_PLATE_ARMOR)
                    n /= 3;

                n *= u->number;

                if (i < 4) {
                    o = (order *)malloc(sizeof(order));
                    o->unit = u;
                    o->qty = n * productivity[r->terrain][i];
                    o->next = produceorders[i];
                    produceorders[i] = o;
                } else {
                    n = MIN(n, u->items[rawmaterial[i]]);

                    if (n == 0) {
                        mistakeu(u, "No material available");
                        break;
                    }

                    u->items[i] += n;
                    u->items[rawmaterial[i]] -= n;

                    if (n == 1)
                        sprintf(buf, "%s produces 1 %s.", unitid(u),
                                itemnames[i][0]);
                    else
                        sprintf(buf, "%s produces %d %s.", unitid(u), n,
                                itemnames[i][1]);
                    addevent(u->faction, buf);
                }

                u->skills[itemskill[i]] += u->number * PRODUCEEXP;
                break;

            case K_RESEARCH:
                if (effskill(u, SK_MAGIC) < 2) {
                    mistakeu(u, "Magic skill of at least 2 required");
                    break;
                }

                i = atoi(getstr());

                if (i > effskill(u, SK_MAGIC) / 2) {
                    mistakeu(u, "Insufficient Magic skill - highest available level researched");
                    i = 0;
                }

                if (i <= 0) {
                    i = effskill(u, SK_MAGIC) / 2;
                }
                k = 0;

                for (j = 0; j != MAXSPELLS; j++)
                    if (spelllevel[j] == i && !u->spells[j])
                        k = 1;

                if (k == 0) {
                    if (u->money < 200) {
                        mistakeu(u, "Insufficient funds");
                        break;
                    }

                    for (n = u->number; n; n--)
                        if (u->money >= 200) {
                            u->money -= 200;
                            u->skills[SK_MAGIC] += 10;
                        }

                    sprintf(buf,
                            "%s discovers that no more level %d spells exist.",
                            unitid(u), i);
                    addevent(u->faction, buf);
                    break;
                }

                for (n = u->number; n; n--) {
                    if (u->money < 200) {
                        mistakeu(u, "Insufficient funds");
                        break;
                    }

                    do
                        j = genrand_int31() % MAXSPELLS;
                    while (spelllevel[j] != i || u->spells[j] == 1);

                    if (!u->faction->seendata[j]) {
                        u->faction->seendata[j] = true;
                        u->faction->showdata[j] = true;
                    }

                    if (u->spells[j] == 0) {
                        sprintf(buf, "%s discovers the %s spell.",
                                unitid(u), spellnames[j]);
                        addevent(u->faction, buf);
                    }

                    u->spells[j] = 2;
                    u->skills[SK_MAGIC] += 10;
                }

                for (j = 0; j != MAXSPELLS; j++)
                    if (u->spells[j] == 2)
                        u->spells[j] = 1;
                break;

            case K_TEACH:
                teaching = u->number * 30 * TEACHNUMBER;
                m = 0;
                do {
                    j = getseen(r, u->faction, uv+m);
                } while (++m != 100 && j!=U_NONE);
                --m;

                for (j = 0; j != m; j++) {
                    u2 = uv[j];

                    if (!u2) {
                        mistakeu(u, "Unit not found");
                        continue;
                    }

                    if (!accepts(u2, u)) {
                        mistakeu(u, "Unit does not accept teaching");
                        continue;
                    }

                    i = igetkeyword(u2->thisorder);

                    if (i != K_STUDY || (i = getskill()) < 0) {
                        mistakeu(u, "Unit not studying");
                        continue;
                    }

                    if (effskill(u, i) <= effskill(u2, i)) {
                        mistakeu(u,
                                 "Unit not studying a skill you can teach it");
                        continue;
                    }

                    n = (u2->number * 30) - u2->learning;
                    n = MIN(n, teaching);

                    if (n == 0)
                        continue;

                    u2->learning += n;
                    teaching -= u->number * 30;

                    strcpy(buf, unitid(u));
                    scat(" teaches ");
                    scat(unitid(u2));
                    scat(" ");
                    scat(skillnames[i]);
                    scat(".");

                    addevent(u->faction, buf);
                    if (u2->faction != u->faction)
                        addevent(u2->faction, buf);
                }

                break;

            case K_WORK:
                o = (order *)malloc(sizeof(order));
                o->unit = u;
                o->qty = u->number * foodproductivity[r->terrain];
                o->next = workorders;
                workorders = o;
                break;
            }
        }            
        /* Entertainment */

        expandorders(r, entertainorders);
        if (oa) {
            for (i = 0, n = r->money / ENTERTAINFRACTION; i != norders && n;
                 i++, n--) {
                oa[i].unit->money++;
                r->money--;
                oa[i].unit->n++;
            }

            free(oa);
        }
        for (u=r->units_;u;u=u->next) {
            if (u->n >= 0) {
                sprintf(buf, "%s earns $%d entertaining.", unitid(u),
                        u->n);
                addevent(u->faction, buf);

                u->skills[SK_ENTERTAINMENT] += 10 * u->number;
            }
        }
        /* Food production */

        expandorders(r, workorders);
        if (oa) {
            for (i = 0, n = maxfoodoutput[r->terrain]; i != norders && n;
                 i++, n--) {
                oa[i].unit->money++;
                oa[i].unit->n++;
            }

            free(oa);
            r->money += MIN(n, r->peasants * foodproductivity[r->terrain]);
        }

        for (u=r->units_;u;u=u->next) {
            if (u->n >= 0) {
                sprintf(buf, "%s earns $%d performing manual labor.",
                        unitid(u), u->n);
                addevent(u->faction, buf);
            }
        }
        /* Production of other primary commodities */

        for (i = 0; i != 4; i++) {
            unit *u;
            expandorders(r, produceorders[i]);

            for (j = 0, n = maxoutput[r->terrain][i]; j != norders && n;
                 j++, n--) {
                oa[j].unit->items[i]++;
                oa[j].unit->n++;
            }

            free(oa);

            for (u=r->units_;u;u=u->next) {
                if (u->n >= 0) {
                    if (u->n == 1)
                        sprintf(buf, "%s produces 1 %s.", unitid(u),
                                itemnames[i][0]);
                    else
                        sprintf(buf, "%s produces %d %s.", unitid(u), u->n,
                                itemnames[i][1]);
                    addevent(u->faction, buf);
                }
            }
        }
    }

    /* Study skills */

    puts("Processing STUDY orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);

        if (r->terrain != T_OCEAN) {
            unit *u;
            for (u=r->units_;u;u=u->next) {
                switch (igetkeyword(u->thisorder)) {
                case K_STUDY:
                    i = getskill();

                    if (i < 0) {
                        mistakeu(u, "Skill not recognized");
                        break;
                    }

                    if (i == SK_TACTICS || i == SK_MAGIC) {
                        if (u->money < STUDYCOST * u->number) {
                            mistakeu(u, "Insufficient funds");
                            break;
                        }

                        if (i == SK_MAGIC && !u->skills[SK_MAGIC] &&
                            magicians(u->faction) + u->number > 3) {
                            mistakeu(u, "Only 3 magicians per faction");
                            break;
                        }

                        u->money -= STUDYCOST * u->number;
                    }

                    sprintf(buf, "%s studies %s.", unitid(u),
                            skillnames[i]);
                    addevent(u->faction, buf);

                    u->skills[i] += (u->number * 30) + u->learning;
                    break;
                }
            }
        }
    }
    /* Ritual spells, and loss of spells where required */
    puts("Processing CAST orders...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        unit *u;
        for (u=r->units_;u;u=u->next) {
            for (i = 0; i != MAXSPELLS; i++) {
                if (u->spells[i]
                    && spelllevel[i] > (effskill(u, SK_MAGIC) + 1) / 2)
                    u->spells[i] = 0;
            }
            if (u->combatspell >= 0 && !cancast(u, u->combatspell))
                u->combatspell = -1;
        }

        if (r->terrain != T_OCEAN) {
            unit *u;
            for (u=r->units_;u;u=u->next) {
                region *r2;
                unit *u2;
                ql_iter fli;

                switch (igetkeyword(u->thisorder)) {
                case K_CAST:
                    i = getspell();

                    if (i < 0 || !cancast(u, i)) {
                        mistakeu(u, "Spell not found");
                        break;
                    }

                    j = spellitem(i);

                    if (j >= 0) {
                        if (u->money < 200 * spelllevel[i]) {
                            mistakeu(u, "Insufficient funds");
                            break;
                        }

                        n = MIN(u->number,
                                u->money / (200 * spelllevel[i]));
                        u->items[j] += n;
                        u->money -= n * 200 * spelllevel[i];
                        u->skills[SK_MAGIC] += n * 10;

                        sprintf(buf, "%s casts %s.", unitid(u),
                                spellnames[i]);
                        addevent(u->faction, buf);
                        break;
                    }

                    if (u->money < 50) {
                        mistakeu(u, "Insufficient funds");
                        break;
                    }

                    switch (i) {
                    case SP_CONTAMINATE_WATER:
                        n = cancast(u, SP_CONTAMINATE_WATER);
                        n = MIN(n, u->money / 50);

                        u->money -= n * 50;
                        u->skills[SK_MAGIC] += n * 10;

                        n = lovar(n * 50);
                        n = MIN(n, r->peasants);

                        if (!n)
                            break;

                        r->peasants -= n;

                        for (fli = qli_init(&factions); qli_more(fli);) {
                            faction *f = (faction *)qli_next(&fli);

                            j = cansee(f, r, u);

                            if (j) {
                                if (j == 2)
                                    sprintf(buf,
                                            "%s contaminates the water supply "
                                            "in %s, causing %d peasants to die.",
                                            unitid(u), regionid(r, f), n);
                                else
                                    sprintf(buf,
                                            "%d peasants die in %s from "
                                            "drinking contaminated water.",
                                            n, regionid(r, f));
                                addevent(f, buf);
                            }
                        }

                        break;

                    case SP_TELEPORT:
                        u2 = getunitg(r, u->faction);

                        if (!u2) {
                            mistakeu(u, "Unit not found");
                            break;
                        }

                        if (!admits(u2, u)) {
                            mistakeu(u, "Target unit does not provide vector");
                            break;
                        }

                        n = cancast(u, SP_TELEPORT);
                        n = MIN(n, u->money / 50);

                        u->money -= n * 50;
                        u->skills[SK_MAGIC] += n * 10;

                        n *= 10000;

                        for (;;) {
                            unit **up;
							unit *u3 = getunitg(r, u->faction);
                            j = getseen(r, u->faction, &u3);
                            if (!u3) {
                                mistakeu(u, "Unit not found");
                                continue;
                            }
                            if (j!=U_UNIT) {
                                break;
                            }

                            if (!accepts(u3, u)) {
                                mistakeu(u, "Unit does not accept teleportation");
                                continue;
                            }

                            i = itemweight(u3) + horseweight(u3) + (u->number * 10);

                            if (i > n) {
                                mistakeu(u, "Unit too heavy");
                                continue;
                            }

                            n -= i;
                            leave(r, u3);
                            region_rmunit(r, u3, 0);

							u3->building = u2->building;
                            u3->ship = u2->ship;
							region_addunit(u2->region, u3, 0, &u2->next);
							sprintf(buf2, "%s teleports %s to %s.", unitid(u), unitid(u3), regionid(u2->region, u->faction));
							addevent(u->faction, buf2);
                        }

                        break;

                    default:
                        mistakeu(u, "Spell not usable with CAST command");
                    }

                    break;
                }
            }
        }
    }

    /* Population growth, dispersal and food consumption */

    puts("Processing demographics...");

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
		unit *u;

        if (r->terrain != T_OCEAN) {
            for (n = r->peasants; n; n--)
                if (genrand_int31() % 100 < POPGROWTH)
                    r->peasants++;

            n = r->money / MAINTENANCE;
            r->peasants = MIN(r->peasants, n);
            r->money -= r->peasants * MAINTENANCE;

            for (n = r->peasants; n; n--) {
                if (genrand_int31() % 100 < PEASANTMOVE) {
                    i = genrand_int31() % MAXDIRECTIONS;

                    if (r->connect[i]->terrain != T_OCEAN) {
                        r->peasants--;
                        r->connect[i]->immigrants++;
                    }
                }
            }
        }

        for (u=r->units_;u;u=u->next) {
            getmoney(r, u, u->number * MAINTENANCE);
            n = u->money / MAINTENANCE;

            if (u->number > n) {
                if (n)
                    sprintf(buf, "%s loses %d to starvation.", unitid(u),
                            u->number - n);
                else
                    sprintf(buf, "%s starves to death.", unitid(u));
                addevent(u->faction, buf);

                for (i = 0; i != MAXSKILLS; i++)
                    u->skills[i] = distribute(u->number, n, u->skills[i]);

                u->number = n;
            }

            u->money -= u->number * MAINTENANCE;
        }
    }

    removeempty();

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
        r->peasants += r->immigrants;
    }
    /* Warn players who haven't sent in orders */

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (turn - f->lastorders == ORDERGAP) {
            addmessage(f, "Please send orders next turn if you wish to continue playing.");
        }
    }
}

void rqstrlist(storage * store, quicklist ** qlp)
{
    int n;

    store->api->r_int(store->handle, &n);

    while (--n >= 0) {
        char buf[1023];
        if (store->api->r_str(store->handle, buf, sizeof(buf))==0) {
            if (qlp) ql_push(qlp, _strdup(buf));
        }
    }
}

int readgame(void)
{
    FILE * F;
    int i, n, n2;
    ql_iter fli;
    region *r;
    building *b;
    ship *sh;
    unit *u;
    int minx, miny, maxx, maxy;
    storage store;
    int version = VER_NOHEADER;

    minx = INT_MAX;
    maxx = INT_MIN;
    miny = INT_MAX;
    maxy = INT_MIN;

    sprintf(buf, "data/%d", turn);
    F = cfopen(buf, "rb");
    store_init(&store, F);

    printf("Reading turn %d...\n", turn);

    store.api->r_int(store.handle, &n);
    if (n==-1) {
        store.api->r_int(store.handle, &version);
        store.api->r_int(store.handle, &n);
    }
    if (turn!=n) return -1;

    /* Read factions */

    store.api->r_int(store.handle, &n);
    if (n<0) return -2;

    while (--n >= 0) {
        int no;
        char buf[NAMESIZE];
        faction *f;

        store.api->r_int(store.handle, &no);
        f = create_faction(no);
        if (store.api->r_str(store.handle, buf, sizeof(buf))==0) {
            faction_setname(f, buf[0] ? buf : 0);
        }
        if (store.api->r_str(store.handle, buf, sizeof(buf))==0) {
            faction_setaddr(f, buf[0] ? buf : 0);
        }
        if (store.api->r_str(store.handle, buf, sizeof(buf))==0) {
            faction_setpwhash(f, buf[0] ? buf : 0);
        }
        /* erase the password to prevent memory bugs from exposing it */
        memset(buf, 0, sizeof(buf));
        store.api->r_int(store.handle, &f->lastorders);
        store.api->r_int(store.handle, &f->origin_x);
        store.api->r_int(store.handle, &f->origin_y);

        for (i = 0; i != MAXSPELLS; i++) {
            store.api->r_int(store.handle, &no);
            f->showdata[i] = (no != 0);
        }

        store.api->r_int(store.handle, &n2);
        if (n2<0) return -6;
        if (n2>0) {
            f->allies.fnos = (int *)malloc(sizeof(int) * (n2+1));
            for (i=0; i!=n2; ++i) {
                store.api->r_int(store.handle, f->allies.fnos+i);
            }
            f->allies.fnos[n2] = 0;
        } else {
            f->allies.fnos = 0;
        }

        rqstrlist(&store, &f->mistakes);
        rqstrlist(&store, &f->messages);
        rqstrlist(&store, 0);
        rqstrlist(&store, &f->events);

        f->alive = false;
    }

    /* Read regions */

    store.api->r_int(store.handle, &n);
    if (n<0) return -3;

    while (--n >= 0) {
        int x, y, n;
        char name[DISPLAYSIZE];
		unit **up = 0;

        store.api->r_int(store.handle, &x);
        store.api->r_int(store.handle, &y);
        store.api->r_int(store.handle, &n);
        r = create_region(x, y, (terrain_t)n);
        minx = MIN(minx, r->x);
        maxx = MAX(maxx, r->x);
        miny = MIN(miny, r->y);
        maxy = MAX(maxy, r->y);
        if (store.api->r_str(store.handle, name, sizeof(name))==0) {
            region_setname(r, name[0] ? name : 0);
        }
        store.api->r_int(store.handle, &r->peasants);
        store.api->r_int(store.handle, &r->money);

        store.api->r_int(store.handle, &n2);
        if (n2<0) return -4;

        while (--n2 >= 0) {
            b = (building *)malloc(sizeof(building));

            store.api->r_int(store.handle, &b->no);
            if (store.api->r_str(store.handle, name, sizeof(name))==0) {
                building_setname(b, name);
            }
            if (store.api->r_str(store.handle, name, sizeof(name))==0) {
                building_setdisplay(b, name);
            }
            store.api->r_int(store.handle, &b->size);

            ql_push(&r->buildings, b);
        }

        store.api->r_int(store.handle, &n2);
        if (n2<0) return -5;

        while (--n2 >= 0) {
            int no, type;
            char temp[DISPLAYSIZE];

            store.api->r_int(store.handle, &no);
            sh = create_ship(no, SH_LONGBOAT);

            if (store.api->r_str(store.handle, name, sizeof(temp))==0) {
                ship_setname(sh, name);
            }
            if (store.api->r_str(store.handle, temp, sizeof(temp))==0) {
                ship_setdisplay(sh, temp);
            }
            store.api->r_int(store.handle, &type);
            sh->type = (ship_t)type;
            store.api->r_int(store.handle, &sh->left);

            ql_push(&r->ships, sh);
        }

        for (;;) {
            char temp[DISPLAYSIZE];
            int no, fno, cs;
            unit *stack = 0;

            store.api->r_int(store.handle, &no);
			if (no==0) {
				break;
			}
            store.api->r_int(store.handle, &fno);
            u = create_unit(findfaction(fno), no);
            if (u->no>nextunitid) nextunitid = no+1;
            if (store.api->r_str(store.handle, temp, sizeof(temp))==0) {
                unit_setname(u, temp[0] ? temp : 0);
            }
            if (store.api->r_str(store.handle, temp, sizeof(temp))==0) {
                unit_setdisplay(u, temp[0] ? temp : 0);
            }
            store.api->r_int(store.handle, &u->number);
            if (u->number) {
                u->faction->alive = true;
            }
            store.api->r_int(store.handle, &u->money);

            store.api->r_int(store.handle, &no);
            u->building = no ? findbuilding(no) : 0;

            store.api->r_int(store.handle, &no);
            u->ship = no ? findship(no) : 0;

            store.api->r_int(store.handle, &no);
            u->owner = no != 0;
            store.api->r_int(store.handle, &no);
            u->behind = no != 0;
            store.api->r_int(store.handle, &no);
            u->guard = no != 0;

            store.api->r_str(store.handle, u->lastorder, sizeof(u->lastorder));
            store.api->r_int(store.handle, &cs);

            for (i = 0; i != MAXSKILLS; i++) {
                store.api->r_int(store.handle, &no);
                u->skills[i] = (skill_t)no;
            }
            for (i = 0; i != MAXITEMS; i++) {
                store.api->r_int(store.handle, &no);
                u->items[i] = (item_t)no;
            }

            for (i = 0; i != MAXSPELLS; i++) {
                store.api->r_int(store.handle, &no);
                u->spells[i] = (spell_t)no;
                if (no==cs) {
                    u->combatspell = cs;
                }
                if (u->spells[i]) {
                    u->faction->seendata[i] = true;
                }
            }
            
            region_addunit(r, u, stack, up);
			if (!u->next) {
				up = &u->next;
			}
        }
    }

    /* Get rid of stuff that was only relevant last turn */

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        memset(f->showdata, 0, sizeof f->showdata);

        ql_foreach(f->mistakes, free);
        ql_free(f->mistakes);
        ql_foreach(f->messages, free);
        ql_free(f->messages);
        ql_foreach(f->events, free);
        ql_free(f->events);
        ql_foreach(f->battles, (ql_cb)free_battle);
        ql_free(f->battles);

        f->mistakes = 0;
        f->messages = 0;
        f->battles = 0;
        f->events = 0;
    }

    /* Link rfaction structures */

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (f->allies.fnos) {
            int i, *fnos = f->allies.fnos;
            f->allies.factions = 0;
            for (i=0;fnos[i];++i) {
                ql_set_insert(&f->allies.factions, findfaction(fnos[i]));
            }
            free(fnos);
        }
    }
    /* Clear away debris of destroyed factions */
    removeempty();
    removenullfactions();

    update_world(minx, miny, maxx, maxy);
    connectregions();
    store_done(&store);
    return 0;
}

void wqstrlist(storage * store, quicklist * ql)
{
    ql_iter qli;
    store->api->w_int(store->handle, ql_length(ql));

    for (qli = qli_init(&ql); qli_more(qli);) {
        const char * str = (const char *)qli_next(&qli);
        store->api->w_str(store->handle, str);
    }
}

int writegame(void)
{
    storage store;
    int i;
    ql_iter rli, fli;

    sprintf(buf, "data/%d", turn);
    printf("Writing turn %d...\n", turn);

    store_init(&store, cfopen(buf, "wb"));
    store.api->w_int(store.handle, -1);
    store.api->w_int(store.handle, VER_CURRENT);
    store.api->w_int(store.handle, turn);

    /* Write factions */

    store.api->w_int(store.handle, ql_length(factions));

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        ql_iter qli;

        store.api->w_int(store.handle, f->no);
        store.api->w_str(store.handle, faction_getname(f));
        store.api->w_str(store.handle, faction_getaddr(f));
        store.api->w_str(store.handle, faction_getpwhash(f));
        store.api->w_int(store.handle, f->lastorders);
        store.api->w_int(store.handle, f->origin_x);
        store.api->w_int(store.handle, f->origin_y);

        for (i = 0; i != MAXSPELLS; i++) {
            store.api->w_int(store.handle, f->showdata[i]);
        }

        store.api->w_int(store.handle, ql_length(f->allies.factions));
        for (qli = qli_init(&f->allies.factions); qli_more(qli);) {
            faction *rf = (faction *)qli_next(&qli);
            store.api->w_int(store.handle, rf->no);
        }

        wqstrlist(&store, f->mistakes);
        wqstrlist(&store, f->messages);
        wqstrlist(&store, 0);
        wqstrlist(&store, f->events);
    }

    /* Write regions */

    store.api->w_int(store.handle, ql_length(regions));

    for (rli = qli_init(&regions); qli_more(rli);) {
        region *r = (region *)qli_next(&rli);
		unit *u;
        ql_iter qli;

        store.api->w_int(store.handle, r->x);
        store.api->w_int(store.handle, r->y);
        store.api->w_int(store.handle, r->terrain);
        store.api->w_str(store.handle, region_getname(r));
        store.api->w_int(store.handle, r->peasants);
        store.api->w_int(store.handle, r->money);

        store.api->w_int(store.handle, ql_length(r->buildings));

        for (qli=qli_init(&r->buildings);qli_more(qli);) {
            building *b = (building *)qli_next(&qli);

            store.api->w_int(store.handle, b->no);
            store.api->w_str(store.handle, building_getname(b));
            store.api->w_str(store.handle, building_getdisplay(b));
            store.api->w_int(store.handle, b->size);
        }

        store.api->w_int(store.handle, ql_length(r->ships));

        for (qli = qli_init(&r->ships); qli_more(qli);) {
            ship *sh = (ship *)qli_next(&qli);
            store.api->w_int(store.handle, sh->no);
            store.api->w_str(store.handle, ship_getname(sh));
            store.api->w_str(store.handle, ship_getdisplay(sh));
            store.api->w_int(store.handle, sh->type);
            store.api->w_int(store.handle, sh->left);
        }

        for (u=r->units_;u;u=u->next) {

            assert(u->region==r);
            store.api->w_int(store.handle, u->no);
            store.api->w_int(store.handle, u->faction->no);
//            store.api->w_int(store.handle, u->stack ? u->stack->no : 0);
            store.api->w_str(store.handle, unit_getname(u));
            store.api->w_str(store.handle, unit_getdisplay(u));
            store.api->w_int(store.handle, u->number);
            store.api->w_int(store.handle, u->money);
            store.api->w_int(store.handle, u->building ? u->building->no : 0);
            store.api->w_int(store.handle, u->ship ? u->ship->no : 0);
            store.api->w_int(store.handle, u->owner);
            store.api->w_int(store.handle, u->behind);
            store.api->w_int(store.handle, u->guard);
            store.api->w_str(store.handle, u->lastorder);
            store.api->w_int(store.handle, u->combatspell);

            for (i = 0; i != MAXSKILLS; i++) {
                store.api->w_int(store.handle, u->skills[i]);
            }

            for (i = 0; i != MAXITEMS; i++) {
                store.api->w_int(store.handle, u->items[i]);
            }

            for (i = 0; i != MAXSPELLS; i++) {
                store.api->w_int(store.handle, u->spells[i]);
            }

        }

		store.api->w_int(store.handle, 0);
    }
    store_done(&store);
    return 0;
}

void initgame(void)
{
    if (turn < 0) {
        turn = 0;
        _mkdir("data");
        makeblock(0, 0);
    } else {
        readgame();
    }
}

void createcontinent(void)
{
    int x, y;
    char buf[16];

    printf("X? ");
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == 0)
        return;

    x = atoi(buf);

    printf("Y? ");
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == 0)
        return;
    y = atoi(buf);

    makeblock(x, y);

    writemap(stdout);
}
