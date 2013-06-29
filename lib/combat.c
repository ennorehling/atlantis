#include "report.h"
#include "combat.h"
#include "items.h"
#include "skills.h"
#include "unit.h"
#include "building.h"
#include "region.h"
#include "faction.h"
#include "battle.h"
#include "parser.h"
#include "atlantis.h"

#include "rtl.h"

#include <quicklist.h>
#include <mtrand.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct troop {
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

static int ntroops;
static troop **ta;
static troop *attacker, *defender;
static int attacker_side, defender_side;
static int initial[2];
static int left[2];
static int infront[2];
static int toattack[2];
static int shields[2];
static int runeswords[2];

static void maketroops(quicklist **troops, unit * u, int terrain)
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
        t = (troop *)malloc(sizeof(troop));
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

        ql_push(troops, t);
    }
}

static void fisher_yates_ql(quicklist *ql, void ** a, int n)
{
    int i;

    if (n>0) {
        a[0] = ql_get(ql, 0);
        for (i = 1; i!=n; ++i) {
            int j = genrand_int31() % i;
            a[i] = a[j];
            a[j] = ql_get(ql, i);
        }
    }
}

static void addbattle(battle * b, const char *s)
{
    ql_push(&b->events, _strdup(s));
}

static void battlerecord(char *s)
{
    ql_iter fli;

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (f->thisbattle) {
            addbattle(f->thisbattle, s);
        }
    }
    if (s[0])
        puts(s);
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

unit * battle_create_unit(faction * f, int no, int number,
                          const char * name, const char * display,
                          const int items[], bool behind)
{
    unit * u = create_unit(f, no);
    u->number = number;
    u->behind = behind;
    unit_setname(u, name);
    unit_setdisplay(u, display);
    if (items) {
        memcpy(u->items, items, sizeof(u->items));
    }
    return u;
}

void battle_add_unit(battle * b, const unit * u)
{
    unit * u2;
    assert(u->side>=0);
    u2 = battle_create_unit(u->faction, u->no, u->number,
                            unit_getname(u), unit_getdisplay(u), 
                            u->items, u->behind);
    ql_push(b->units+u->side, u2);
}

void battle_report_unit(const unit * u)
{
    ql_iter fli;

    for (fli = qli_init(&factions); qli_more(fli);) {
        faction *f = (faction *)qli_next(&fli);
        if (f->thisbattle) {
            battle_add_unit(f->thisbattle, u);
        }
    }
}

int contest(int a, int d)
{
    int i;
    static int table[] = { 10, 25, 40 };

    i = a - d + 1;
    if (i < 0)
        return genrand_int32() % 100 < 1;
    if (i > 2)
        return genrand_int32() % 100 < 49;
    return genrand_int31() % 100 < table[i];
}

int hits(void)
{
    int k;

    if (defender->weapon == I_CROSSBOW || defender->weapon == I_LONGBOW) {
        defender->skill = -2;
    }
    defender->skill += defender->inside;
    attacker->skill -= (attacker->demoralized + attacker->dazzled);
    defender->skill -= (defender->demoralized + defender->dazzled);

    switch (attacker->weapon) {
    case I_CROSSBOW:
        k = contest(attacker->skill, 0);
        break;

    case I_LONGBOW:
        k = contest(attacker->skill, 2);
        break;

    default:
        k = contest(attacker->skill, defender->skill);
        break;

    }

    if (defender->invulnerable && genrand_int32() % 10000)
        k = 0;

    if (genrand_int31() % 3 < defender->armor)
        k = 0;

    return k;
}

int validtarget(int i)
{
    return !ta[i]->status &&
        ta[i]->side == defender_side &&
        (!ta[i]->behind || !infront[defender_side]);
}

int canbedemoralized(int i)
{
    return validtarget(i) && !ta[i]->demoralized;
}

int canbedazzled(int i)
{
    return validtarget(i) && !ta[i]->dazzled;
}

int canberemoralized(int i)
{
    return !ta[i]->status &&
        ta[i]->side == attacker->side && ta[i]->demoralized;
}

int selecttarget(void)
{
    int i;

    for (i=0;i!=ntroops;++i) {
        if (validtarget(i)) return i;
    }
    return -1;
}

void terminate(int i)
{
    if (!ta[i]->attacked) {
        ta[i]->attacked = 1;
        toattack[defender->side]--;
    }

    ta[i]->status = 1;
    left[defender->side]--;
    if (infront[defender->side])
        infront[defender->side]--;
    if (ta[i]->runesword)
        runeswords[defender->side]--;
}

void dozap(int n)
{
    static char buf2[40];

    n = lovar(n * (1 + attacker->power));
    n = MIN(n, left[defender->side]);

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
    char buf2[256];

    z = ta[i]->unit->combatspell;
    sprintf(buf, "%s casts %s", unitid(ta[i]->unit), spellnames[z]);

    if (shields[defender->side])
        if (genrand_int32() & 1) {
            scat(", and gets through the shield");
            shields[defender->side] -= 1 + attacker->power;
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
        if (runeswords[defender->side] && (genrand_int32() & 1))
            break;

        n = lovar(100 * (1 + attacker->power));

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
                j = genrand_int32() % ntroops;
            while (!canbedemoralized(j));

            ta[j]->demoralized = 1;
        }

        break;

    case SP_DAZZLING_LIGHT:
        n = lovar(50 * (1 + attacker->power));

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
                j = genrand_int32() % ntroops;
            while (!canbedazzled(j));

            ta[j]->dazzled = 1;
        }

        break;

    case SP_FIREBALL:
        dozap(50);
        break;

    case SP_HAND_OF_DEATH:
        dozap(250);
        break;

    case SP_INSPIRE_COURAGE:
        n = lovar(100 * (1 + attacker->power));

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
                j = genrand_int32() % ntroops;
            while (!canberemoralized(j));

            ta[j]->demoralized = 0;
        }

        break;

    case SP_LIGHTNING_BOLT:
        dozap(10);
        break;

    case SP_SHIELD:
        shields[attacker->side] += 1 + attacker->power;
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
        ai = genrand_int32() % ntroops;
    while (ta[ai]->attacked);

    attacker = ta[ai];
    ta[ai]->attacked = 1;
    toattack[attacker->side]--;
    defender_side = 1 - attacker->side;

    ta[ai]->dazzled = 0;

    if (attacker->unit) {
        if (attacker->behind && infront[attacker->side] && !attacker->missile)
            return;

        if (attacker->shieldstone)
            shields[attacker->side] += 1 + attacker->power;

        if (attacker->unit->combatspell >= 0) {
            docombatspell(ai);
            return;
        }

        if (attacker->reload) {
            ta[ai]->reload--;
            return;
        }

        if (attacker->weapon == I_CROSSBOW)
            ta[ai]->reload = 2;
    }

    /* Select defender */

    di = selecttarget();
    if (di>=0) {
        defender = ta[di];
        assert(defender->side == 1 - attacker->side);

        /* If attack succeeds */

        if (hits())
            terminate(di);
    }
}

troop * get_troop(troop **ta, int ntroops, int side, int mask) {
    int j;
    do {
        j = genrand_int32() % ntroops;
    } while (ta[j]->status & mask || ta[j]->side ==side);
    return ta[j];
}

void count_casualties(region *r, troop **ta, int ntroops, int *peasants) {
    unit *u;
    int i, deadpeasants = 0;
    /* Count the casualties */
    for (u=r->units_;u;u=u->next) {
        u->dead = 0;
    }
    for (i = 0; i != ntroops; i++) {
        if (ta[i]->unit) {
            ta[i]->unit->dead += ta[i]->status;
        } else {
            deadpeasants += ta[i]->status;
        }
    }
    if (peasants) {
        *peasants = deadpeasants;
    }
}

static bool ispresent(const faction * f, region * r)
{
    unit *u;

    for (u=r->units_;u;u=u->next) {
        if (u->faction == f)
            return true;
    }
    return false;
}

void process_combat(void)
{
    char buf2[256];
    faction **fa;
    int nfactions;

    /* Combat */

    puts("Processing ATTACK orders...");

    nfactions = ql_length(factions);
    fa = (faction **)malloc(nfactions * sizeof(faction *));

    if (fa && factions) {
        ql_iter rli;
        for (rli = qli_init(&regions); qli_more(rli);) {
            region * r = (region *)qli_next(&rli);
            ql_iter fli;
            int i, fno;
        
            /* Create randomly sorted list of factions */
            fli = qli_init(&factions);
            fa[0] = (faction *)qli_next(&fli);
            for (i = 1; qli_more(fli); ++i) {
                int j = genrand_int31() % i;
                fa[i] = fa[j];
                fa[j] = (faction *)qli_next(&fli);
            }

            /* Handle each faction's attack orders */

            for (fno = 0; fno != nfactions; fno++) {
                unit *u;
                faction *f;

                f = fa[fno];

                for (u=r->units_;u;u=u->next) {
                    if (u->faction == f) {
                        ql_iter oli;
                        for (oli = qli_init(&u->orders); qli_more(oli);) {
                            char *s = (char *)qli_next(&oli);
                            if (igetkeyword(s) == K_ATTACK) {
                                int leader[2];
                                troop *t;
                                quicklist *troops = 0;
                                int maxtactics[2];
                                int winnercasualties = 0, deadpeasants = 0, lmoney = 0;
                                int litems[MAXITEMS];
                                int n, k, j;
                                unit *u2, *u3, *u4;
                                ql_iter qli;
                            
                                j = getseen(r, u->faction, &u2);

                                if (j!=U_UNIT && j!=U_PEASANTS) {
                                    mistakes(u, s, "Unit not found");
                                    continue;
                                }

                                if (u2 && u2->faction == f) {
                                    mistakes(u, s, "One of your units");
                                    continue;
                                }

                                if (isallied(u, u2)) {
                                    mistakes(u, s, "An allied unit");
                                    continue;
                                }

                                /* Draw up troops for the battle */

                                for (qli=qli_init(&r->buildings);qli_more(qli);) {
                                    building *b = (building *)qli_next(&qli);

                                    b->sizeleft = b->size;
                                }

                                left[0] = left[1] = 0;
                                infront[0] = infront[1] = 0;

                                /* If peasants are defenders */

                                if (!u2) {
                                    for (i = r->peasants; i; i--) {
                                        t = (troop *)calloc(1, sizeof(troop));
                                        ql_push(&troops, t);
                                    }

                                    left[0] = r->peasants;
                                    infront[0] = r->peasants;
                                }

                                /* What units are involved? */

                                for (fli = qli_init(&factions); qli_more(fli); ) {
                                    faction *f2 = (faction *)qli_next(&fli);
                                    f2->attacking = false;
                                }
                                for (u3=r->units_;u3;u3=u3->next) {
                                    unit *u3 = (unit *)qli_next(&qli);
                                    ql_iter oli;
                                    for (oli = qli_init(&u3->orders); qli_more(oli);) {
                                        char *s = (char *)qli_next(&oli);
                                        if (igetkeyword(s) == K_ATTACK) {
                                            int j = getseen(r, u3->faction, &u4);

                                            if ((!u2) || (j==U_UNIT && u4->faction == u2->faction && !isallied(u3, u4))) {
                                                u3->faction->attacking = true;
                                                s[0] = 0;
                                                break;
                                            }
                                        }
                                    }
                                }
                                for (u3=r->units_;u3;u3=u3->next) {
                                    u3->side = -1;

                                    if (!u3->number)
                                        continue;

                                    if (u3->faction->attacking) {
                                        u3->side = 0;
                                        maketroops(&troops, u3, r->terrain);
                                    } else if (isallied(u3, u2)) {
                                        u3->side = 1;
                                        maketroops(&troops, u3, r->terrain);
                                    }
                                }

                                /* If only one side shows up, cancel */

                                if (!left[0] || !left[1]) {
                                    ql_foreach(troops, free);
                                    ql_free(troops);
                                    continue;
                                }

                                /* Set up array of troops */

                                ntroops = ql_length(troops);
                                ta = (troop **)malloc(ntroops * sizeof(troop*));
                                fisher_yates_ql(troops, (void **)ta, ntroops);
                                ql_free(troops);

                                initial[0] = left[0];
                                initial[1] = left[1];
                                shields[0] = 0;
                                shields[1] = 0;
                                runeswords[0] = 0;
                                runeswords[1] = 0;

                                lmoney = 0;
                                memset(litems, 0, sizeof litems);

                                /* Initial attack message */

                                for (fli = qli_init(&factions); qli_more(fli);) {
                                    faction *f2 = (faction *)qli_next(&fli);
                                    if (ispresent(f2, r)) {
                                        f2->thisbattle = create_battle(r);
                                        ql_push(&f2->battles, f2->thisbattle);
                                    } else {
                                        f2->thisbattle = 0;
                                    }
                                }

                                if (u2) {
                                    strcpy(buf2, unitid(u2));
                                } else {
                                    strcpy(buf2, "the peasants");
                                }
                                for (fli = qli_init(&factions); qli_more(fli); ) {
                                    faction *f2 = (faction *)qli_next(&fli);
                                    if (f2->thisbattle) {
                                        battle * b = f2->thisbattle;
                                        sprintf(buf, "%s attacks %s in %s!", unitid(u),
                                                buf2, regionid(r, f2));
                                        ql_push(&b->events, _strdup(buf));
                                    }
                                }

                                /* List sides */
                                battle_report_unit(u);

                                for (u3=r->units_;u3;u3=u3->next) {
                                    if (u3->side == 0 && u3 != u) {
                                        battle_report_unit(u3);
                                    }
                                }

                                if (u2) {
                                    battle_report_unit(u2);
                                } else {
                                    u3 = battle_create_unit(u->faction, 0, r->peasants,
                                                            "Peasants", 0, 0, false);
                                    u3->side = 1;
                                    for (fli = qli_init(&factions); qli_more(fli); ) {
                                        faction *f2 = (faction *)qli_next(&fli);
                                        if (f2->thisbattle) {
                                            battle_add_unit(f2->thisbattle, u3);
                                        }
                                    }
                                }

                                for (u3=r->units_;u3;u3=u3->next) {
                                    if (u3->side == 1 && u3 != u2) {
                                        battle_report_unit(u3);
                                    }
                                }

                                /* Does one side have an advantage in tactics? */

                                maxtactics[0] = 0;
                                maxtactics[1] = 0;

                                for (i = 0; i != ntroops; i++) {
                                    if (ta[i]->unit) {
                                        j = effskill(ta[i]->unit, SK_TACTICS);

                                        if (maxtactics[ta[i]->side] < j) {
                                            leader[ta[i]->side] = i;
                                            maxtactics[ta[i]->side] = j;
                                        }
                                    }
                                }
                                attacker_side = -1;
                                if (maxtactics[0] > maxtactics[1]) {
                                    attacker_side = 0;
                                }
                                else if (maxtactics[1] > maxtactics[0]) {
                                    attacker_side = 1;
                                }

                                /* Better leader gets free round of attacks */

                                if (attacker_side >= 0) {
                                    /* Note the fact in the battle report */

                                    if (attacker_side) {
                                        sprintf(buf,
                                                "%s gets a free round of attacks!",
                                                unitid(u));
                                    } else if (u2) {
                                        sprintf(buf,
                                                "%s gets a free round of attacks!",
                                                unitid(u2));
                                    } else {
                                        sprintf(buf,
                                                "The peasants get a free round of attacks!");
                                    }
                                    battlerecord(buf);

                                    /* Number of troops to attack */

                                    toattack[attacker_side] = 0;

                                    for (i = 0; i != ntroops; i++) {
                                        ta[i]->attacked = 1;

                                        if (ta[i]->side == attacker->side) {
                                            ta[i]->attacked = 0;
                                            toattack[attacker_side]++;
                                        }
                                    }

                                    /* Do round of attacks */

                                    do {
                                        doshot();
                                    } while (toattack[attacker_side]
                                             && left[1-attacker_side]);
                                }

                                /* Handle main body of battle */

                                toattack[0] = 0;
                                toattack[1] = 0;

                                while (left[0] && left[1]) {
                                    /* End of a round */

                                    if (toattack[0] == 0 && toattack[1] == 0)
                                        for (i = 0; i != ntroops; i++) {
                                            ta[i]->attacked = 1;

                                            if (!ta[i]->status) {
                                                ta[i]->attacked = 0;
                                                toattack[ta[i]->side]++;
                                            }
                                        }

                                    doshot();
                                }

                                /* Report on winner */

                                if (attacker->side==0) {
                                    sprintf(buf, "%s wins the battle!",
                                            unitid(u));
                                } else if (u2) {
                                    sprintf(buf, "%s wins the battle!",
                                            unitid(u2));
                                } else {
                                    sprintf(buf,
                                            "The peasants win the battle!");
                                }
                                battlerecord(buf);

                                /* Has winner suffered any casualties? */

                                winnercasualties = 0;

                                for (i = 0; i != ntroops; i++) {
                                    if (ta[i]->side == attacker->side
                                        && ta[i]->status) {
                                        winnercasualties = 1;
                                        break;
                                    }
                                }
                                /* Can wounded be healed? */

                                n = 0;

                                for (i = 0; i != ntroops &&
                                     n != initial[attacker->side] -
                                     left[attacker->side]; i++) {
                                    if (!ta[i]->status && ta[i]->canheal) {
                                        int k = lovar(50 * (1 + ta[i]->power));
                                        k = MIN(k, initial[attacker->side] -
                                                left[attacker->side] - n);
                                        sprintf(buf, "%s heals %d wounded.",
                                                unitid(ta[i]->unit), k);
                                        battlerecord(buf);

                                        n += k;
                                    }
                                }
                                while (--n >= 0) {
                                    do {
                                        i = genrand_int32() % ntroops;
                                    } while (!ta[i]->status
                                             || ta[i]->side != attacker->side);

                                    ta[i]->status = 0;
                                }

                                count_casualties(r, ta, ntroops, &deadpeasants);
                                /* Report the casualties */

                                reportcasualtiesdh = 0;

                                if (attacker->side) {
                                    reportcasualties(u);

                                    for (u3=r->units_;u3;u3=u3->next) {
                                        if (u3->side == 1 && u3 != u) {
                                            reportcasualties(u3);
                                        }
                                    }
                                } else {
                                    if (u2) {
                                        reportcasualties(u2);
                                    } else if (deadpeasants) {
                                        battlerecord("");
                                        reportcasualtiesdh = 1;
                                        sprintf(buf, "The peasants lose %d.",
                                                deadpeasants);
                                        battlerecord(buf);
                                    }

                                    for (u3=r->units_;u3;u3=u3->next) {
                                        if (u3->side == 0 && u3 != u2) {
                                            reportcasualties(u3);
                                        }
                                    }
                                }

                                /* Dead peasants */

                                k = r->peasants - deadpeasants;

                                j = distribute(r->peasants, k, r->money);
                                lmoney += r->money - j;
                                r->money = j;

                                r->peasants = k;

                                /* Adjust units */

                                for (u3=r->units_;u3;u3=u3->next) {
                                    k = u3->number - u3->dead;

                                    /* Redistribute items and skills */

                                    if (u3->side == defender->side) {
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

                                    for (i = 0; i != MAXSKILLS; i++) {
                                        u3->skills[i] =
                                          distribute(u3->number, k,
                                                     u3->skills[i]);
                                    }
                                    /* Adjust unit numbers */

                                    u3->number = k;

                                    /* Need this flag cleared for reporting of loot */

                                    u3->n = 0;
                                }

                                /* Distribute loot */

                                for (n = lmoney; n; n--) {
                                    troop *t = get_troop(ta, ntroops, defender->side, 0xFF);
                                    if (t->unit) {
                                        t->unit->money++;
                                        t->unit->n++;
                                    } else {
                                        r->money++;
                                    }
                                }

                                for (i = 0; i != MAXITEMS; i++) {
                                    for (n = litems[i]; n; n--) {
                                        if (i <= I_STONE || genrand_int32() & 1) {
                                            do {
                                                j = genrand_int32() % ntroops;
                                            } while (ta[j]->status || ta[j]->side != attacker->side);

                                            if (ta[j]->unit) {
                                                if (!ta[j]->unit->litems) {
                                                    ta[j]->unit->litems =
                                                      (int *)calloc(MAXITEMS, sizeof(int));
                                                }
                                                ta[j]->unit->items[i]++;
                                                ta[j]->unit->litems[i]++;
                                            }
                                        }
                                    }
                                }
                                /* Report loot */

                                for (fli = qli_init(&factions); qli_more(fli); ) {
                                    faction *f2 = (faction *)qli_next(&fli);
                                    f2->dh = 0;
                                }
                                for (u3=r->units_;u3;u3=u3->next) {
                                    if (u3->n || u3->litems) {
                                        int dh = 0;

                                        sprintf(buf, "%s finds ", unitid(u3));

                                        if (u3->n) {
                                            scat("$");
                                            icat(u3->n);
                                            dh = 1;
                                        }

                                        if (u3->litems) {
                                            for (i = 0; i != MAXITEMS; i++) {
                                                if (u3->litems[i]) {
                                                    if (dh) {
                                                        scat(", ");
                                                    }
                                                    dh = 1;

                                                    icat(u3->litems[i]);
                                                    scat(" ");

                                                    if (u3->litems[i] == 1) {
                                                        scat(itemnames[i][0]);
                                                    } else {
                                                        scat(itemnames[i][1]);
                                                    }
                                                }
                                            }
                                            free(u3->litems);
                                            u3->litems = 0;
                                        }

                                        if (!u3->faction->dh) {
                                            addbattle(u3->faction->thisbattle, "");
                                            u3->faction->dh = 1;
                                        }

                                        scat(".");
                                        addbattle(u3->faction->thisbattle, buf);
                                    }
                                }

                                /* Does winner get combat experience? */
                                if (winnercasualties) {
                                    if (maxtactics[attacker->side] &&
                                        !ta[leader[attacker->side]]->status) {
                                        ta[leader[attacker->side]]->unit->skills[SK_TACTICS] += COMBATEXP;
                                    }
                                    for (i = 0; i != ntroops; i++) {
                                        if (ta[i]->unit &&
                                            !ta[i]->status &&
                                            ta[i]->side == attacker->side) {
                                            switch (ta[i]->weapon) {
                                            case I_SWORD:
                                                ta[i]->unit->skills[SK_SWORD] +=
                                                    COMBATEXP;
                                                break;

                                            case I_CROSSBOW:
                                                ta[i]->unit->
                                                    skills[SK_CROSSBOW] +=
                                                    COMBATEXP;
                                                break;

                                            case I_LONGBOW:
                                                ta[i]->unit->
                                                    skills[SK_LONGBOW] +=
                                                    COMBATEXP;
                                                break;
                                            }
                                        }
                                    }
                                }

                                free(ta);
                            }
                        }
                    }
                }
            }
        }
        free(fa);
    }
}
