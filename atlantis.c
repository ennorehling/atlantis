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
#include "unit.h"
#include "settings.h"
#include "spells.h"
#include "skills.h"
#include "items.h"

#include "rtl.h"
#include "bool.h"

#include "storage/storage.h"
#include "storage/binarystore.h"
#include "storage/textstore.h"

#include "crypto/mtrand.h"
#include "crypto/base64.h"

const storage * store = &binary_store;

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

#define addlist2(l,p) (*l = p, l = &p->next)
#define xisdigit(c) ((c) == '-' || ((c) >= '0' && (c) <= '9'))
#define addptr(p,i) ((void *)(((char *)p) + i))

typedef enum {
    SH_LONGBOAT,
    SH_CLIPPER,
    SH_GALLEON,
} ship_t;

typedef struct list {
    struct list *next;
} list;

typedef struct strlist {
    struct strlist *next;
    char s[1];
} strlist;

typedef struct building {
    struct building *next;
    int no;
    char name[NAMESIZE];
    char display[DISPLAYSIZE];
    int size;
    int sizeleft;
} building;

typedef struct ship {
    struct ship *next;
    int no;
    char name[NAMESIZE];
    char display[DISPLAYSIZE];
    ship_t type;
    int left;
} ship;

typedef struct order {
    struct order *next;
    struct unit *unit;
    int qty;
} order;

typedef struct troop {
    struct troop *next;
    struct unit *unit;
    int lmoney;
    int status;
    int side;
    bool attacked;
    item_t weapon;
    bool missile;
    int skill;
    int armor;
    bool behind;
    int inside;
    int reload;
    bool canheal;
    char runesword;
    bool invulnerable;
    char power;
    char shieldstone;
    char demoralized;
    char dazzled;
} troop;

char buf[10240];
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
    "study",
    "tax",
    "teach",
    "transfer",
    "unit",
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

char *shiptypenames[] = {
    "longboat",
    "clipper",
    "galleon",
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

const char *skillnames[] = {
    "mining",
    "lumberjack",
    "quarrying",
    "horse training",
    "weaponsmith",
    "armorer",
    "building",
    "shipbuilding",
    "entertainment",
    "stealth",
    "observation",
    "tactics",
    "riding",
    "sword",
    "crossbow",
    "longbow",
    "magic",
};

const char *itemnames[2][MAXITEMS] = {
    {
        "iron",
        "wood",
        "stone",
        "horse",
        "sword",
        "crossbow",
        "longbow",
        "chain mail",
        "plate armor",
        "Amulet of Darkness",
        "Amulet of Death",
        "Amulet of Healing",
        "Amulet of True Seeing",
        "Cloak of Invulnerability",
        "Ring of Invisibility",
        "Ring of Power",
        "Runesword",
        "Shieldstone",
        "Staff of Fire",
        "Staff of Lightning",
        "Wand of Teleportation",
    }, {
        "iron",
        "wood",
        "stone",
        "horses",
        "swords",
        "crossbows",
        "longbows",
        "chain mail",
        "plate armor",
        "Amulets of Darkness",
        "Amulets of Death",
        "Amulets of Healing",
        "Amulets of True Seeing",
        "Cloaks of Invulnerability",
        "Rings of Invisibility",
        "Rings of Power",
        "Runeswords",
        "Shieldstones",
        "Staffs of Fire",
        "Staffs of Lightning",
        "Wands of Teleportation",
    }
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

const char *spellnames[] = {
    "Black Wind",
    "Cause Fear",
    "Contaminate Water",
    "Dazzling Light",
    "Fireball",
    "Hand of Death",
    "Heal",
    "Inspire Courage",
    "Lightning Bolt",
    "Make Amulet of Darkness",
    "Make Amulet of Death",
    "Make Amulet of Healing",
    "Make Amulet of True Seeing",
    "Make Cloak of Invulnerability",
    "Make Ring of Invisibility",
    "Make Ring of Power",
    "Make Runesword",
    "Make Shieldstone",
    "Make Staff of Fire",
    "Make Staff of Lightning",
    "Make Wand of Teleportation",
    "Shield",
    "Sunfire",
    "Teleport",
};

char spelllevel[] = {
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

char *spelldata[] = {
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

void scat(const char *s)
{
    strcat(buf, s);
}

void icat(int n)
{
    char s[20];

    sprintf(s, "%d", n);
    scat(s);
}

void *cmalloc(int n)
{
    void *p;

    if (n == 0)
        n = 1;

    p = malloc(n);

    if (p == 0) {
        puts("Out of memory.");
        exit(1);
    }

    return p;
}

void rnd_seed(unsigned long x)
{
    init_genrand(x);
}

int rnd(void)
{
    return (int)genrand_int31();
}

void addlist(void *l1, void *p1)
{
    list **l;
    list *p, *q;

    l = l1;
    p = p1;

    p->next = 0;

    if (*l) {
        for (q = *l; q->next; q = q->next);
        q->next = p;
    } else
        *l = p;
}

void choplist(list ** l, list * p)
{
    list *q;

    if (*l == p)
        *l = p->next;
    else {
        for (q = *l; q->next != p; q = q->next)
            assert(q);
        q->next = p->next;
    }
}

void translist(void *l1, void *l2, void *p)
{
    choplist(l1, p);
    addlist(l2, p);
}

void removelist(void *l, void *p)
{
    choplist(l, p);
    free(p);
}

void freelist(void *p1)
{
    list *p, *p2;

    p = p1;

    while (p) {
        p2 = p->next;
        free(p);
        p = p2;
    }
}

int listlen(void *l)
{
    int i;
    list *p;

    for (p = l, i = 0; p; p = p->next, i++);
    return i;
}

strlist *makestrlist(char *s)
{
    strlist *S;

    S = cmalloc(sizeof(strlist) + strlen(s));
    strcpy(S->s, s);
    return S;
}

void addstrlist(strlist ** SP, char *s)
{
    addlist(SP, makestrlist(s));
}

void catstrlist(strlist ** SP, strlist * S)
{
    strlist *S2;

    while (*SP)
        SP = &((*SP)->next);

    while (S) {
        S2 = makestrlist(S->s);
        addlist2(SP, S2);
        S = S->next;
    }

    *SP = 0;
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

char * getbuf(FILE * F)
{
    int i;
    int c;

    i = 0;

    for (;;) {
        c = fgetc(F);

        if (c == EOF) {
            buf[i] = 0;
            return buf;
        }

        if (c == '\n') {
            buf[i] = 0;
            return buf;
        }

        if (i == sizeof buf - 1) {
            buf[i] = 0;
            while (c != EOF && c != '\n')
                c = fgetc(F);
            if (c == EOF)
                buf[i] = 0;
            return buf;
        }

        buf[i++] = (char) c;
    }
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
    return 0;
}

int effskill(unit * u, int i)
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

bool ispresent(faction * f, region * r)
{
    unit *u;

    for (u = r->units; u; u = u->next)
        if (u->faction == f)
            return true;

    return false;
}

int cansee(faction * f, region * r, unit * u)
{
    int n, o;
    int cansee;
    unit *u2;

    if (u->faction == f)
        return 2;

    cansee = 0;
    if (u->guard || u->building || u->ship)
        cansee = 1;

    n = effskill(u, SK_STEALTH);

    for (u2 = r->units; u2; u2 = u2->next) {
        if (u2->faction != f)
            continue;

        if (u->items[I_RING_OF_INVISIBILITY] &&
            u->items[I_RING_OF_INVISIBILITY] == u->number &&
            !u2->items[I_AMULET_OF_TRUE_SEEING])
            continue;

        o = effskill(u2, SK_OBSERVATION);
        if (o > n)
            return 2;
        if (o >= n)
            cansee = 1;
    }

    return cansee;
}

char *igetstr(const char *s1)
{
    int i;
    static const char *s;
    static char buf[256];

    if (s1)
        s = s1;
    while (*s == ' ')
        s++;
    i = 0;

    while (*s && *s != ' ') {
        buf[i] = *s;
        if (*s == '_')
            buf[i] = ' ';
        s++;
        i++;
    }

    buf[i] = 0;
    return buf;
}

char *getstr(void)
{
    return igetstr(0);
}

int geti(void)
{
    return atoip(getstr());
}

faction *findfaction(int n)
{
    faction *f;

    for (f = factions; f; f = f->next)
        if (f->no == n)
            return f;

    return 0;
}

faction *getfaction(void)
{
    return findfaction(atoi(getstr()));
}

region *findregion(int x, int y)
{
    region *r;

    for (r = regions; r; r = r->next)
        if (r->x == x && r->y == y)
            return r;

    return 0;
}

building *findbuilding(int n)
{
    region *r;
    building *b;

    for (r = regions; r; r = r->next)
        for (b = r->buildings; b; b = b->next)
            if (b->no == n)
                return b;

    return 0;
}

building *getbuilding(region * r)
{
    int n;
    building *b;

    n = geti();

    for (b = r->buildings; b; b = b->next)
        if (b->no == n)
            return b;

    return 0;
}

ship *findship(int n)
{
    region *r;
    ship *sh;

    for (r = regions; r; r = r->next)
        for (sh = r->ships; sh; sh = sh->next)
            if (sh->no == n)
                return sh;

    return 0;
}

ship *getship(region * r)
{
    int n;
    ship *sh;

    n = geti();

    for (sh = r->ships; sh; sh = sh->next)
        if (sh->no == n)
            return sh;

    return 0;
}

unit *findunitg(int n)
{
    region *r;
    unit *u;

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            if (u->no == n)
                return u;

    return 0;
}

unit *getnewunit(region * r, unit * u)
{
    int n;
    unit *u2;

    n = geti();

    if (n == 0)
        return 0;

    for (u2 = r->units; u2; u2 = u2->next)
        if (u2->faction == u->faction && u2->alias == n)
            return u2;

    return 0;
}

unit *getunitg(region * r, unit * u)
{
    const char *s;

    s = getstr();

    if (!_strcmpl(s, "new"))
        return getnewunit(r, u);

    return findunitg(atoi(s));
}

int getunit0;
int getunitpeasants;

unit *getunit(region * r, unit * u)
{
    int n;
    const char *s;
    unit *u2;

    getunit0 = 0;
    getunitpeasants = 0;

    s = getstr();

    if (!_strcmpl(s, "new"))
        return getnewunit(r, u);

    if (r->terrain != T_OCEAN && !_strcmpl(s, "peasants")) {
        getunitpeasants = 1;
        return 0;
    }

    n = atoi(s);

    if (n == 0) {
        getunit0 = 1;
        return 0;
    }

    for (u2 = r->units; u2; u2 = u2->next)
        if (u2->no == n && cansee(u->faction, r, u2) && !u2->isnew)
            return u2;

    return 0;
}

int strpcmp(const void *s1, const void *s2)
{
    return _strcmpl(*(char **) s1, *(char **) s2);
}

int findkeyword(const char *s)
{
    const char **sp;

    if (!_strcmpl(s, "describe"))
        return K_DISPLAY;
    if (!_strcmpl(s, "n"))
        return K_NORTH;
    if (!_strcmpl(s, "s"))
        return K_SOUTH;
    if (!_strcmpl(s, "e"))
        return K_EAST;
    if (!_strcmpl(s, "w"))
        return K_WEST;
    if (!_strcmpl(s, "m"))
        return K_MIR;
    if (!_strcmpl(s, "y"))
        return K_YDD;

    sp = bsearch(&s, keywords, MAXKEYWORDS, sizeof s, strpcmp);
    if (sp == 0)
        return -1;
    return sp - keywords;
}

int igetkeyword(const char *s)
{
    return findkeyword(igetstr(s));
}

int getkeyword(void)
{
    return findkeyword(getstr());
}

int findstr(const char **v, const char *s, int n)
{
    int i;

    for (i = 0; i != n; i++)
        if (!_strcmpl(v[i], s))
            return i;

    return -1;
}

int findskill(const char *s)
{
    if (!_strcmpl(s, "horse"))
        return SK_HORSE_TRAINING;
    if (!_strcmpl(s, "entertain"))
        return SK_ENTERTAINMENT;

    return findstr(skillnames, s, MAXSKILLS);
}

int getskill(void)
{
    return findskill(getstr());
}

int finditem(const char *s)
{
    int i;

    if (!_strcmpl(s, "chain"))
        return I_CHAIN_MAIL;
    if (!_strcmpl(s, "plate"))
        return I_PLATE_ARMOR;

    i = findstr(itemnames[0], s, MAXITEMS);
    if (i >= 0)
        return i;

    return findstr(itemnames[1], s, MAXITEMS);
}

int getitem(void)
{
    return finditem(getstr());
}

spell_t findspell(const char *s)
{
    return (spell_t) findstr(spellnames, s, MAXSPELLS);
}

spell_t getspell(void)
{
    return findspell(getstr());
}

unit *createunit(region * r1, faction * f)
{
    int i, n;
    region *r;
    unit *u2;
    char v[1000];

    for (n = 0;; n += 1000) {
        memset(v, 0, sizeof v);

        if (n == 0)
            v[0] = 1;

        for (r = regions; r; r = r->next)
            for (u2 = r->units; u2; u2 = u2->next)
                if (u2->no >= n && u2->no < n + 1000)
                    v[u2->no - n] = 1;

        for (i = 0; i != 1000; i++)
            if (!v[i]) {
                unit * u = create_unit(f, n + i);
                strcpy(u->lastorder, "work");
                u->combatspell = -1;
                addlist(&r1->units, u);
                return u;
            }
    }
}

int scramblecmp(const void *p1, const void *p2)
{
    return *((long *) p1) - *((long *) p2);
}

void scramble(void *v1, int n, int width)
{
    int i;
    void *v;

    v = cmalloc(n * (width + 4));

    for (i = 0; i != n; i++) {
        *(long *) addptr(v, i * (width + 4)) = rnd();
        memcpy(addptr(v, i * (width + 4) + 4), addptr(v1, i * width),
               width);
    }

    qsort(v, n, width + 4, scramblecmp);

    for (i = 0; i != n; i++)
        memcpy(addptr(v1, i * width), addptr(v, i * (width + 4) + 4),
               width);

    free(v);
}

region *inputregion(void)
{
    int x, y;
    region *r = 0;

    while (!r) {
        printf("X? ");
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == 0)
            return 0;
        x = atoi(buf);

        printf("Y? ");
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == 0)
            return 0;
        y = atoi(buf);

        r = findregion(x, y);

        if (!r) {
            puts("No such region.");
        }
    }
    return r;
}

faction * createfaction(int no)
{
    char name[NAMESIZE];
    faction * f = create_faction(no);
    f->alive = 1;
    f->lastorders = turn;
    sprintf(name, "Faction %d", f->no);
    faction_setname(f, name);
    return f;
}

faction * addplayer(region * r, const char * email, int no)
{
    faction * f;
    unit * u;
    int i;
    unsigned long pwdata[4];
    char msg[1024];
    
    if (no==0) ++no;
    while (findfaction(no)) ++no;

    f = createfaction(no);
    faction_setaddr(f, email);

    for (i=0;i!=4;++i) pwdata[i] = genrand_int32();
    base64_encode((unsigned char *)pwdata, sizeof(pwdata), buf2, sizeof(buf2));
    buf2[8] = 0;
    faction_setpassword(f, buf2);
    sprintf(msg, "Your password is '%s'.", buf2);
    addstrlist(&f->messages, msg);

    u = createunit(r, f);
    u->number = 1;
    u->money = STARTMONEY;
    u->isnew = true;

    f->origin_x = r->x;
    f->origin_y = r->y;
    return f;
}

void addplayers(void)
{
    FILE * F;
    region *r;
    faction *f;
    int no = 0;

    r = inputregion();

    if (!r)
        return;

    printf("Name of players file? ");
    fgets(buf, sizeof(buf), stdin);

    if (!buf[0])
        return;

    F = cfopen(buf, "r");

    for (;;) {
        if (!getbuf(F)) {
            fclose(F);
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
    region *r;

    for (r = regions; r; r = r->next) {
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
            x = rnd() % BLOCKSIZE;
            y = rnd() % BLOCKSIZE;
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
        x = rnd() % BLOCKSIZE;
        y = rnd() % BLOCKSIZE;
    }
    while (newblock[x][y] != T_PLAIN);

    newblock[x][y] = (char) to;
    transmute(T_PLAIN, to, n, 1);
}

bool regionnameinuse(const char *s)
{
    region *r;

    for (r = regions; r; r = r->next) {
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

struct region * create_region(int x, int y, terrain_t t)
{
    region * r;

    r = cmalloc(sizeof(region));
    memset(r, 0, sizeof(region));

    r->x = x;
    r->y = y;
    r->terrain = t;

    addlist(&regions, r);
    return r;
}

void initregion(region *r) {
    if (r->terrain != T_OCEAN) {
        int i, n = 0;
        region * r2;

        for (r2 = regions; r2; r2 = r2->next) {
            const char * rname = region_getname(r2);
            if (rname)
                n++;
        }
        i = rnd() % (sizeof regionnames / sizeof(char *));
        if (n < sizeof regionnames / sizeof(char *))
            while (regionnameinuse(regionnames[i]))
                i = rnd() % (sizeof regionnames /
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
        x = rnd() % BLOCKSIZE;
        y = rnd() % BLOCKSIZE;
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
                terrain_t t = (terrain_t)(rnd() % NUMTERRAINS);
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
            terrain_t t = T_OCEAN;

            if (x >= BLOCKBORDER && x < BLOCKBORDER + BLOCKSIZE &&
                y >= BLOCKBORDER && y < BLOCKBORDER + BLOCKSIZE) {
                t = newblock[x - BLOCKBORDER][y - BLOCKBORDER];
            }
            r = create_region(x1 + x, y1 + y, t);
            initregion(r);
        }
    }
    connectregions();
}

char *gamedate(void)
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

char *factionid(faction * f)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", faction_getname(f), f->no);
    return buf;
}

const char *regionid(const region * r, const faction * f)
{
    static char buf[NAMESIZE + 20];
    assert(f);
    assert(r);
    if (r->terrain == T_OCEAN)
        sprintf(buf, "(%d,%d)", r->x - f->origin_x, r->y - f->origin_y);
    else
        sprintf(buf, "%s (%d,%d)", region_getname(r), r->x - f->origin_x, r->y - f->origin_y);
    return buf;
}

char *buildingid(building * b)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", b->name, b->no);
    return buf;
}

char *shipid(ship * sh)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", sh->name, sh->no);
    return buf;
}

char *unitid(unit * u)
{
    static char buf[NAMESIZE + 20];

    sprintf(buf, "%s (%d)", unit_getname(u), u->no);
    return buf;
}

void sparagraph(strlist ** SP, char *s, int indent, int mark)
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

        if (firstline && mark)
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

void spskill(unit * u, int i, int *dh, int days)
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

void spunit(strlist ** SP, faction * f, region * r, unit * u, int indent,
            int battle)
{
    const char * sc;
    int i;
    int dh;

    strcpy(buf, unitid(u));

    if (cansee(f, r, u) == 2) {
        scat(", faction ");
        scat(factionid(u->faction));
    }

    if (u->number != 1) {
        scat(", number: ");
        icat(u->number);
    }

    if (u->behind && (u->faction == f || battle))
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
                scat(itemnames[0][i]);
            else {
                icat(u->items[i]);
                scat(" ");
                scat(itemnames[1][i]);
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

void mistake(faction * f, char *s, char *comment)
{
    static char buf[512];

    sprintf(buf, "%s: %s.", s, comment);
    sparagraph(&f->mistakes, buf, 0, 0);
}

void mistake2(unit * u, strlist * S, char *comment)
{
    mistake(u->faction, S->s, comment);
}

void mistakeu(unit * u, char *comment)
{
    mistake(u->faction, u->thisorder, comment);
}

void addevent(faction * f, char *s)
{
    sparagraph(&f->events, s, 0, 0);
}

void addbattle(faction * f, char *s)
{
    sparagraph(&f->battles, s, 0, 0);
}

void reportevent(region * r, char *s)
{
    faction *f;
    unit *u;

    for (f = factions; f; f = f->next)
        for (u = r->units; u; u = u->next)
            if (u->faction == f && u->number) {
                addevent(f, s);
                break;
            }
}

void leave(region * r, unit * u)
{
    unit *u2;
    building *b;
    ship *sh;

    if (u->building) {
        b = u->building;
        u->building = 0;

        if (u->owner) {
            u->owner = false;

            for (u2 = r->units; u2; u2 = u2->next)
                if (u2->faction == u->faction && u2->building == b) {
                    u2->owner = true;
                    return;
                }

            for (u2 = r->units; u2; u2 = u2->next)
                if (u2->building == b) {
                    u2->owner = true;
                    return;
                }
        }
    }

    if (u->ship) {
        sh = u->ship;
        u->ship = 0;

        if (u->owner) {
            u->owner = false;

            for (u2 = r->units; u2; u2 = u2->next)
                if (u2->faction == u->faction && u2->ship == sh) {
                    u2->owner = true;
                    return;
                }

            for (u2 = r->units; u2; u2 = u2->next)
                if (u2->ship == sh) {
                    u2->owner = true;
                    return;
                }
        }
    }
}

void removeempty(void)
{
    int i;
    region *r;
    ship *sh, *sh2;
    unit *u, *u2, *u3;

    for (r = regions; r; r = r->next) {
        for (u = r->units; u;) {
            u2 = u->next;

            if (!u->number) {
                leave(r, u);

                for (u3 = r->units; u3; u3 = u3->next)
                    if (u3->faction == u->faction) {
                        u3->money += u->money;
                        u->money = 0;
                        for (i = 0; i != MAXITEMS; i++)
                            u3->items[i] += u->items[i];
                        break;
                    }

                if (r->terrain != T_OCEAN)
                    r->money += u->money;

                freelist(u->orders);
                removelist(&r->units, u);
            }

            u = u2;
        }

        if (r->terrain == T_OCEAN)
            for (sh = r->ships; sh;) {
                sh2 = sh->next;

                for (u = r->units; u; u = u->next)
                    if (u->ship == sh)
                        break;

                if (!u)
                    removelist(&r->ships, sh);

                sh = sh2;
            }
    }
}

void destroyfaction(faction * f)
{
    region *r;
    unit *u;

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            if (u->faction == f) {
                if (r->terrain != T_OCEAN)
                    r->peasants += u->number;

                u->number = 0;
            }

    f->alive = 0;
}

void togglerf(unit * u, strlist * S, rfaction ** r)
{
    faction *f;
    rfaction *rf;

    f = getfaction();

    if (f) {
        if (f != u->faction) {
            for (rf = *r; rf; rf = rf->next)
                if (rf->faction == f)
                    break;

            if (rf)
                removelist(r, rf);
            else {
                rf = cmalloc(sizeof(rfaction));
                rf->faction = f;
                addlist(r, rf);
            }
        }
    } else
        mistake2(u, S, "Faction not found");
}

bool iscoast(region * r)
{
    int i;

    for (i = 0; i != MAXDIRECTIONS; i++)
        if (r->connect[i]->terrain == T_OCEAN)
            return true;

    return false;
}

int distribute(int old, int new, int n)
{
    int i;
    int t;

    assert(new <= old);

    if (old == 0)
        return 0;

    t = (n / old) * new;
    for (i = (n % old); i; i--)
        if (rnd() % old < new)
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

void getmoney(region * r, unit * u, int n)
{
    int i;
    unit *u2;

    n -= u->money;

    for (u2 = r->units; u2 && n >= 0; u2 = u2->next)
        if (u2->faction == u->faction && u2 != u) {
            i = MIN(u2->money, n);
            u2->money -= i;
            u->money += i;
            n -= i;
        }
}

int ntroops;
troop *ta;
troop attacker, defender;
int initial[2];
int left[2];
int infront[2];
int toattack[2];
int shields[2];
int runeswords[2];

troop **maketroops(troop ** tp, unit * u, int terrain)
{
    int i;
    troop *t;
    static int skills[MAXSKILLS];
    static int items[MAXITEMS];

    for (i = 0; i != MAXSKILLS; i++)
        skills[i] = effskill(u, i);
    memcpy(items, u->items, sizeof items);

    left[u->side] += u->number;
    if (!u->behind)
        infront[u->side] += u->number;

    for (i = u->number; i; i--) {
        t = cmalloc(sizeof(troop));
        memset(t, 0, sizeof(troop));

        t->unit = u;
        t->side = u->side;
        t->skill = -2;
        t->behind = u->behind;

        if (u->combatspell >= 0)
            t->missile = true;
        else if (items[I_RUNESWORD] && skills[SK_SWORD]) {
            t->weapon = I_SWORD;
            t->skill = skills[SK_SWORD] + 2;
            t->runesword = 1;
            items[I_RUNESWORD]--;
            runeswords[u->side]++;

            if (items[I_HORSE] && skills[SK_RIDING] >= 2
                && terrain == T_PLAIN) {
                t->skill += 2;
                items[I_HORSE]--;
            }
        } else if (items[I_LONGBOW] && skills[SK_LONGBOW]) {
            t->weapon = I_LONGBOW;
            t->missile = true;
            t->skill = skills[SK_LONGBOW];
            items[I_LONGBOW]--;
        } else if (items[I_CROSSBOW] && skills[SK_CROSSBOW]) {
            t->weapon = I_CROSSBOW;
            t->missile = true;
            t->skill = skills[SK_CROSSBOW];
            items[I_CROSSBOW]--;
        } else if (items[I_SWORD] && skills[SK_SWORD]) {
            t->weapon = I_SWORD;
            t->skill = skills[SK_SWORD];
            items[I_SWORD]--;

            if (items[I_HORSE] && skills[SK_RIDING] >= 2
                && terrain == T_PLAIN) {
                t->skill += 2;
                items[I_HORSE]--;
            }
        }

        if (u->spells[SP_HEAL] || items[I_AMULET_OF_HEALING] > 0) {
            t->canheal = true;
            items[I_AMULET_OF_HEALING]--;
        }

        if (items[I_RING_OF_POWER]) {
            t->power = 1;
            items[I_RING_OF_POWER]--;
        }

        if (items[I_SHIELDSTONE]) {
            t->shieldstone = 1;
            items[I_SHIELDSTONE]--;
        }

        if (items[I_CLOAK_OF_INVULNERABILITY]) {
            t->invulnerable = 1;
            items[I_CLOAK_OF_INVULNERABILITY]--;
        } else if (items[I_PLATE_ARMOR]) {
            t->armor = 2;
            items[I_PLATE_ARMOR]--;
        } else if (items[I_CHAIN_MAIL]) {
            t->armor = 1;
            items[I_CHAIN_MAIL]--;
        }

        if (u->building && u->building->sizeleft) {
            t->inside = 2;
            u->building->sizeleft--;
        }

        addlist2(tp, t);
    }

    return tp;
}

void battlerecord(char *s)
{
    faction *f;

    for (f = factions; f; f = f->next)
        if (f->seesbattle)
            sparagraph(&f->battles, s, 0, 0);

    if (s[0])
        puts(s);
}

void battlepunit(region * r, unit * u)
{
    faction *f;

    for (f = factions; f; f = f->next)
        if (f->seesbattle)
            spunit(&f->battles, f, r, u, 4, 1);
}

int contest(int a, int d)
{
    int i;
    static char table[] = { 10, 25, 40 };

    i = a - d + 1;
    if (i < 0)
        return rnd() % 100 < 1;
    if (i > 2)
        return rnd() % 100 < 49;
    return rnd() % 100 < table[i];
}

int hits(void)
{
    int k;

    if (defender.weapon == I_CROSSBOW || defender.weapon == I_LONGBOW)
        defender.skill = -2;
    defender.skill += defender.inside;
    attacker.skill -= (attacker.demoralized + attacker.dazzled);
    defender.skill -= (defender.demoralized + defender.dazzled);

    switch (attacker.weapon) {
    case I_CROSSBOW:
        k = contest(attacker.skill, 0);
        break;

    case I_LONGBOW:
        k = contest(attacker.skill, 2);
        break;

    default:
        k = contest(attacker.skill, defender.skill);
        break;

    }

    if (defender.invulnerable && rnd() % 10000)
        k = 0;

    if (rnd() % 3 < defender.armor)
        k = 0;

    return k;
}

int validtarget(int i)
{
    return !ta[i].status &&
        ta[i].side == defender.side &&
        (!ta[i].behind || !infront[defender.side]);
}

int canbedemoralized(int i)
{
    return validtarget(i) && !ta[i].demoralized;
}

int canbedazzled(int i)
{
    return validtarget(i) && !ta[i].dazzled;
}

int canberemoralized(int i)
{
    return !ta[i].status &&
        ta[i].side == attacker.side && ta[i].demoralized;
}

int selecttarget(void)
{
    int i;

    do
        i = rnd() % ntroops;
    while (!validtarget(i));

    return i;
}

void terminate(int i)
{
    if (!ta[i].attacked) {
        ta[i].attacked = 1;
        toattack[defender.side]--;
    }

    ta[i].status = 1;
    left[defender.side]--;
    if (infront[defender.side])
        infront[defender.side]--;
    if (ta[i].runesword)
        runeswords[defender.side]--;
}

int lovar(int n)
{
    n /= 2;
    return (rnd() % n + 1) + (rnd() % n + 1);
}

void dozap(int n)
{
    static char buf2[40];

    n = lovar(n * (1 + attacker.power));
    n = MIN(n, left[defender.side]);

    sprintf(buf2, ", inflicting %d %s", n,
            (n == 1) ? "casualty" : "casualties");
    scat(buf2);

    while (--n >= 0)
        terminate(selecttarget());
}

void docombatspell(int i)
{
    int j;
    int z;
    int n, m;

    z = ta[i].unit->combatspell;
    sprintf(buf, "%s casts %s", unitid(ta[i].unit), spellnames[z]);

    if (shields[defender.side])
        if (rnd() & 1) {
            scat(", and gets through the shield");
            shields[defender.side] -= 1 + attacker.power;
        } else {
            scat(", but the spell is deflected by the shield!");
            battlerecord(buf);
            return;
        }

    switch (z) {
    case SP_BLACK_WIND:
        dozap(1250);
        break;

    case SP_CAUSE_FEAR:
        if (runeswords[defender.side] && (rnd() & 1))
            break;

        n = lovar(100 * (1 + attacker.power));

        m = 0;
        for (j = 0; j != ntroops; j++)
            if (canbedemoralized(j))
                m++;

        n = MIN(n, m);

        sprintf(buf2, ", affecting %d %s", n,
                (n == 1) ? "person" : "people");
        scat(buf2);

        while (--n >= 0) {
            do
                j = rnd() % ntroops;
            while (!canbedemoralized(j));

            ta[j].demoralized = 1;
        }

        break;

    case SP_DAZZLING_LIGHT:
        n = lovar(50 * (1 + attacker.power));

        m = 0;
        for (j = 0; j != ntroops; j++)
            if (canbedazzled(j))
                m++;

        n = MIN(n, m);

        sprintf(buf2, ", dazzling %d %s", n,
                (n == 1) ? "person" : "people");
        scat(buf2);

        while (--n >= 0) {
            do
                j = rnd() % ntroops;
            while (!canbedazzled(j));

            ta[j].dazzled = 1;
        }

        break;

    case SP_FIREBALL:
        dozap(50);
        break;

    case SP_HAND_OF_DEATH:
        dozap(250);
        break;

    case SP_INSPIRE_COURAGE:
        n = lovar(100 * (1 + attacker.power));

        m = 0;
        for (j = 0; j != ntroops; j++)
            if (canberemoralized(j))
                m++;

        n = MIN(n, m);

        sprintf(buf2, ", affecting %d %s", n,
                (n == 1) ? "person" : "people");
        scat(buf2);

        while (--n >= 0) {
            do
                j = rnd() % ntroops;
            while (!canberemoralized(j));

            ta[j].demoralized = 0;
        }

        break;

    case SP_LIGHTNING_BOLT:
        dozap(10);
        break;

    case SP_SHIELD:
        shields[attacker.side] += 1 + attacker.power;
        break;

    case SP_SUNFIRE:
        dozap(6250);
        break;

    default:
        assert(0);
    }

    scat("!");
    battlerecord(buf);
}

void doshot(void)
{
    int ai, di;

    /* Select attacker */

    do
        ai = rnd() % ntroops;
    while (ta[ai].attacked);

    attacker = ta[ai];
    ta[ai].attacked = 1;
    toattack[attacker.side]--;
    defender.side = 1 - attacker.side;

    ta[ai].dazzled = 0;

    if (attacker.unit) {
        if (attacker.behind && infront[attacker.side] && !attacker.missile)
            return;

        if (attacker.shieldstone)
            shields[attacker.side] += 1 + attacker.power;

        if (attacker.unit->combatspell >= 0) {
            docombatspell(ai);
            return;
        }

        if (attacker.reload) {
            ta[ai].reload--;
            return;
        }

        if (attacker.weapon == I_CROSSBOW)
            ta[ai].reload = 2;
    }

    /* Select defender */

    di = selecttarget();
    defender = ta[di];
    assert(defender.side == 1 - attacker.side);

    /* If attack succeeds */

    if (hits())
        terminate(di);
}

int isallied(unit * u, unit * u2)
{
    rfaction *rf;

    if (!u2)
        return u->guard;

    if (u->faction == u2->faction)
        return 1;

    for (rf = u->faction->allies; rf; rf = rf->next)
        if (rf->faction == u2->faction)
            return 1;

    return 0;
}

int accepts(unit * u, unit * u2)
{
    rfaction *rf;

    if (isallied(u, u2))
        return 1;

    for (rf = u->faction->accept; rf; rf = rf->next)
        if (rf->faction == u2->faction)
            return 1;

    return 0;
}

int admits(unit * u, unit * u2)
{
    rfaction *rf;

    if (isallied(u, u2))
        return 1;

    for (rf = u->faction->admit; rf; rf = rf->next)
        if (rf->faction == u2->faction)
            return 1;

    return 0;
}

unit *buildingowner(region * r, building * b)
{
    unit *u;

    for (u = r->units; u; u = u->next)
        if (u->building == b && u->owner)
            return u;

    return 0;
}

unit *shipowner(region * r, ship * sh)
{
    unit *u;

    for (u = r->units; u; u = u->next)
        if (u->ship == sh && u->owner)
            return u;

    return 0;
}

int mayenter(region * r, unit * u, building * b)
{
    unit *u2;

    u2 = buildingowner(r, b);
    return u2 == 0 || admits(u2, u);
}

int mayboard(region * r, unit * u, ship * sh)
{
    unit *u2;

    u2 = shipowner(r, sh);
    return u2 == 0 || admits(u2, u);
}

void readorders(const char * filename)
{
    FILE * F;
    int i, j;
    faction *f;
    region *r;
    unit *u;
    strlist *S, **SP;

    F = cfopen(filename, "r");

    while (getbuf(F) && buf[0]) {
        if (!strncmp(buf, keywords[K_FACTION], strlen(keywords[K_FACTION]))) {
          NEXTPLAYER:
            igetstr(buf);
            i = geti();
            f = findfaction(i);

            if (f) {
                for (r = regions; r; r = r->next)
                    for (u = r->units; u; u = u->next)
                        if (u->faction == f) {
                            freelist(u->orders);
                            u->orders = 0;
                        }

                for (;;) {
                    if (getbuf(F) && !strncmp(buf, keywords[K_FACTION], strlen(keywords[K_FACTION])))
                        goto NEXTPLAYER;

                    if (buf[0] == '\f' || buf[0] == '#')
                        goto DONEPLAYER;

                    if (!_strcmpl(igetstr(buf), "unit")) {
                      NEXTUNIT:
                        i = geti();
                        u = findunitg(i);

                        if (u && u->faction == f) {
                            SP = &u->orders;
                            u->faction->lastorders = turn;

                            for (;;) {
                                if (getbuf(F)) {

                                if (!_strcmpl(igetstr(buf), "unit")) {
                                    *SP = 0;
                                    goto NEXTUNIT;
                                }

                                if (!strncmp(buf, keywords[K_FACTION], strlen(keywords[K_FACTION]))) {
                                    *SP = 0;
                                    goto NEXTPLAYER;
                                }

                                if (buf[0] == '\f'
                                    || buf[0] == '#') {
                                    *SP = 0;
                                    goto DONEPLAYER;
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
                                    S = makestrlist(buf2);
                                    addlist2(SP, S);
                                }
        }
          }
                        } else {
                            sprintf(buf,
                                    "Unit %d is not one of your units.",
                                    i);
                            addstrlist(&f->mistakes, buf);
                        }
                    }
                }
            } else
                printf("Invalid faction number %d.\n", i);
        }

      DONEPLAYER:
        getbuf(F);
    }

    fclose(F);
}

void makeworld(void)
{
    int x, y, minx, miny, maxx, maxy;
    region *r;

    minx = INT_MAX;
    maxx = INT_MIN;
    miny = INT_MAX;
    maxy = INT_MIN;

    for (r = regions; r; r = r->next) {
        minx = MIN(minx, r->x);
        maxx = MAX(maxx, r->x);
        miny = MIN(miny, r->y);
        maxy = MAX(maxy, r->y);
    }

    for (x=minx-BLOCKBORDER;x<=maxx+BLOCKBORDER;++x) {
        for (y=miny-BLOCKBORDER;y<=maxy+BLOCKBORDER;++y) {
            r = findregion(x, y);
            if (!r) {
                r = create_region(x, y, T_OCEAN);
            }
        }
    }
    connectregions();
}

void writemap(FILE * F)
{
    int x, y, minx, miny, maxx, maxy;
    region *r;

    minx = INT_MAX;
    maxx = INT_MIN;
    miny = INT_MAX;
    maxy = INT_MIN;

    for (r = regions; r; r = r->next) {
        minx = MIN(minx, r->x);
        maxx = MAX(maxx, r->x);
        miny = MIN(miny, r->y);
        maxy = MAX(maxy, r->y);
    }

    for (y = miny; y <= maxy; y++) {
        memset(buf, ' ', sizeof buf);
        buf[maxx - minx + 1] = 0;

        for (r = regions; r; r = r->next)
            if (r->y == y)
                buf[r->x - minx] = ".+MFS"[r->terrain];

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
    faction *f;
    region *r;
    unit *u;

    F = cfopen("summary", "w");
    puts("Writing summary file...");

    inhabitedregions = 0;
    peasants = 0;
    peasantmoney = 0;

    nunits = 0;
    playerpop = 0;
    playermoney = 0;

    for (r = regions; r; r = r->next)
        if (r->peasants || r->units) {
            inhabitedregions++;
            peasants += r->peasants;
            peasantmoney += r->money;

            for (u = r->units; u; u = u->next) {
                nunits++;
                playerpop += u->number;
                playermoney += u->money;

                u->faction->nunits++;
                u->faction->number += u->number;
                u->faction->money += u->money;
            }
        }

    fprintf(F, "Summary file for Atlantis, %s\n\n", gamedate());

    fprintf(F, "Regions: %d\n", listlen(regions));
    fprintf(F, "Inhabited Regions: %d\n\n", inhabitedregions);

    fprintf(F, "Factions: %d\n", listlen(factions));
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

    for (f = factions; f; f = f->next) {
        fprintf(F, "%s, units: %d, number: %d, $%d, address: %s, loc: %d,%d\n",
        factionid(f), f->nunits, f->number, f->money, faction_getaddr(f), f->origin_x, f->origin_y);
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

void centrestrlist(FILE * F, char *s, strlist * S)
{
    if (S) {
        rnl(F);
        centre(F, s);
        rnl(F);

        rpstrlist(F, S);
    }
}

void rparagraph(FILE * F, char *s, int indent, int mark)
{
    strlist *S;

    S = 0;
    sparagraph(&S, s, indent, mark);
    rpstrlist(F, S);
    freelist(S);
}

void rpunit(FILE * F, faction * f, region * r, unit * u, int indent, int battle)
{
    strlist *S;

    S = 0;
    spunit(&S, f, r, u, indent, battle);
    rpstrlist(F, S);
    freelist(S);
}

void report(faction * f)
{
    FILE * F;
    int i;
    int dh;
    int anyunits;
    rfaction *rf;
    region *r;
    building *b;
    ship *sh;
    unit *u;
    strlist *S;

    sprintf(buf, "reports/%d-%d.r", turn, f->no);
    F = cfopen(buf, "w");

    printf("Writing report for %s...\n", factionid(f));

    centre(F, "Atlantis Turn Report");
    centre(F, factionid(f));
    centre(F, gamedate());

    centrestrlist(F, "Mistakes", f->mistakes);
    centrestrlist(F, "Messages", f->messages);

    if (f->battles || f->events) {
        rnl(F);
        centre(F, "Events During Turn");
        rnl(F);

        for (S = f->battles; S; S = S->next) {
            rps(S->s);
            rnl(F);
        }

        if (f->battles && f->events)
            rnl(F);

        for (S = f->events; S; S = S->next) {
            rps(S->s);
            rnl(F);
        }
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

    if (f->allies) {
        dh = 0;
        strcpy(buf, "You are allied to ");

        for (rf = f->allies; rf; rf = rf->next) {
            if (dh)
                scat(", ");
            dh = 1;
            scat(factionid(rf->faction));
        }

        scat(".");
        rnl(F);
        rparagraph(F, buf, 0, 0);
    }

    anyunits = 0;

    for (r = regions; r; r = r->next) {
        int d;
        for (u = r->units; u; u = u->next)
            if (u->faction == f)
                break;
        if (!u)
            continue;

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

        for (b = r->buildings; b; b = b->next) {
            sprintf(buf, "%s, size %d", buildingid(b), b->size);

            if (b->display[0]) {
                scat("; ");
                scat(b->display);
            }

            scat(".");

            if (dh)
                rnl(F);

            dh = 1;

            rparagraph(F, buf, 4, 0);

            for (u = r->units; u; u = u->next)
                if (u->building == b && u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                    break;
                }

            for (u = r->units; u; u = u->next)
                if (u->building == b && !u->owner)
                    rpunit(F, f, r, u, 8, 0);
        }

        for (sh = r->ships; sh; sh = sh->next) {
            sprintf(buf, "%s, %s", shipid(sh), shiptypenames[sh->type]);
            if (sh->left)
                scat(", under construction");

            if (sh->display[0]) {
                scat("; ");
                scat(sh->display);
            }

            scat(".");

            if (dh)
                rnl(F);

            dh = 1;

            rparagraph(F, buf, 4, 0);

            for (u = r->units; u; u = u->next)
                if (u->ship == sh && u->owner) {
                    rpunit(F, f, r, u, 8, 0);
                    break;
                }

            for (u = r->units; u; u = u->next)
                if (u->ship == sh && !u->owner)
                    rpunit(F, f, r, u, 8, 0);
        }

        dh = 0;

        for (u = r->units; u; u = u->next)
            if (!u->building && !u->ship && cansee(f, r, u)) {
                if (!dh && (r->buildings || r->ships)) {
                    rnl(F);
                    dh = 1;
                }

                rpunit(F, f, r, u, 4, 0);
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

void reports(void)
{
    FILE * F;
    faction *f;

    _mkdir("reports");

    for (f = factions; f; f = f->next)
        report(f);

    F = cfopen("send", "w");
    puts("Writing send file...");

    for (f = factions; f; f = f->next) {
        const char * addr = faction_getaddr(f);
        if (addr) {
            fprintf(F, "mail %d-%d.r\n", turn, f->no);
            fprintf(F, "in%%\"%s\"\n", addr);
            fprintf(F, "Atlantis Report for %s\n", gamedate());
        }
    }

    fclose(F);

    F = cfopen("maillist", "w");
    puts("Writing maillist file...");

    for (f = factions; f; f = f->next) {
        const char * addr = faction_getaddr(f);
        if (addr) {
            fprintf(F, "%s\n", addr);
        }
    }

    fclose(F);
}

int reportcasualtiesdh;

void reportcasualties(unit * u)
{
    if (!u->dead)
        return;

    if (!reportcasualtiesdh) {
        battlerecord("");
        reportcasualtiesdh = 1;
    }

    if (u->number == 1)
        sprintf(buf, "%s is dead.", unitid(u));
    else if (u->dead == u->number)
        sprintf(buf, "%s is wiped out.", unitid(u));
    else
        sprintf(buf, "%s loses %d.", unitid(u), u->dead);
    battlerecord(buf);
}

int norders;
order *oa;

void expandorders(region * r, order * orders)
{
    int i, j;
    unit *u;
    order *o;

    for (u = r->units; u; u = u->next)
        u->n = -1;

    norders = 0;

    for (o = orders; o; o = o->next)
        norders += o->qty;

    oa = cmalloc(norders * sizeof(order));

    i = 0;

    for (o = orders; o; o = o->next)
        for (j = o->qty; j; j--) {
            oa[i].unit = o->unit;
            oa[i].unit->n = 0;
            i++;
        }

    freelist(orders);

    scramble(oa, norders, sizeof(order));
}

void removenullfactions(void)
{
    faction *f, *f2, *f3;
    rfaction *rf, *rf2;

    for (f = factions; f;) {
        f2 = f->next;

        if (!f->alive) {
            printf("Removing %s.\n", faction_getname(f));

            for (f3 = factions; f3; f3 = f3->next)
                for (rf = f3->allies; rf;) {
                    rf2 = rf->next;

                    if (rf->faction == f)
                        removelist(&f3->allies, rf);

                    rf = rf2;
                }

            freelist(f->allies);
            freelist(f->mistakes);
            freelist(f->messages);
            freelist(f->battles);
            freelist(f->events);

            removelist(&factions, f);
        }

        f = f2;
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

    for (u = r->units; u; u = u->next)
        if (u->ship == sh)
            n += itemweight(u) + horseweight(u) + (u->number * 10);

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
    region *r;
    unit *u;

    n = 0;

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            if (u->skills[SK_MAGIC] && u->faction == f)
                n += u->number;

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
        if (!r->connect[dir]) {
            makeblock(x, y);
            assert(r->connect[dir]);
        }
        return r->connect[dir];
    }
    return 0;
}

void processorders(void)
{
    int i, j, k;
    int n, m;
    int nfactions;
    int fno;
    int winnercasualties;
    int deadpeasants;
    int taxed;
    int availmoney;
    int teaching;
    int maxtactics[2];
    int leader[2];
    int lmoney;
    int dh;
    static int litems[MAXITEMS];
    const char *s, *s2;
    char *sx, *sn;
    faction *f, *f2, **fa;
    rfaction *rf;
    region *r, *r2;
    building *b;
    ship *sh;
    unit *u, *u2, *u3, *u4;
    static unit *uv[100];
    troop *t, *troops, **tp;
    order *o, *taxorders, *recruitorders, *entertainorders, *workorders;
    static order *produceorders[MAXITEMS];
    strlist *S, *S2;

    /* FORM orders */

    puts("Processing FORM orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            for (S = u->orders; S;)
                switch (igetkeyword(S->s)) {
                case K_FORM:
                    u2 = createunit(r, u->faction);

                    u2->alias = geti();
                    if (u2->alias == 0)
                        u2->alias = geti();

                    u2->building = u->building;
                    u2->ship = u->ship;
                    u2->behind = u->behind;
                    u2->guard = u->guard;

                    S = S->next;

                    while (S) {
                        if (igetkeyword(S->s) == K_END) {
                            S = S->next;
                            break;
                        }

                        S2 = S->next;
                        translist(&u->orders, &u2->orders, S);
                        S = S2;
                    }

                    break;

                default:
                    S = S->next;
                }

    /* Instant orders - diplomacy etc. */

    puts("Processing instant orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case -1:
                    mistake2(u, S, "Order not recognized");
                    break;

                case K_ACCEPT:
                    togglerf(u, S, &u->faction->accept);
                    break;

                case K_ADDRESS:
                    s = getstr();

                    if (!s[0]) {
                        mistake2(u, S, "No address given");
                        break;
                    }

                    faction_setaddr(u->faction, s);

                    printf("%s is changing address to %s.\n",
                           faction_getname(u->faction), faction_getaddr(u->faction));
                    break;

                case K_ADMIT:
                    togglerf(u, S, &u->faction->admit);
                    break;

                case K_ALLY:
                    f = getfaction();

                    if (f == 0) {
                        mistake2(u, S, "Faction not found");
                        break;
                    }

                    if (f == u->faction)
                        break;

                    if (geti()) {
                        for (rf = u->faction->allies; rf; rf = rf->next)
                            if (rf->faction == f)
                                break;

                        if (!rf) {
                            rf = cmalloc(sizeof(rfaction));
                            rf->faction = f;
                            addlist(&u->faction->allies, rf);
                        }
                    } else
                        for (rf = u->faction->allies; rf; rf = rf->next)
                            if (rf->faction == f) {
                                removelist(&u->faction->allies, rf);
                                break;
                            }

                    break;

                case K_BEHIND:
                    u->behind = geti() != 0;
                    break;

                case K_COMBAT:
                    s = getstr();

                    if (!s[0]) {
                        u->combatspell = -1;
                        break;
                    }

                    i = findspell(s);

                    if (i < 0 || !cancast(u, i)) {
                        mistake2(u, S, "Spell not found");
                        break;
                    }

                    if (!iscombatspell[i]) {
                        mistake2(u, S, "Not a combat spell");
                        break;
                    }

                    u->combatspell = i;
                    break;

                case K_DISPLAY:
                    sn = 0;

                    switch (getkeyword()) {
                    case K_BUILDING:
                        if (!u->building) {
                            mistake2(u, S, "Not in a building");
                            break;
                        }

                        if (!u->owner) {
                            mistake2(u, S, "Building not owned by you");
                            break;
                        }

                        sn = u->building->display;
                        break;

                    case K_SHIP:
                        if (!u->ship) {
                            mistake2(u, S, "Not in a ship");
                            break;
                        }

                        if (!u->owner) {
                            mistake2(u, S, "Ship not owned by you");
                            break;
                        }

                        sn = u->ship->display;
                        break;

                    case K_UNIT:
                        unit_setdisplay(u, getstr());
                        break;

                    default:
                        mistake2(u, S, "Order not recognized");
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
                    if (geti() == 0)
                        u->guard = false;
                    break;

                case K_NAME:
                    sn = 0;

                    switch (getkeyword()) {
                    case K_BUILDING:
                        if (!u->building) {
                            mistake2(u, S, "Not in a building");
                            break;
                        }

                        if (!u->owner) {
                            mistake2(u, S, "Building not owned by you");
                            break;
                        }

                        sn = u->building->name;
                        break;

                    case K_FACTION:
                        faction_setname(u->faction, getstr());
                        break;

                    case K_SHIP:
                        if (!u->ship) {
                            mistake2(u, S, "Not in a ship");
                            break;
                        }

                        if (!u->owner) {
                            mistake2(u, S, "Ship not owned by you");
                            break;
                        }

                        sn = u->ship->name;
                        break;

                    case K_UNIT:
                        unit_setname(u, getstr());
                        break;

                    default:
                        mistake2(u, S, "Order not recognized");
                        break;
                    }

                    if (!sn)
                        break;

                    s2 = getstr();

                    if (!s2[0]) {
                        mistake2(u, S, "No name given");
                        break;
                    }

                    for (i = 0; s2[i]; i++)
                        if (s2[i] == '(')
                            break;

                    if (s2[i]) {
                        mistake2(u, S, "Names cannot contain brackets");
                        break;
                    }

                    nstrcpy(sn, s2, NAMESIZE);
                    break;

                case K_RESHOW:
                    i = getspell();

                    if (i < 0 || !u->faction->seendata[i]) {
                        mistake2(u, S, "Spell not found");
                        break;
                    }

                    u->faction->showdata[i] = 1;
                    break;
                }

    /* FIND orders */

    puts("Processing FIND orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_FIND:
                    f = getfaction();

                    if (f == 0) {
                        mistake2(u, S, "Faction not found");
                        break;
                    }

                    sprintf(buf, "The address of %s is %s.", factionid(f),
                            faction_getaddr(f));
                    sparagraph(&u->faction->messages, buf, 0, 0);
                    break;
                }

    /* Leaving and entering buildings and ships */

    puts("Processing leaving and entering orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_BOARD:
                    sh = getship(r);

                    if (!sh) {
                        mistake2(u, S, "Ship not found");
                        break;
                    }

                    if (!mayboard(r, u, sh)) {
                        mistake2(u, S, "Not permitted to board");
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
                        mistake2(u, S, "Building not found");
                        break;
                    }

                    if (!mayenter(r, u, b)) {
                        mistake2(u, S, "Not permitted to enter");
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
                        mistake2(u, S, "Ship is at sea");
                        break;
                    }

                    leave(r, u);
                    break;

                case K_PROMOTE:
                    u2 = getunit(r, u);

                    if (!u2) {
                        mistake2(u, S, "Unit not found");
                        break;
                    }

                    if (!u->building && !u->ship) {
                        mistake2(u, S,
                                 "No building or ship to transfer ownership of");
                        break;
                    }

                    if (!u->owner) {
                        mistake2(u, S, "Not owned by you");
                        break;
                    }

                    if (!accepts(u2, u)) {
                        mistake2(u, S, "Unit does not accept ownership");
                        break;
                    }

                    if (u->building) {
                        if (u2->building != u->building) {
                            mistake2(u, S, "Unit not in same building");
                            break;
                        }
                    } else if (u2->ship != u->ship) {
                        mistake2(u, S, "Unit not on same ship");
                        break;
                    }

                    u->owner = 0;
                    u2->owner = 1;
                    break;
                }

    /* Combat */

    puts("Processing ATTACK orders...");

    nfactions = listlen(factions);
    fa = cmalloc(nfactions * sizeof(faction *));

    for (r = regions; r; r = r->next) {
        /* Create randomly sorted list of factions */

        for (f = factions, i = 0; f; f = f->next, i++)
            fa[i] = f;
        scramble(fa, nfactions, sizeof(faction *));

        /* Handle each faction's attack orders */

        for (fno = 0; fno != nfactions; fno++) {
            f = fa[fno];

            for (u = r->units; u; u = u->next)
                if (u->faction == f)
                    for (S = u->orders; S; S = S->next)
                        if (igetkeyword(S->s) == K_ATTACK) {
                            u2 = getunit(r, u);

                            if (!u2 && !getunitpeasants) {
                                mistake2(u, S, "Unit not found");
                                continue;
                            }

                            if (u2 && u2->faction == f) {
                                mistake2(u, S, "One of your units");
                                continue;
                            }

                            if (isallied(u, u2)) {
                                mistake2(u, S, "An allied unit");
                                continue;
                            }

                            /* Draw up troops for the battle */

                            for (b = r->buildings; b; b = b->next)
                                b->sizeleft = b->size;

                            troops = 0;
                            tp = &troops;
                            left[0] = left[1] = 0;
                            infront[0] = infront[1] = 0;

                            /* If peasants are defenders */

                            if (!u2) {
                                for (i = r->peasants; i; i--) {
                                    t = cmalloc(sizeof(troop));
                                    memset(t, 0, sizeof(troop));
                                    addlist2(tp, t);
                                }

                                left[0] = r->peasants;
                                infront[0] = r->peasants;
                            }

                            /* What units are involved? */

                            for (f2 = factions; f2; f2 = f2->next)
                                f2->attacking = 0;

                            for (u3 = r->units; u3; u3 = u3->next)
                                for (S2 = u3->orders; S2; S2 = S2->next)
                                    if (igetkeyword(S2->s) == K_ATTACK) {
                                        u4 = getunit(r, u3);

                                        if ((getunitpeasants && !u2) ||
                                            (u4
                                             && u4->faction == u2->faction
                                             && !isallied(u3, u4))) {
                                            u3->faction->attacking = 1;
                                            S2->s[0] = 0;
                                            break;
                                        }
                                    }

                            for (u3 = r->units; u3; u3 = u3->next) {
                                u3->side = -1;

                                if (!u3->number)
                                    continue;

                                if (u3->faction->attacking) {
                                    u3->side = 1;
                                    tp = maketroops(tp, u3, r->terrain);
                                } else if (isallied(u3, u2)) {
                                    u3->side = 0;
                                    tp = maketroops(tp, u3, r->terrain);
                                }
                            }

                            *tp = 0;

                            /* If only one side shows up, cancel */

                            if (!left[0] || !left[1]) {
                                freelist(troops);
                                continue;
                            }

                            /* Set up array of troops */

                            ntroops = listlen(troops);
                            ta = cmalloc(ntroops * sizeof(troop));
                            for (t = troops, i = 0; t; t = t->next, i++)
                                ta[i] = *t;
                            freelist(troops);
                            scramble(ta, ntroops, sizeof(troop));

                            initial[0] = left[0];
                            initial[1] = left[1];
                            shields[0] = 0;
                            shields[1] = 0;
                            runeswords[0] = 0;
                            runeswords[1] = 0;

                            lmoney = 0;
                            memset(litems, 0, sizeof litems);

                            /* Initial attack message */

                            for (f2 = factions; f2; f2 = f2->next) {
                                f2->seesbattle = ispresent(f2, r);
                                if (f2->seesbattle && f2->battles)
                                    addstrlist(&f2->battles, "");
                            }

                            if (u2)
                                strcpy(buf2, unitid(u2));
                            else
                                strcpy(buf2, "the peasants");
                            for (f2 = factions; f2; f2 = f2->next) {
                                if (f2->seesbattle) {
                                    sprintf(buf, "%s attacks %s in %s!", unitid(u),
                                            buf2, regionid(r, f2));
                                    sparagraph(&f2->battles, buf, 0, 0);
                                }
                            }

                            /* List sides */

                            battlerecord("");

                            battlepunit(r, u);

                            for (u3 = r->units; u3; u3 = u3->next)
                                if (u3->side == 1 && u3 != u)
                                    battlepunit(r, u3);

                            battlerecord("");

                            if (u2)
                                battlepunit(r, u2);
                            else {
                                sprintf(buf, "Peasants, number: %d",
                                        r->peasants);
                                for (f2 = factions; f2; f2 = f2->next)
                                    if (f2->seesbattle)
                                        sparagraph(&f2->battles, buf, 4,
                                                   '-');
                            }

                            for (u3 = r->units; u3; u3 = u3->next)
                                if (u3->side == 0 && u3 != u2)
                                    battlepunit(r, u3);

                            battlerecord("");

                            /* Does one side have an advantage in tactics? */

                            maxtactics[0] = 0;
                            maxtactics[1] = 0;

                            for (i = 0; i != ntroops; i++)
                                if (ta[i].unit) {
                                    j = effskill(ta[i].unit, SK_TACTICS);

                                    if (maxtactics[ta[i].side] < j) {
                                        leader[ta[i].side] = i;
                                        maxtactics[ta[i].side] = j;
                                    }
                                }

                            attacker.side = -1;
                            if (maxtactics[0] > maxtactics[1])
                                attacker.side = 0;
                            if (maxtactics[1] > maxtactics[0])
                                attacker.side = 1;

                            /* Better leader gets free round of attacks */

                            if (attacker.side >= 0) {
                                /* Note the fact in the battle report */

                                if (attacker.side)
                                    sprintf(buf,
                                            "%s gets a free round of attacks!",
                                            unitid(u));
                                else if (u2)
                                    sprintf(buf,
                                            "%s gets a free round of attacks!",
                                            unitid(u2));
                                else
                                    sprintf(buf,
                                            "The peasants get a free round of attacks!");
                                battlerecord(buf);

                                /* Number of troops to attack */

                                toattack[attacker.side] = 0;

                                for (i = 0; i != ntroops; i++) {
                                    ta[i].attacked = 1;

                                    if (ta[i].side == attacker.side) {
                                        ta[i].attacked = 0;
                                        toattack[attacker.side]++;
                                    }
                                }

                                /* Do round of attacks */

                                do
                                    doshot();
                                while (toattack[attacker.side]
                                       && left[defender.side]);
                            }

                            /* Handle main body of battle */

                            toattack[0] = 0;
                            toattack[1] = 0;

                            while (left[defender.side]) {
                                /* End of a round */

                                if (toattack[0] == 0 && toattack[1] == 0)
                                    for (i = 0; i != ntroops; i++) {
                                        ta[i].attacked = 1;

                                        if (!ta[i].status) {
                                            ta[i].attacked = 0;
                                            toattack[ta[i].side]++;
                                        }
                                    }

                                doshot();
                            }

                            /* Report on winner */

                            if (attacker.side)
                                sprintf(buf, "%s wins the battle!",
                                        unitid(u));
                            else if (u2)
                                sprintf(buf, "%s wins the battle!",
                                        unitid(u2));
                            else
                                sprintf(buf,
                                        "The peasants win the battle!");
                            battlerecord(buf);

                            /* Has winner suffered any casualties? */

                            winnercasualties = 0;

                            for (i = 0; i != ntroops; i++)
                                if (ta[i].side == attacker.side
                                    && ta[i].status) {
                                    winnercasualties = 1;
                                    break;
                                }

                            /* Can wounded be healed? */

                            n = 0;

                            for (i = 0; i != ntroops &&
                                 n != initial[attacker.side] -
                                 left[attacker.side]; i++)
                                if (!ta[i].status && ta[i].canheal) {
                                    k = lovar(50 * (1 + ta[i].power));
                                    k = MIN(k, initial[attacker.side] -
                                            left[attacker.side] - n);

                                    sprintf(buf, "%s heals %d wounded.",
                                            unitid(ta[i].unit), k);
                                    battlerecord(buf);

                                    n += k;
                                }

                            while (--n >= 0) {
                                do
                                    i = rnd() % ntroops;
                                while (!ta[i].status
                                       || ta[i].side != attacker.side);

                                ta[i].status = 0;
                            }

                            /* Count the casualties */

                            deadpeasants = 0;

                            for (u3 = r->units; u3; u3 = u3->next)
                                u3->dead = 0;

                            for (i = 0; i != ntroops; i++)
                                if (ta[i].unit)
                                    ta[i].unit->dead += ta[i].status;
                                else
                                    deadpeasants += ta[i].status;

                            /* Report the casualties */

                            reportcasualtiesdh = 0;

                            if (attacker.side) {
                                reportcasualties(u);

                                for (u3 = r->units; u3; u3 = u3->next)
                                    if (u3->side == 1 && u3 != u)
                                        reportcasualties(u3);
                            } else {
                                if (u2)
                                    reportcasualties(u2);
                                else if (deadpeasants) {
                                    battlerecord("");
                                    reportcasualtiesdh = 1;
                                    sprintf(buf, "The peasants lose %d.",
                                            deadpeasants);
                                    battlerecord(buf);
                                }

                                for (u3 = r->units; u3; u3 = u3->next)
                                    if (u3->side == 0 && u3 != u2)
                                        reportcasualties(u3);
                            }

                            /* Dead peasants */

                            k = r->peasants - deadpeasants;

                            j = distribute(r->peasants, k, r->money);
                            lmoney += r->money - j;
                            r->money = j;

                            r->peasants = k;

                            /* Adjust units */

                            for (u3 = r->units; u3; u3 = u3->next) {
                                k = u3->number - u3->dead;

                                /* Redistribute items and skills */

                                if (u3->side == defender.side) {
                                    j = distribute(u3->number, k,
                                                   u3->money);
                                    lmoney += u3->money - j;
                                    u3->money = j;

                                    for (i = 0; i != MAXITEMS; i++) {
                                        j = distribute(u3->number, k,
                                                       u3->items[i]);
                                        litems[i] += u3->items[i] - j;
                                        u3->items[i] = j;
                                    }
                                }

                                for (i = 0; i != MAXSKILLS; i++)
                                    u3->skills[i] =
                                        distribute(u3->number, k,
                                                   u3->skills[i]);

                                /* Adjust unit numbers */

                                u3->number = k;

                                /* Need this flag cleared for reporting of loot */

                                u3->n = 0;
                            }

                            /* Distribute loot */

                            for (n = lmoney; n; n--) {
                                do
                                    j = rnd() % ntroops;
                                while (ta[j].status
                                       || ta[j].side != attacker.side);

                                if (ta[j].unit) {
                                    ta[j].unit->money++;
                                    ta[j].unit->n++;
                                } else
                                    r->money++;
                            }

                            for (i = 0; i != MAXITEMS; i++)
                                for (n = litems[i]; n; n--)
                                    if (i <= I_STONE || rnd() & 1) {
                                        do
                                            j = rnd() % ntroops;
                                        while (ta[j].status
                                               || ta[j].side !=
                                               attacker.side);

                                        if (ta[j].unit) {
                                            if (!ta[j].unit->litems) {
                                                ta[j].unit->litems =
                                                    cmalloc(MAXITEMS *
                                                            sizeof(int));
                                                memset(ta[j].unit->litems,
                                                       0,
                                                       MAXITEMS *
                                                       sizeof(int));
                                            }

                                            ta[j].unit->items[i]++;
                                            ta[j].unit->litems[i]++;
                                        }
                                    }

                            /* Report loot */

                            for (f2 = factions; f2; f2 = f2->next)
                                f2->dh = 0;

                            for (u3 = r->units; u3; u3 = u3->next)
                                if (u3->n || u3->litems) {
                                    sprintf(buf, "%s finds ", unitid(u3));
                                    dh = 0;

                                    if (u3->n) {
                                        scat("$");
                                        icat(u3->n);
                                        dh = 1;
                                    }

                                    if (u3->litems) {
                                        for (i = 0; i != MAXITEMS; i++)
                                            if (u3->litems[i]) {
                                                if (dh)
                                                    scat(", ");
                                                dh = 1;

                                                icat(u3->litems[i]);
                                                scat(" ");

                                                if (u3->litems[i] == 1)
                                                    scat(itemnames[0][i]);
                                                else
                                                    scat(itemnames[1][i]);
                                            }

                                        free(u3->litems);
                                        u3->litems = 0;
                                    }

                                    if (!u3->faction->dh) {
                                        addbattle(u3->faction, "");
                                        u3->faction->dh = 1;
                                    }

                                    scat(".");
                                    addbattle(u3->faction, buf);
                                }

                            /* Does winner get combat experience? */

                            if (winnercasualties) {
                                if (maxtactics[attacker.side] &&
                                    !ta[leader[attacker.side]].status)
                                    ta[leader[attacker.side]].
                                        unit->skills[SK_TACTICS] +=
                                        COMBATEXP;

                                for (i = 0; i != ntroops; i++)
                                    if (ta[i].unit &&
                                        !ta[i].status &&
                                        ta[i].side == attacker.side)
                                        switch (ta[i].weapon) {
                                        case I_SWORD:
                                            ta[i].unit->skills[SK_SWORD] +=
                                                COMBATEXP;
                                            break;

                                        case I_CROSSBOW:
                                            ta[i].unit->
                                                skills[SK_CROSSBOW] +=
                                                COMBATEXP;
                                            break;

                                        case I_LONGBOW:
                                            ta[i].unit->
                                                skills[SK_LONGBOW] +=
                                                COMBATEXP;
                                            break;
                                        }
                            }

                            free(ta);
                        }
        }
    }

    free(fa);

    /* Economic orders */

    puts("Processing economic orders...");

    for (r = regions; r; r = r->next) {
        taxorders = 0;
        recruitorders = 0;

        /* DEMOLISH, GIVE, PAY, SINK orders */

        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_DEMOLISH:
                    if (!u->building) {
                        mistake2(u, S, "Not in a building");
                        break;
                    }

                    if (!u->owner) {
                        mistake2(u, S, "Building not owned by you");
                        break;
                    }

                    b = u->building;

                    for (u2 = r->units; u2; u2 = u2->next)
                        if (u2->building == b) {
                            u2->building = 0;
                            u2->owner = 0;
                        }

                    sprintf(buf, "%s demolishes %s.", unitid(u),
                            buildingid(b));
                    reportevent(r, buf);

                    removelist(&r->buildings, b);
                    break;

                case K_GIVE:
                    u2 = getunit(r, u);

                    if (!u2 && !getunit0) {
                        mistake2(u, S, "Unit not found");
                        break;
                    }

                    if (u2 && !accepts(u2, u)) {
                        mistake2(u, S, "Unit does not accept your gift");
                        break;
                    }

                    s = getstr();
                    i = findspell(s);

                    if (i >= 0) {
                        if (!u2) {
                            mistake2(u, S, "Unit not found");
                            break;
                        }

                        if (!u->spells[i]) {
                            mistake2(u, S, "Spell not found");
                            break;
                        }

                        if (spelllevel[i] >
                            (effskill(u2, SK_MAGIC) + 1) / 2) {
                            mistake2(u, S,
                                     "Recipient is not able to learn that spell");
                            break;
                        }

                        u2->spells[i] = 1;

                        sprintf(buf, "%s gives ", unitid(u));
                        scat(unitid(u2));
                        scat(" the ");
                        scat(spellnames[i]);
                        scat(" spell.");
                        addevent(u->faction, buf);
                        if (u->faction != u2->faction)
                            addevent(u2->faction, buf);

                        if (!u2->faction->seendata[i]) {
                            u2->faction->seendata[i] = true;
                            u2->faction->showdata[i] = true;
                        }
                    } else {
                        n = atoip(s);
                        i = getitem();

                        if (i < 0) {
                            mistake2(u, S, "Item not recognized");
                            break;
                        }

                        if (n > u->items[i])
                            n = u->items[i];

                        if (n == 0) {
                            mistake2(u, S, "Item not available");
                            break;
                        }

                        u->items[i] -= n;

                        if (!u2) {
                            if (n == 1)
                                sprintf(buf, "%s discards 1 %s.",
                                        unitid(u), itemnames[0][i]);
                            else
                                sprintf(buf, "%s discards %d %s.",
                                        unitid(u), n, itemnames[1][i]);
                            addevent(u->faction, buf);
                            break;
                        }

                        u2->items[i] += n;

                        sprintf(buf, "%s gives ", unitid(u));
                        scat(unitid(u2));
                        scat(" ");
                        if (n == 1) {
                            scat("1 ");
                            scat(itemnames[0][i]);
                        } else {
                            icat(n);
                            scat(" ");
                            scat(itemnames[1][i]);
                        }
                        scat(".");
                        addevent(u->faction, buf);
                        if (u->faction != u2->faction)
                            addevent(u2->faction, buf);
                    }

                    break;

                case K_PAY:
                    u2 = getunit(r, u);

                    if (!u2 && !getunit0 && !getunitpeasants) {
                        mistake2(u, S, "Unit not found");
                        break;
                    }

                    n = geti();

                    if (n > u->money)
                        n = u->money;

                    if (n == 0) {
                        mistake2(u, S, "No money available");
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
                    } else if (getunitpeasants) {
                        r->money += n;

                        sprintf(buf, "%s pays the peasants $%d.",
                                unitid(u), n);
                    } else
                        sprintf(buf, "%s discards $%d.", unitid(u), n);

                    addevent(u->faction, buf);
                    break;

                case K_SINK:
                    if (!u->ship) {
                        mistake2(u, S, "Not on a ship");
                        break;
                    }

                    if (!u->owner) {
                        mistake2(u, S, "Ship not owned by you");
                        break;
                    }

                    if (r->terrain == T_OCEAN) {
                        mistake2(u, S, "Ship is at sea");
                        break;
                    }

                    sh = u->ship;

                    for (u2 = r->units; u2; u2 = u2->next)
                        if (u2->ship == sh) {
                            u2->ship = 0;
                            u2->owner = 0;
                        }

                    sprintf(buf, "%s sinks %s.", unitid(u), shipid(sh));
                    reportevent(r, buf);

                    removelist(&r->ships, sh);
                    break;
                }

        /* TRANSFER orders */

        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_TRANSFER:
                    u2 = getunit(r, u);

                    if (u2) {
                        if (!accepts(u2, u)) {
                            mistake2(u, S,
                                     "Unit does not accept your gift");
                            break;
                        }
                    } else if (!getunitpeasants) {
                        mistake2(u, S, "Unit not found");
                        break;
                    }

                    n = atoip(getstr());

                    if (n > u->number)
                        n = u->number;

                    if (n == 0) {
                        mistake2(u, S, "No people available");
                        break;
                    }

                    if (u->skills[SK_MAGIC] && u2) {
                        k = magicians(u2->faction);
                        if (u2->faction != u->faction)
                            k += n;
                        if (!u2->skills[SK_MAGIC])
                            k += u2->number;

                        if (k > 3) {
                            mistake2(u, S, "Only 3 magicians per faction");
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

        /* TAX orders */

        for (u = r->units; u; u = u->next) {
            taxed = 0;

            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_TAX:
                    if (taxed)
                        break;

                    n = armedmen(u);

                    if (!n) {
                        mistake2(u, S,
                                 "Unit is not armed and combat trained");
                        break;
                    }

                    for (u2 = r->units; u2; u2 = u2->next)
                        if (u2->guard && u2->number && !admits(u2, u)) {
                            sprintf(buf, "%s is on guard", unit_getname(u2));
                            mistake2(u, S, buf);
                            break;
                        }

                    if (u2)
                        break;

                    o = cmalloc(sizeof(order));
                    o->qty = n * TAXINCOME;
                    o->unit = u;
                    addlist(&taxorders, o);
                    taxed = 1;
                    break;
                }
        }

        /* Do taxation */

        for (u = r->units; u; u = u->next)
            u->n = -1;

        norders = 0;

        for (o = taxorders; o; o = o->next)
            norders += o->qty / 10;

        oa = cmalloc(norders * sizeof(order));

        i = 0;

        for (o = taxorders; o; o = o->next)
            for (j = o->qty / 10; j; j--) {
                oa[i].unit = o->unit;
                oa[i].unit->n = 0;
                i++;
            }

        freelist(taxorders);

        scramble(oa, norders, sizeof(order));

        for (i = 0; i != norders && r->money > 10; i++, r->money -= 10) {
            oa[i].unit->money += 10;
            oa[i].unit->n += 10;
        }

        free(oa);

        for (u = r->units; u; u = u->next)
            if (u->n >= 0) {
                sprintf(buf, "%s collects $%d in taxes.", unitid(u), u->n);
                addevent(u->faction, buf);
            }

        /* GUARD 1, RECRUIT orders */

        for (u = r->units; u; u = u->next) {
            availmoney = u->money;

            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_GUARD:
                    if (geti())
                        u->guard = true;
                    break;

                case K_RECRUIT:
                    if (availmoney < RECRUITCOST)
                        break;

                    n = geti();

                    if (u->skills[SK_MAGIC]
                        && magicians(u->faction) + n > 3) {
                        mistake2(u, S, "Only 3 magicians per faction");
                        break;
                    }

                    n = MIN(n, availmoney / RECRUITCOST);

                    o = cmalloc(sizeof(order));
                    o->qty = n;
                    o->unit = u;
                    addlist(&recruitorders, o);

                    availmoney -= o->qty * RECRUITCOST;
                    break;
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

        free(oa);

        for (u = r->units; u; u = u->next)
            if (u->n >= 0) {
                sprintf(buf, "%s recruits %d.", unitid(u), u->n);
                addevent(u->faction, buf);
            }
    }

    /* QUIT orders */

    puts("Processing QUIT orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next)
            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
                case K_QUIT:
                    if (geti() != u->faction->no) {
                        mistake2(u, S, "Correct faction number not given");
                        break;
                    }

                    destroyfaction(u->faction);
                    break;
                }

    /* Remove players who haven't sent in orders */

    for (f = factions; f; f = f->next)
        if (turn - f->lastorders > ORDERGAP)
            destroyfaction(f);

    /* Clear away debris of destroyed factions */

    removeempty();
    removenullfactions();

    /* Set production orders */

    puts("Setting production orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u; u = u->next) {
            strcpy(u->thisorder, u->lastorder);

            for (S = u->orders; S; S = S->next)
                switch (igetkeyword(S->s)) {
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
                    nstrcpy(u->thisorder, S->s, sizeof u->thisorder);
                    break;
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

    /* MOVE orders */

    puts("Processing MOVE orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u;) {
            u2 = u->next;

            switch (igetkeyword(u->thisorder)) {
            case K_MOVE:
                r2 = movewhere(r);

                if (!r2) {
                    mistakeu(u, "Direction not recognized");
                    break;
                }

                if (r->terrain == T_OCEAN) {
                    mistakeu(u, "Currently at sea");
                    break;
                }

                if (r2->terrain == T_OCEAN) {
                    sprintf(buf, "%s discovers that (%d,%d) is ocean.",
                            unitid(u), r2->x, r2->y);
                    addevent(u->faction, buf);
                    break;
                }

                if (!canmove(u)) {
                    mistakeu(u, "Carrying too much weight to move");
                    break;
                }

                leave(r, u);
                translist(&r->units, &r2->units, u);
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
            }

            u = u2;
        }

    /* SAIL orders */

    puts("Processing SAIL orders...");

    for (r = regions; r; r = r->next)
        for (u = r->units; u;) {
            u2 = u->next;

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

                translist(&r->ships, &r2->ships, u->ship);

                for (u2 = r->units; u2;) {
                    u3 = u2->next;

                    if (u2->ship == u->ship) {
                        translist(&r->units, &r2->units, u2);
                        u2->thisorder[0] = 0;
                    }

                    u2 = u3;
                }

                u->thisorder[0] = 0;
                break;
            }

            u = u2;
        }

    /* Do production orders */

    puts("Processing production orders...");

    for (r = regions; r; r = r->next) {
        ship_t stype;
        if (r->terrain == T_OCEAN)
            continue;

        entertainorders = 0;
        workorders = 0;
        memset(produceorders, 0, sizeof produceorders);

        for (u = r->units; u; u = u->next)
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
                        b = cmalloc(sizeof(building));
                        memset(b, 0, sizeof(building));

                        do {
                            b->no++;
                            sprintf(b->name, "Building %d", b->no);
                        }
                        while (findbuilding(b->no));

                        addlist(&r->buildings, b);

                        leave(r, u);
                        u->building = b;
                        u->owner = 1;
                    }

                    n = u->number * effskill(u, SK_BUILDING);
                    n = MIN(n, u->items[I_STONE]);
                    b->size += n;
                    u->items[I_STONE] -= n;

                    u->skills[SK_BUILDING] += n * 10;

                    sprintf(buf, "%s adds %d to %s.", unitid(u), n,
                            buildingid(b));
                    addevent(u->faction, buf);
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

                    sh = cmalloc(sizeof(ship));
                    memset(sh, 0, sizeof(ship));

                    sh->type = stype;
                    sh->left = shipcost[i];

                    do {
                        sh->no++;
                        sprintf(sh->name, "Ship %d", sh->no);
                    }
                    while (findship(sh->no));

                    addlist(&r->ships, sh);

                    leave(r, u);
                    u->ship = sh;
                    u->owner = 1;
                    goto BUILDSHIP;

                default:
                    mistakeu(u, "Order not recognized");
                }

                break;

            case K_ENTERTAIN:
                o = cmalloc(sizeof(order));
                o->unit = u;
                o->qty =
                    u->number * effskill(u,
                                         SK_ENTERTAINMENT) *
                    ENTERTAININCOME;
                addlist(&entertainorders, o);
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
                    o = cmalloc(sizeof(order));
                    o->unit = u;
                    o->qty = n * productivity[r->terrain][i];
                    addlist(&produceorders[i], o);
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
                                itemnames[0][i]);
                    else
                        sprintf(buf, "%s produces %d %s.", unitid(u), n,
                                itemnames[1][i]);
                    addevent(u->faction, buf);
                }

                u->skills[itemskill[i]] += u->number * PRODUCEEXP;
                break;

            case K_RESEARCH:
                if (effskill(u, SK_MAGIC) < 2) {
                    mistakeu(u, "Magic skill of at least 2 required");
                    break;
                }

                i = geti();

                if (i > effskill(u, SK_MAGIC) / 2) {
                    mistakeu(u,
                             "Insufficient Magic skill - highest available level researched");
                    i = 0;
                }

                if (i == 0)
                    i = effskill(u, SK_MAGIC) / 2;

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
                        j = rnd() % MAXSPELLS;
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

                do
                    uv[m++] = getunit(r, u);
                while (!getunit0 && m != 100);

                m--;

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
                o = cmalloc(sizeof(order));
                o->unit = u;
                o->qty = u->number * foodproductivity[r->terrain];
                addlist(&workorders, o);
                break;
            }

        /* Entertainment */

        expandorders(r, entertainorders);

        for (i = 0, n = r->money / ENTERTAINFRACTION; i != norders && n;
             i++, n--) {
            oa[i].unit->money++;
            r->money--;
            oa[i].unit->n++;
        }

        free(oa);

        for (u = r->units; u; u = u->next)
            if (u->n >= 0) {
                sprintf(buf, "%s earns $%d entertaining.", unitid(u),
                        u->n);
                addevent(u->faction, buf);

                u->skills[SK_ENTERTAINMENT] += 10 * u->number;
            }

        /* Food production */

        expandorders(r, workorders);

        for (i = 0, n = maxfoodoutput[r->terrain]; i != norders && n;
             i++, n--) {
            oa[i].unit->money++;
            oa[i].unit->n++;
        }

        free(oa);

        r->money += MIN(n, r->peasants * foodproductivity[r->terrain]);

        for (u = r->units; u; u = u->next)
            if (u->n >= 0) {
                sprintf(buf, "%s earns $%d performing manual labor.",
                        unitid(u), u->n);
                addevent(u->faction, buf);
            }

        /* Production of other primary commodities */

        for (i = 0; i != 4; i++) {
            expandorders(r, produceorders[i]);

            for (j = 0, n = maxoutput[r->terrain][i]; j != norders && n;
                 j++, n--) {
                oa[j].unit->items[i]++;
                oa[j].unit->n++;
            }

            free(oa);

            for (u = r->units; u; u = u->next)
                if (u->n >= 0) {
                    if (u->n == 1)
                        sprintf(buf, "%s produces 1 %s.", unitid(u),
                                itemnames[0][i]);
                    else
                        sprintf(buf, "%s produces %d %s.", unitid(u), u->n,
                                itemnames[1][i]);
                    addevent(u->faction, buf);
                }
        }
    }

    /* Study skills */

    puts("Processing STUDY orders...");

    for (r = regions; r; r = r->next)
        if (r->terrain != T_OCEAN)
            for (u = r->units; u; u = u->next)
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

    /* Ritual spells, and loss of spells where required */

    puts("Processing CAST orders...");

    for (r = regions; r; r = r->next) {
        for (u = r->units; u; u = u->next) {
            for (i = 0; i != MAXSPELLS; i++)
                if (u->spells[i]
                    && spelllevel[i] > (effskill(u, SK_MAGIC) + 1) / 2)
                    u->spells[i] = 0;

            if (u->combatspell >= 0 && !cancast(u, u->combatspell))
                u->combatspell = -1;
        }

        if (r->terrain != T_OCEAN)
            for (u = r->units; u; u = u->next)
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

                        for (f = factions; f; f = f->next) {
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
                        u2 = getunitg(r, u);

                        if (!u2) {
                            mistakeu(u, "Unit not found");
                            break;
                        }

                        if (!admits(u2, u)) {
                            mistakeu(u,
                                     "Target unit does not provide vector");
                            break;
                        }

                        for (r2 = regions;; r2 = r2->next) {
                            for (u3 = r2->units; u3; u3 = u3->next)
                                if (u3 == u2)
                                    break;

                            if (u3)
                                break;
                        }

                        n = cancast(u, SP_TELEPORT);
                        n = MIN(n, u->money / 50);

                        u->money -= n * 50;
                        u->skills[SK_MAGIC] += n * 10;

                        n *= 10000;

                        for (;;) {
                            u3 = getunit(r, u);

                            if (getunit0)
                                break;

                            if (!u3) {
                                mistakeu(u, "Unit not found");
                                continue;
                            }

                            if (!accepts(u3, u)) {
                                mistakeu(u,
                                         "Unit does not accept teleportation");
                                continue;
                            }

                            i = itemweight(u3) + horseweight(u3) +
                                (u->number * 10);

                            if (i > n) {
                                mistakeu(u, "Unit too heavy");
                                continue;
                            }

                            leave(r, u3);
                            n -= i;
                            translist(&r->units, &r2->units, u3);
                            u3->building = u2->building;
                            u3->ship = u2->ship;
                        }

                        sprintf(buf, "%s casts Teleport.", unitid(u));
                        addevent(u->faction, buf);
                        break;

                    default:
                        mistakeu(u, "Spell not usable with CAST command");
                    }

                    break;
                }
    }

    /* Population growth, dispersal and food consumption */

    puts("Processing demographics...");

    for (r = regions; r; r = r->next) {
        if (r->terrain != T_OCEAN) {
            for (n = r->peasants; n; n--)
                if (rnd() % 100 < POPGROWTH)
                    r->peasants++;

            n = r->money / MAINTENANCE;
            r->peasants = MIN(r->peasants, n);
            r->money -= r->peasants * MAINTENANCE;

            for (n = r->peasants; n; n--)
                if (rnd() % 100 < PEASANTMOVE) {
                    i = rnd() % MAXDIRECTIONS;

                    if (r->connect[i]->terrain != T_OCEAN) {
                        r->peasants--;
                        r->connect[i]->immigrants++;
                    }
                }
        }

        for (u = r->units; u; u = u->next) {
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

    for (r = regions; r; r = r->next)
        r->peasants += r->immigrants;

    /* Warn players who haven't sent in orders */

    for (f = factions; f; f = f->next)
        if (turn - f->lastorders == ORDERGAP)
            addstrlist(&f->messages,
                       "Please send orders next turn if you wish to continue playing.");
}

void rstrlist(HSTORAGE H, strlist ** SP)
{
    int n;
    strlist *S;

    store->r_int(H, &n);

    while (--n >= 0) {
        if (store->r_str(H, buf, sizeof(buf))==0) {
            S = makestrlist(buf);
            addlist2(SP, S);
        }
    }

    *SP = 0;
}

int readgame(void)
{
    FILE * F;
    HSTORAGE H;
    int i, n, n2;
    faction *f, **fp;
    rfaction *rf, **rfp;
    region *r, **rp;
    building *b, **bp;
    ship *sh, **shp;
    unit *u, **up;

    sprintf(buf, "data/%d", turn);
    F = cfopen(buf, "rb");
    H = store->begin(F, IO_READ);

    printf("Reading turn %d...\n", turn);

    store->r_int(H, &n);
    if (turn!=n) return -1;

    /* Read factions */

    store->r_int(H, &n);
    if (n<0) return -2;
    fp = &factions;

    while (--n >= 0) {
        int no;

        store->r_int(H, &no);
        f = create_faction(no);
        if (store->r_str(H, buf, sizeof(buf))==0) {
            faction_setname(f, buf[0] ? buf : 0);
        }
        if (store->r_str(H, buf, sizeof(buf))==0) {
            faction_setaddr(f, buf[0] ? buf : 0);
        }
        if (store->r_str(H, buf, sizeof(buf))==0) {
            faction_setpwhash(f, buf[0] ? buf : 0);
        }
        store->r_int(H, &f->lastorders);
        store->r_int(H, &f->origin_x);
        store->r_int(H, &f->origin_y);

        for (i = 0; i != MAXSPELLS; i++) {
            store->r_int(H, &no);
            f->showdata[i] = (no != 0);
        }

        store->r_int(H, &n2);
        if (n2<0) return -6;
        rfp = &f->allies;

        while (--n2 >= 0) {
            rf = cmalloc(sizeof(rfaction));
            store->r_int(H, &rf->factionno);
            addlist2(rfp, rf);
        }

        *rfp = 0;

        rstrlist(H, &f->mistakes);
        rstrlist(H, &f->messages);
        rstrlist(H, &f->battles);
        rstrlist(H, &f->events);

        addlist2(fp, f);
    }

    *fp = 0;

    /* Read regions */

    store->r_int(H, &n);
    if (n<0) return -3;
    rp = &regions;

    while (--n >= 0) {
        int x, y, n;
        char name[NAMESIZE];

        store->r_int(H, &x);
        store->r_int(H, &y);
        store->r_int(H, &n);
        r = create_region(x, y, (terrain_t)n);
        if (store->r_str(H, name, sizeof(name))==0) {
            region_setname(r, name[0] ? name : 0);
        }
        store->r_int(H, &r->peasants);
        store->r_int(H, &r->money);

        store->r_int(H, &n2);
        if (n2<0) return -4;
        bp = &r->buildings;

        while (--n2 >= 0) {
            b = cmalloc(sizeof(building));

            store->r_int(H, &b->no);
            store->r_str(H, b->name, sizeof(b->name));
            store->r_str(H, b->display, sizeof(b->display));
            store->r_int(H, &b->size);

            addlist2(bp, b);
        }

        *bp = 0;

        store->r_int(H, &n2);
        if (n2<0) return -5;
        shp = &r->ships;

        while (--n2 >= 0) {
            int no;
            store->r_int(H, &no);

            sh = cmalloc(sizeof(ship));
            sh->no = no;

            store->r_str(H, sh->name, sizeof(sh->name));
            store->r_str(H, sh->display, sizeof(sh->display));
            store->r_int(H, &no);
            sh->type = (ship_t)no;
            store->r_int(H, &sh->left);

            addlist2(shp, sh);
        }

        *shp = 0;

        store->r_int(H, &n2);
        if (n2<0) return -7;
        up = &r->units;

        addlist2(rp, r);

        while (--n2 >= 0) {
            char temp[DISPLAYSIZE];
            int no, fno;

            store->r_int(H, &no);
            store->r_int(H, &fno);
            u = create_unit(findfaction(fno), no);

            if (store->r_str(H, temp, sizeof(temp))==0) {
                unit_setname(u, temp[0] ? temp : 0);
            }
            if (store->r_str(H, temp, sizeof(temp))==0) {
                unit_setdisplay(u, temp[0] ? temp : 0);
            }
            store->r_int(H, &u->number);
            store->r_int(H, &u->money);

            store->r_int(H, &no);
            u->building = findbuilding(no);

            store->r_int(H, &no);
            u->ship = findship(no);

            store->r_int(H, &no);
            u->owner = no != 0;
            store->r_int(H, &no);
            u->behind = no != 0;
            store->r_int(H, &no);
            u->guard = no != 0;

            store->r_str(H, u->lastorder, sizeof(u->lastorder));
            store->r_int(H, &u->combatspell);

            for (i = 0; i != MAXSKILLS; i++) {
                store->r_int(H, &no);
                u->skills[i] = (skill_t)no;
            }
            for (i = 0; i != MAXITEMS; i++) {
                store->r_int(H, &no);
                u->items[i] = (item_t)no;
            }

            for (i = 0; i != MAXSPELLS; i++) {
                store->r_int(H, &no);
                u->spells[i] = (spell_t)no;
            }

            addlist2(up, u);
        }

        *up = 0;
    }

    *rp = 0;

    /* Get rid of stuff that was only relevant last turn */

    for (f = factions; f; f = f->next) {
        memset(f->showdata, 0, sizeof f->showdata);

        freelist(f->mistakes);
        freelist(f->messages);
        freelist(f->battles);
        freelist(f->events);

        f->mistakes = 0;
        f->messages = 0;
        f->battles = 0;
        f->events = 0;
    }

    /* Link rfaction structures */

    for (f = factions; f; f = f->next)
        for (rf = f->allies; rf; rf = rf->next)
            rf->faction = findfaction(rf->factionno);

    for (r = regions; r; r = r->next) {
        /* Initialize faction seendata values */

        for (u = r->units; u; u = u->next)
            for (i = 0; i != MAXSPELLS; i++)
                if (u->spells[i])
                    u->faction->seendata[i] = true;

        /* Check for alive factions */

        for (u = r->units; u; u = u->next)
            u->faction->alive = 1;
    }

    connectregions();
    fclose(F);
    return 0;
}

void wstrlist(HSTORAGE H, strlist * S)
{
    store->w_int(H, listlen(S));

    while (S) {
        store->w_str(H, S->s);
        S = S->next;
    }
}

void freestrlist(strlist ** slist) {
    while (*slist) {
        strlist * sl = *slist;
        *slist = sl->next;
        free(sl);
    }
}

void cleargame(void)
{
    while (regions) {
        region * r = regions;
        regions = r->next;

        free(r->name_);
        while (r->units) {
            unit * u = r->units;
            r->units = u->next;
            freestrlist(&u->orders);
            free(u->name_);
            free(u->display_);
            free(u);
        }
        while (r->ships) {
            ship * s = r->ships;
            r->ships = s->next;
            free(s);
        }
        while (r->buildings) {
            building * b = r->buildings;
            r->buildings = b->next;
            free(b);
        }
        free(r);
    }

    while (factions) {
        faction * f = factions;
        factions = f->next;

        free(f->name_);
        free(f->addr_);
        free(f->pwhash_);
        freestrlist(&f->messages);
        freestrlist(&f->battles);
        freestrlist(&f->events);
        freestrlist(&f->mistakes);
        while (f->allies) {
            rfaction * rf = f->allies;
            f->allies = rf->next;
            free(rf);
        }
        while (f->accept) {
            rfaction * rf = f->admit;
            f->admit = rf->next;
            free(rf);
        }
        while (f->admit) {
            rfaction * rf = f->admit;
            f->admit = rf->next;
            free(rf);
        }
        free(f);
    }
}

int writegame(void)
{
    HSTORAGE H;
    FILE * F;
    int i;
    faction *f;
    rfaction *rf;
    region *r;
    building *b;
    ship *sh;
    unit *u;

    sprintf(buf, "data/%d", turn);
    F = cfopen(buf, "wb");
    printf("Writing turn %d...\n", turn);

    H = store->begin(F, IO_WRITE);
    store->w_int(H, turn);

    /* Write factions */

    store->w_int(H, listlen(factions));

    for (f = factions; f; f = f->next) {
        store->w_int(H, f->no);
        store->w_str(H, faction_getname(f));
        store->w_str(H, faction_getaddr(f));
        store->w_str(H, faction_getpwhash(f));
        store->w_int(H, f->lastorders);
        store->w_int(H, f->origin_x);
        store->w_int(H, f->origin_y);

        for (i = 0; i != MAXSPELLS; i++) {
            store->w_int(H, f->showdata[i]);
        }

        store->w_int(H, listlen(f->allies));

        for (rf = f->allies; rf; rf = rf->next) {
            store->w_int(H, rf->faction->no);
        }

        wstrlist(H, f->mistakes);
        wstrlist(H, f->messages);
        wstrlist(H, f->battles);
        wstrlist(H, f->events);
    }

    /* Write regions */

    store->w_int(H, listlen(regions));

    for (r = regions; r; r = r->next) {
        store->w_int(H, r->x);
        store->w_int(H, r->y);
        store->w_int(H, r->terrain);
        store->w_str(H, region_getname(r));
        store->w_int(H, r->peasants);
        store->w_int(H, r->money);

        store->w_int(H, listlen(r->buildings));

        for (b = r->buildings; b; b = b->next) {
            store->w_int(H, b->no);
            store->w_str(H, b->name);
            store->w_str(H, b->display);
            store->w_int(H, b->size);
        }

        store->w_int(H, listlen(r->ships));

        for (sh = r->ships; sh; sh = sh->next) {
            store->w_int(H, sh->no);
            store->w_str(H, sh->name);
            store->w_str(H, sh->display);
            store->w_int(H, sh->type);
            store->w_int(H, sh->left);
        }

        store->w_int(H, listlen(r->units));

        for (u = r->units; u; u = u->next) {
            store->w_int(H, u->no);
            store->w_int(H, u->faction->no);
            store->w_str(H, unit_getname(u));
            store->w_str(H, unit_getdisplay(u));
            store->w_int(H, u->number);
            store->w_int(H, u->money);
            store->w_int(H, u->building ? u->building->no : 0);
            store->w_int(H, u->ship ? u->ship->no : 0);
            store->w_int(H, u->owner);
            store->w_int(H, u->behind);
            store->w_int(H, u->guard);
            store->w_str(H, u->lastorder);
            store->w_int(H, u->combatspell);

            for (i = 0; i != MAXSKILLS; i++) {
                store->w_int(H, u->skills[i]);
            }

            for (i = 0; i != MAXITEMS; i++) {
                store->w_int(H, u->items[i]);
            }

            for (i = 0; i != MAXSPELLS; i++) {
                store->w_int(H, u->spells[i]);
            }

        }
    }
    store->end(H);
    fclose(F);
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

int processturn(const char *orders)
{
    turn++;
    readorders(orders);
    processorders();
    reports();
    writesummary();
    writegame();
    return 0;
}

void createcontinent(void)
{
    int x, y;

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
