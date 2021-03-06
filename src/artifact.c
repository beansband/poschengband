/* Purpose: Artifact code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

static void random_resistance(object_type * o_ptr);
static void random_slay(object_type *o_ptr);


/* Chance of using syllables to form the name instead of the "template" files */
#define SINDARIN_NAME   10
#define TABLE_NAME      20
#define A_CURSED        13
#define WEIRD_LUCK      12
#define BIAS_LUCK       20
#define IM_LUCK         7
#define IM_DEPTH        100
bool immunity_hack = FALSE;
int slaying_hack = 0;
static bool has_pval;

/*
 * Bias luck needs to be higher than weird luck,
 * since it is usually tested several times...
 */
#define ACTIVATION_CHANCE 5


/*
 * Use for biased artifact creation
 */
static int artifact_bias;

/*
 * Cursed arts now pick up bad flags at the end. To avoid idiocy like
 * arts that give resistance and vulnerability to fire simultaneously, 
 * we implement the following check:
 */
static bool _add_bad_flag(object_type *o_ptr, int bad_flag, int good_flag)
{
    u32b flags[TR_FLAG_SIZE];
    object_flags(o_ptr, flags); /* Careful: Might be a cursed ego! */
    if (!have_flag(flags, good_flag))
    {
        add_flag(o_ptr->art_flags, bad_flag);
        return TRUE;
    }
    return FALSE;
}

/*
 * Choose one random sustain
 */
void one_sustain(object_type *o_ptr)
{
    switch (randint0(6))
    {
        case 0: add_flag(o_ptr->art_flags, TR_SUST_STR); break;
        case 1: add_flag(o_ptr->art_flags, TR_SUST_INT); break;
        case 2: add_flag(o_ptr->art_flags, TR_SUST_WIS); break;
        case 3: add_flag(o_ptr->art_flags, TR_SUST_DEX); break;
        case 4: add_flag(o_ptr->art_flags, TR_SUST_CON); break;
        case 5: add_flag(o_ptr->art_flags, TR_SUST_CHR); break;
    }
}


/*
 * Choose one random high resistance
 */
void one_high_resistance(object_type *o_ptr)
{
    switch (randint0(12))
    {
        case  0: add_flag(o_ptr->art_flags, TR_RES_POIS);   break;
        case  1: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
        case  2: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
        case  3: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
        case  4: add_flag(o_ptr->art_flags, TR_RES_BLIND);  break;
        case  5: add_flag(o_ptr->art_flags, TR_RES_CONF);   break;
        case  6: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
        case  7: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
        case  8: add_flag(o_ptr->art_flags, TR_RES_NEXUS);  break;
        case  9: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
        case 10: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
        case 11: add_flag(o_ptr->art_flags, TR_RES_FEAR);   break;
    }
}
bool one_high_vulnerability(object_type *o_ptr)
{
    int attempts = 100;
    int i;
    for (i = 0; i < attempts; i++)
    {
        int good_flag = 0, bad_flag = 0;
        switch (randint0(12))
        {
            case  0: bad_flag = TR_VULN_POIS; good_flag = TR_RES_POIS; break;
            case  1: bad_flag = TR_VULN_LITE; good_flag = TR_RES_LITE; break;
            case  2: bad_flag = TR_VULN_DARK; good_flag = TR_RES_DARK; break;
            case  3: bad_flag = TR_VULN_SHARDS; good_flag = TR_RES_SHARDS; break;
            case  4: bad_flag = TR_VULN_BLIND; good_flag = TR_RES_BLIND; break;
            case  5: bad_flag = TR_VULN_CONF; good_flag = TR_RES_CONF; break;
            case  6: bad_flag = TR_VULN_SOUND; good_flag = TR_RES_SOUND; break;
            case  7: bad_flag = TR_VULN_NETHER; good_flag = TR_RES_NETHER; break;
            case  8: bad_flag = TR_VULN_NEXUS; good_flag = TR_RES_NEXUS; break;
            case  9: bad_flag = TR_VULN_CHAOS; good_flag = TR_RES_CHAOS; break;
            case 10: bad_flag = TR_VULN_DISEN; good_flag = TR_RES_DISEN; break;
            case 11: bad_flag = TR_VULN_FEAR; good_flag = TR_RES_FEAR; break;
        }
        if (_add_bad_flag(o_ptr, bad_flag, good_flag))
            return TRUE;
    }
    return FALSE;
}
void one_undead_resistance(object_type *o_ptr)
{
    switch (randint1(6))
    {
        case 1: add_flag(o_ptr->art_flags, TR_RES_COLD);   break;
        case 2: add_flag(o_ptr->art_flags, TR_RES_POIS);   break;
        case 3: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
        case 4: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
        case 5: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
        case 6: add_flag(o_ptr->art_flags, TR_RES_FEAR);   break;
    }
}
void one_demon_resistance(object_type *o_ptr)
{
    switch (randint1(6))
    {
        case 1: add_flag(o_ptr->art_flags, TR_RES_FIRE);   break;
        case 2: add_flag(o_ptr->art_flags, TR_RES_CONF);   break;
        case 3: add_flag(o_ptr->art_flags, TR_RES_NEXUS);  break;
        case 4: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
        case 5: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
        case 6: add_flag(o_ptr->art_flags, TR_RES_FEAR);   break;
    }
}

void one_holy_resistance(object_type *o_ptr)
{
    switch (randint1(4))
    {
        case 1: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
        case 2: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
        case 3: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
        case 4: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
    }
}

/*
 * Choose one random high resistance ( except poison and disenchantment )
 */
void one_lordly_high_resistance(object_type *o_ptr)
{
    switch (randint0(10))
    {
        case 0: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
        case 1: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
        case 2: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
        case 3: add_flag(o_ptr->art_flags, TR_RES_BLIND);  break;
        case 4: add_flag(o_ptr->art_flags, TR_RES_CONF);   break;
        case 5: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
        case 6: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
        case 7: add_flag(o_ptr->art_flags, TR_RES_NEXUS);  break;
        case 8: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
        case 9: add_flag(o_ptr->art_flags, TR_RES_FEAR);   break;
    }
}

/*
 * Choose one random element resistance
 */
void one_ele_resistance(object_type *o_ptr)
{
    switch (randint0(4))
    {
        case  0: add_flag(o_ptr->art_flags, TR_RES_ACID); break;
        case  1: add_flag(o_ptr->art_flags, TR_RES_ELEC); break;
        case  2: add_flag(o_ptr->art_flags, TR_RES_COLD); break;
        case  3: add_flag(o_ptr->art_flags, TR_RES_FIRE); break;
    }
}
bool one_ele_vulnerability(object_type *o_ptr)
{
    int attempts = 100;
    int i;
    for (i = 0; i < attempts; i++)
    {
        int good_flag = 0, bad_flag = 0;
        switch (randint0(4))
        {
            case  0: bad_flag = TR_VULN_ACID; good_flag = TR_RES_ACID; break;
            case  1: bad_flag = TR_VULN_ELEC; good_flag = TR_RES_ELEC; break;
            case  2: bad_flag = TR_VULN_COLD; good_flag = TR_RES_COLD; break;
            case  3: bad_flag = TR_VULN_FIRE; good_flag = TR_RES_FIRE; break;
        }
        if (_add_bad_flag(o_ptr, bad_flag, good_flag))
            return TRUE;
    }
    return FALSE;
}

void one_ele_slay(object_type *o_ptr)
{
    switch (randint0(5))
    {
        case  0: add_flag(o_ptr->art_flags, TR_BRAND_ACID); break;
        case  1: add_flag(o_ptr->art_flags, TR_BRAND_COLD); break;
        case  2: add_flag(o_ptr->art_flags, TR_BRAND_FIRE); break;
        case  3: add_flag(o_ptr->art_flags, TR_BRAND_ELEC); break;
        case  4: add_flag(o_ptr->art_flags, TR_BRAND_POIS); break;
    }
}


/*
 * Choose one random element or poison resistance
 */
void one_dragon_ele_resistance(object_type *o_ptr)
{
    if (one_in_(7))
    {
        add_flag(o_ptr->art_flags, TR_RES_POIS);
    }
    else
    {
        one_ele_resistance(o_ptr);
    }
}
bool one_dragon_ele_vulnerability(object_type *o_ptr)
{
    if (one_in_(7) && !have_flag(o_ptr->art_flags, TR_RES_POIS))
    {
        add_flag(o_ptr->art_flags, TR_VULN_POIS);
        return TRUE;
    }
    return one_ele_vulnerability(o_ptr);
}

/*
 * Choose one lower rank esp
 */
void one_low_esp(object_type *o_ptr)
{
    switch (randint1(9))
    {
    case 1: add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);   break;
    case 2: add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);   break;
    case 3: add_flag(o_ptr->art_flags, TR_ESP_DEMON);   break;
    case 4: add_flag(o_ptr->art_flags, TR_ESP_ORC);   break;
    case 5: add_flag(o_ptr->art_flags, TR_ESP_TROLL);   break;
    case 6: add_flag(o_ptr->art_flags, TR_ESP_GIANT);   break;
    case 7: add_flag(o_ptr->art_flags, TR_ESP_DRAGON);   break;
    case 8: add_flag(o_ptr->art_flags, TR_ESP_HUMAN);   break;
    case 9: add_flag(o_ptr->art_flags, TR_ESP_GOOD);   break;
    }
}



/*
 * Choose one random resistance
 */
void one_resistance(object_type *o_ptr)
{
    if (one_in_(3))
    {
        one_ele_resistance(o_ptr);
    }
    else
    {
        one_high_resistance(o_ptr);
    }
}
bool one_vulnerability(object_type *o_ptr)
{
    if (one_in_(3))
        return one_ele_vulnerability(o_ptr);
    
    return one_high_vulnerability(o_ptr);
}

bool one_stat_biff(object_type *o_ptr)
{
    int attempts = 100;
    int i;
    for (i = 0; i < attempts; i++)
    {
        int n = randint0(6);
        if (_add_bad_flag(o_ptr, TR_DEC_STR + n, TR_STR + n))
            return TRUE;
    }
    return FALSE;
}

bool one_biff(object_type *o_ptr)
{
    int attempts = 100;
    int i;
    for (i = 0; i < attempts; i++)
    {
        int n = randint0(100);
        if (n < 10)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_STEALTH, TR_STEALTH))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 13)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_SPEED, TR_SPEED))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 17)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_LIFE, TR_LIFE))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 22)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_MAGIC_MASTERY, TR_MAGIC_MASTERY))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 30)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_SPELL_CAP, TR_SPELL_CAP))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 35)
        {
            if (_add_bad_flag(o_ptr, TR_DEC_SPELL_POWER, TR_SPELL_POWER))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else if (n < 67) 
        {
            if (one_stat_biff(o_ptr))
            {
                has_pval = TRUE;
                return TRUE;
            }
        }
        else
        {
            if (one_vulnerability(o_ptr))
                return TRUE;
        }
    }
    return FALSE;
}

/*
 * Choose one random ability
 */
void one_ability(object_type *o_ptr)
{
    switch (randint0(10))
    {
    case 0: add_flag(o_ptr->art_flags, TR_LEVITATION);     break;
    case 1: add_flag(o_ptr->art_flags, TR_LITE);        break;
    case 2: add_flag(o_ptr->art_flags, TR_SEE_INVIS);   break;
    case 3: add_flag(o_ptr->art_flags, TR_WARNING);     break;
    case 4: add_flag(o_ptr->art_flags, TR_SLOW_DIGEST); break;
    case 5: add_flag(o_ptr->art_flags, TR_REGEN);       break;
    case 6: add_flag(o_ptr->art_flags, TR_FREE_ACT);    break;
    case 7: add_flag(o_ptr->art_flags, TR_HOLD_LIFE);   break;
    case 8:
    case 9:
        one_low_esp(o_ptr);
        break;
    }
}

static void random_plus(object_type * o_ptr)
{
    int this_type = (object_is_weapon_ammo(o_ptr) ? 23 : 19);
    
    has_pval = TRUE;
    switch (artifact_bias)
    {
    case BIAS_WARRIOR:
        if (!(have_flag(o_ptr->art_flags, TR_STR)))
        {
            add_flag(o_ptr->art_flags, TR_STR);
            if (one_in_(2)) return;
        }

        if (!(have_flag(o_ptr->art_flags, TR_CON)))
        {
            add_flag(o_ptr->art_flags, TR_CON);
            if (one_in_(2)) return;
        }

        if (!(have_flag(o_ptr->art_flags, TR_DEX)))
        {
            add_flag(o_ptr->art_flags, TR_DEX);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_MAGE:
        if (!(have_flag(o_ptr->art_flags, TR_INT)))
        {
            add_flag(o_ptr->art_flags, TR_INT);
            if (one_in_(2)) return;
        }
        if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY)))
        {
            add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_PRIESTLY:
        if (!(have_flag(o_ptr->art_flags, TR_WIS)))
        {
            add_flag(o_ptr->art_flags, TR_WIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_RANGER:
        if (!(have_flag(o_ptr->art_flags, TR_DEX)))
        {
            add_flag(o_ptr->art_flags, TR_DEX);
            if (one_in_(2)) return;
        }

        if (!(have_flag(o_ptr->art_flags, TR_CON)))
        {
            add_flag(o_ptr->art_flags, TR_CON);
            if (one_in_(2)) return;
        }

        if (!(have_flag(o_ptr->art_flags, TR_STR)))
        {
            add_flag(o_ptr->art_flags, TR_STR);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_ROGUE:
        if (!(have_flag(o_ptr->art_flags, TR_STEALTH)))
        {
            add_flag(o_ptr->art_flags, TR_STEALTH);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_SEARCH)))
        {
            add_flag(o_ptr->art_flags, TR_SEARCH);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_STR:
        if (!(have_flag(o_ptr->art_flags, TR_STR)))
        {
            add_flag(o_ptr->art_flags, TR_STR);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_WIS:
        if (!(have_flag(o_ptr->art_flags, TR_WIS)))
        {
            add_flag(o_ptr->art_flags, TR_WIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_INT:
        if (!(have_flag(o_ptr->art_flags, TR_INT)))
        {
            add_flag(o_ptr->art_flags, TR_INT);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_DEX:
        if (!(have_flag(o_ptr->art_flags, TR_DEX)))
        {
            add_flag(o_ptr->art_flags, TR_DEX);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CON:
        if (!(have_flag(o_ptr->art_flags, TR_CON)))
        {
            add_flag(o_ptr->art_flags, TR_CON);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CHR:
        if (!(have_flag(o_ptr->art_flags, TR_CHR)))
        {
            add_flag(o_ptr->art_flags, TR_CHR);
            if (one_in_(2)) return;
        }
        break;
    }

    if ((artifact_bias == BIAS_MAGE || artifact_bias == BIAS_PRIESTLY) && (o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ROBE))
    {
        if (!(have_flag(o_ptr->art_flags, TR_DEC_MANA)) && one_in_(3))
        {
            add_flag(o_ptr->art_flags, TR_DEC_MANA);
            if (one_in_(2)) return;
        }
    }

    switch (randint1(this_type))
    {
    case 1: case 2:
        add_flag(o_ptr->art_flags, TR_STR);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_STR;
        else if (!artifact_bias && one_in_(7))
            artifact_bias = BIAS_WARRIOR;
        break;
    case 3: case 4:
        add_flag(o_ptr->art_flags, TR_INT);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_INT;
        else if (!artifact_bias && one_in_(7))
            artifact_bias = BIAS_MAGE;
        break;
    case 5: case 6:
        add_flag(o_ptr->art_flags, TR_WIS);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_WIS;
        else if (!artifact_bias && one_in_(7))
            artifact_bias = BIAS_PRIESTLY;
        break;
    case 7: case 8:
        add_flag(o_ptr->art_flags, TR_DEX);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_DEX;
        else if (!artifact_bias && one_in_(7))
            artifact_bias = BIAS_ROGUE;
        break;
    case 9: case 10:
        add_flag(o_ptr->art_flags, TR_CON);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_CON;
        else if (!artifact_bias && one_in_(9))
            artifact_bias = BIAS_RANGER;
        break;
    case 11: case 12:
        add_flag(o_ptr->art_flags, TR_CHR);
        if (!artifact_bias && !one_in_(13))
            artifact_bias = BIAS_CHR;
        break;
    case 13: case 14:
        add_flag(o_ptr->art_flags, TR_STEALTH);
        if (!artifact_bias && one_in_(3))
            artifact_bias = BIAS_ROGUE;
        break;
    case 15: case 16:
        add_flag(o_ptr->art_flags, TR_SEARCH);
        if (!artifact_bias && one_in_(9))
            artifact_bias = BIAS_RANGER;
        break;
    case 17: case 18:
        add_flag(o_ptr->art_flags, TR_INFRA);
        break;
    case 19:
        add_flag(o_ptr->art_flags, TR_SPEED);
        if (!artifact_bias && one_in_(11))
            artifact_bias = BIAS_ROGUE;
        break;
    case 20: case 21:
        if (one_in_(3))
            add_flag(o_ptr->art_flags, TR_SPEED);
        else
            add_flag(o_ptr->art_flags, TR_TUNNEL);
        break;
    case 22: case 23:
        if (o_ptr->tval == TV_BOW) random_plus(o_ptr);
        else
        {
            if (slaying_hack == 0)
            {
                if (one_in_(3))
                {
                    add_flag(o_ptr->art_flags, TR_BLOWS);
                    if (!artifact_bias && one_in_(11))
                        artifact_bias = BIAS_WARRIOR;
                }
                else
                    random_slay(o_ptr);
            }
            else
                random_plus(o_ptr);
        }
        break;
    }
}

static bool double_check_immunity(object_type * o_ptr)
{
    if (randint1(IM_DEPTH) < (object_level - 40))
        return TRUE;
    else if (immunity_hack)
    {
        return FALSE;
    }
    else
    {
        int i;
        int ct = randint1(3);

        immunity_hack = TRUE;

        if (object_is_melee_weapon(o_ptr))
        {
            for (i = 0; i < ct; i++)
                random_slay(o_ptr);

            switch (randint1(10))
            {
            case 1: case 2:
                if (!have_flag(o_ptr->art_flags, TR_SPEED))
                {
                    add_flag(o_ptr->art_flags, TR_SPEED);
                    has_pval = TRUE;
                    break;
                }
                /* vvvv FALL THRU vvvv */
            case 3: case 4: case 5: case 6:
                add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                o_ptr->to_h += 5 + randint1(10);
                o_ptr->to_d += 5 + randint1(10);
                break;
            case 7: case 8:
                if (!have_flag(o_ptr->art_flags, TR_BLOWS))
                {
                    add_flag(o_ptr->art_flags, TR_BLOWS);
                    has_pval = TRUE;
                    break;
                }
                /* vvvv FALL THRU vvvv */
            case 9: case 10:
                add_flag(o_ptr->art_flags, TR_STR);
                add_flag(o_ptr->art_flags, TR_DEX);
                add_flag(o_ptr->art_flags, TR_CON);
                add_flag(o_ptr->art_flags, TR_FREE_ACT);
                add_flag(o_ptr->art_flags, TR_SEE_INVIS);
                has_pval = TRUE;
                break;
            }
        }
        else if (object_is_armour(o_ptr))
        {
            for (i = 0; i < ct; i++)
                random_resistance(o_ptr);

            switch (randint1(10))
            {
            case 1: case 2:
                if (!have_flag(o_ptr->art_flags, TR_SPEED))
                {
                    add_flag(o_ptr->art_flags, TR_SPEED);
                    has_pval = TRUE;
                    break;
                }
                /* vvvv FALL THRU vvvv */
            case 3: case 4: case 5: case 6:
                add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                o_ptr->to_h += 5 + randint1(10);    /* These will get trimmed later ... */
                o_ptr->to_d += 5 + randint1(10);
                break;
            case 7: case 8:
                o_ptr->to_a += 5 + randint1(10);
                add_flag(o_ptr->art_flags, TR_FREE_ACT);
                add_flag(o_ptr->art_flags, TR_SEE_INVIS);
                add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
                add_flag(o_ptr->art_flags, TR_LEVITATION);
                break;
            case 9: case 10:
                add_flag(o_ptr->art_flags, TR_STR);
                add_flag(o_ptr->art_flags, TR_DEX);
                add_flag(o_ptr->art_flags, TR_CON);
                has_pval = TRUE;
                break;
            }
        }
        else if (o_ptr->tval == TV_RING || o_ptr->tval == TV_AMULET)
        {
            /*
            ct /= 2;
            for (i = 0; i < ct; i++)
                random_resistance(o_ptr);
            */

            switch (randint1(3))
            {
            case 1:
                if (!have_flag(o_ptr->art_flags, TR_SPEED))
                {
                    add_flag(o_ptr->art_flags, TR_SPEED);
                    has_pval = TRUE;
                    break;
                }
            case 2:
                add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                o_ptr->to_h += 5 + randint1(10);    /* These will get trimmed later ... */
                o_ptr->to_d += 5 + randint1(10);
                break;

            case 3:
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_STR);
                    has_pval = TRUE;
                }
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_INT);
                    has_pval = TRUE;
                }
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_WIS);
                    has_pval = TRUE;
                }
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_DEX);
                    has_pval = TRUE;
                }
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_CON);
                    has_pval = TRUE;
                }
                if (one_in_(2)) 
                {
                    add_flag(o_ptr->art_flags, TR_CHR);
                    has_pval = TRUE;
                }
                break;
            }
        }
        return FALSE;
    }
}

static void random_resistance(object_type * o_ptr)
{
    switch (artifact_bias)
    {
    case BIAS_ACID:
        if (!(have_flag(o_ptr->art_flags, TR_RES_ACID)))
        {
            add_flag(o_ptr->art_flags, TR_RES_ACID);
            if (one_in_(2)) return;
        }
        if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ACID)) && double_check_immunity(o_ptr))
        {
            add_flag(o_ptr->art_flags, TR_IM_ACID);
            if (!one_in_(IM_LUCK))
            {
                remove_flag(o_ptr->art_flags, TR_IM_ELEC);
                remove_flag(o_ptr->art_flags, TR_IM_COLD);
                remove_flag(o_ptr->art_flags, TR_IM_FIRE);
            }
            if (one_in_(2)) return;
        }
        break;

    case BIAS_ELEC:
        if (!(have_flag(o_ptr->art_flags, TR_RES_ELEC)))
        {
            add_flag(o_ptr->art_flags, TR_RES_ELEC);
            if (one_in_(2)) return;
        }
        if ((o_ptr->tval >= TV_CLOAK) && (o_ptr->tval <= TV_HARD_ARMOR) &&
            !(have_flag(o_ptr->art_flags, TR_SH_ELEC)))
        {
            add_flag(o_ptr->art_flags, TR_SH_ELEC);
            if (one_in_(2)) return;
        }
        if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ELEC)) && double_check_immunity(o_ptr))
        {
            add_flag(o_ptr->art_flags, TR_IM_ELEC);
            if (!one_in_(IM_LUCK))
            {
                remove_flag(o_ptr->art_flags, TR_IM_ACID);
                remove_flag(o_ptr->art_flags, TR_IM_COLD);
                remove_flag(o_ptr->art_flags, TR_IM_FIRE);
            }

            if (one_in_(2)) return;
        }
        break;

    case BIAS_FIRE:
        if (!(have_flag(o_ptr->art_flags, TR_RES_FIRE)))
        {
            add_flag(o_ptr->art_flags, TR_RES_FIRE);
            if (one_in_(2)) return;
        }
        if ((o_ptr->tval >= TV_CLOAK) &&
            (o_ptr->tval <= TV_HARD_ARMOR) &&
            !(have_flag(o_ptr->art_flags, TR_SH_FIRE)))
        {
            add_flag(o_ptr->art_flags, TR_SH_FIRE);
            if (one_in_(2)) return;
        }
        if (one_in_(BIAS_LUCK)
            && !(have_flag(o_ptr->art_flags, TR_IM_FIRE))
            && double_check_immunity(o_ptr))
        {
            add_flag(o_ptr->art_flags, TR_IM_FIRE);
            if (!one_in_(IM_LUCK))
            {
                remove_flag(o_ptr->art_flags, TR_IM_ELEC);
                remove_flag(o_ptr->art_flags, TR_IM_COLD);
                remove_flag(o_ptr->art_flags, TR_IM_ACID);
            }
            if (one_in_(2)) return;
        }
        break;

    case BIAS_COLD:
        if (!(have_flag(o_ptr->art_flags, TR_RES_COLD)))
        {
            add_flag(o_ptr->art_flags, TR_RES_COLD);
            if (one_in_(2)) return;
        }
        if ((o_ptr->tval >= TV_CLOAK) &&
            (o_ptr->tval <= TV_HARD_ARMOR) &&
            !(have_flag(o_ptr->art_flags, TR_SH_COLD)))
        {
            add_flag(o_ptr->art_flags, TR_SH_COLD);
            if (one_in_(2)) return;
        }
        if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_COLD)) && double_check_immunity(o_ptr))
        {
            add_flag(o_ptr->art_flags, TR_IM_COLD);
            if (!one_in_(IM_LUCK))
            {
                remove_flag(o_ptr->art_flags, TR_IM_ELEC);
                remove_flag(o_ptr->art_flags, TR_IM_ACID);
                remove_flag(o_ptr->art_flags, TR_IM_FIRE);
            }
            if (one_in_(2)) return;
        }
        break;

    case BIAS_POIS:
        if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
        {
            add_flag(o_ptr->art_flags, TR_RES_POIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_WARRIOR:
        if (!one_in_(3) && (!(have_flag(o_ptr->art_flags, TR_RES_FEAR))))
        {
            add_flag(o_ptr->art_flags, TR_RES_FEAR);
            if (one_in_(2)) return;
        }
        if (one_in_(9) && (!(have_flag(o_ptr->art_flags, TR_NO_MAGIC))))
        {
            add_flag(o_ptr->art_flags, TR_NO_MAGIC);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_NECROMANTIC:
        if (!(have_flag(o_ptr->art_flags, TR_RES_NETHER)))
        {
            add_flag(o_ptr->art_flags, TR_RES_NETHER);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
        {
            add_flag(o_ptr->art_flags, TR_RES_POIS);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_RES_DARK)))
        {
            add_flag(o_ptr->art_flags, TR_RES_DARK);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CHAOS:
        if (!(have_flag(o_ptr->art_flags, TR_RES_CHAOS)))
        {
            add_flag(o_ptr->art_flags, TR_RES_CHAOS);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_RES_CONF)))
        {
            add_flag(o_ptr->art_flags, TR_RES_CONF);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_RES_DISEN)))
        {
            add_flag(o_ptr->art_flags, TR_RES_DISEN);
            if (one_in_(2)) return;
        }
        break;
    }

    switch (randint1(42))
    {
        case 1:
            if (!one_in_(WEIRD_LUCK))
                random_resistance(o_ptr);
            else if (double_check_immunity(o_ptr))
            {
                add_flag(o_ptr->art_flags, TR_IM_ACID);
                if (!artifact_bias)
                    artifact_bias = BIAS_ACID;
            }
            break;
        case 2:
            if (!one_in_(WEIRD_LUCK))
                random_resistance(o_ptr);
            else if (double_check_immunity(o_ptr))
            {
                add_flag(o_ptr->art_flags, TR_IM_ELEC);
                if (!artifact_bias)
                    artifact_bias = BIAS_ELEC;
            }
            break;
        case 3:
            if (!one_in_(WEIRD_LUCK))
                random_resistance(o_ptr);
            else if (double_check_immunity(o_ptr))
            {
                add_flag(o_ptr->art_flags, TR_IM_COLD);
                if (!artifact_bias)
                    artifact_bias = BIAS_COLD;
            }
            break;
        case 4:
            if (!one_in_(WEIRD_LUCK))
                random_resistance(o_ptr);
            else if (double_check_immunity(o_ptr))
            {
                add_flag(o_ptr->art_flags, TR_IM_FIRE);
                if (!artifact_bias)
                    artifact_bias = BIAS_FIRE;
            }
            break;
        case 5:
        case 6:
        case 13:
            add_flag(o_ptr->art_flags, TR_RES_ACID);
            if (!artifact_bias)
                artifact_bias = BIAS_ACID;
            break;
        case 7:
        case 8:
        case 14:
            add_flag(o_ptr->art_flags, TR_RES_ELEC);
            if (!artifact_bias)
                artifact_bias = BIAS_ELEC;
            break;
        case 9:
        case 10:
        case 15:
            add_flag(o_ptr->art_flags, TR_RES_FIRE);
            if (!artifact_bias)
                artifact_bias = BIAS_FIRE;
            break;
        case 11:
        case 12:
        case 16:
            add_flag(o_ptr->art_flags, TR_RES_COLD);
            if (!artifact_bias)
                artifact_bias = BIAS_COLD;
            break;
        case 17:
        case 18:
            add_flag(o_ptr->art_flags, TR_RES_POIS);
            if (!artifact_bias && !one_in_(4))
                artifact_bias = BIAS_POIS;
            else if (!artifact_bias && one_in_(2))
                artifact_bias = BIAS_NECROMANTIC;
            else if (!artifact_bias && one_in_(2))
                artifact_bias = BIAS_ROGUE;
            break;
        case 19:
        case 20:
            add_flag(o_ptr->art_flags, TR_RES_FEAR);
            if (!artifact_bias && one_in_(3))
                artifact_bias = BIAS_WARRIOR;
            break;
        case 21:
            add_flag(o_ptr->art_flags, TR_RES_LITE);
            break;
        case 22:
            add_flag(o_ptr->art_flags, TR_RES_DARK);
            break;
        case 23:
        case 24:
            add_flag(o_ptr->art_flags, TR_RES_BLIND);
            break;
        case 25:
        case 26:
            add_flag(o_ptr->art_flags, TR_RES_CONF);
            if (!artifact_bias && one_in_(6))
                artifact_bias = BIAS_CHAOS;
            break;
        case 27:
        case 28:
            add_flag(o_ptr->art_flags, TR_RES_SOUND);
            break;
        case 29:
        case 30:
            add_flag(o_ptr->art_flags, TR_RES_SHARDS);
            break;
        case 31:
        case 32:
            add_flag(o_ptr->art_flags, TR_RES_NETHER);
            if (!artifact_bias && one_in_(3))
                artifact_bias = BIAS_NECROMANTIC;
            break;
        case 33:
        case 34:
            add_flag(o_ptr->art_flags, TR_RES_NEXUS);
            break;
        case 35:
        case 36:
            add_flag(o_ptr->art_flags, TR_RES_CHAOS);
            if (!artifact_bias && one_in_(2))
                artifact_bias = BIAS_CHAOS;
            break;
        case 37:
        case 38:
            add_flag(o_ptr->art_flags, TR_RES_DISEN);
            break;
        case 39:
            if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
                add_flag(o_ptr->art_flags, TR_SH_ELEC);
            else
                random_resistance(o_ptr);
            if (!artifact_bias)
                artifact_bias = BIAS_ELEC;
            break;
        case 40:
            if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
                add_flag(o_ptr->art_flags, TR_SH_FIRE);
            else
                random_resistance(o_ptr);
            if (!artifact_bias)
                artifact_bias = BIAS_FIRE;
            break;
        case 41:
            if (o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_CLOAK ||
                o_ptr->tval == TV_HELM || o_ptr->tval == TV_HARD_ARMOR)
                add_flag(o_ptr->art_flags, TR_REFLECT);
            else
                random_resistance(o_ptr);
            break;
        case 42:
            if (o_ptr->tval >= TV_CLOAK && o_ptr->tval <= TV_HARD_ARMOR)
                add_flag(o_ptr->art_flags, TR_SH_COLD);
            else
                random_resistance(o_ptr);
            if (!artifact_bias)
                artifact_bias = BIAS_COLD;
            break;
    }
}



static void random_misc(object_type * o_ptr)
{
    switch (artifact_bias)
    {
    case BIAS_RANGER:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_CON);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_STR:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_STR)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_STR);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_WIS:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_WIS)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_WIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_INT:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_INT)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_INT);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_DEX:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_DEX)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_DEX);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CON:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_CON);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CHR:
        if (!(have_flag(o_ptr->art_flags, TR_SUST_CHR)))
        {
            add_flag(o_ptr->art_flags, TR_SUST_CHR);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_CHAOS:
        if (!(have_flag(o_ptr->art_flags, TR_TELEPORT)))
        {
            add_flag(o_ptr->art_flags, TR_TELEPORT);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_FIRE:
        if (!(have_flag(o_ptr->art_flags, TR_LITE)))
        {
            add_flag(o_ptr->art_flags, TR_LITE); /* Freebie */
        }
        break;
    }

    switch (randint1(33))
    {
        case 1:
            add_flag(o_ptr->art_flags, TR_SUST_STR);
            if (!artifact_bias)
                artifact_bias = BIAS_STR;
            break;
        case 2:
            add_flag(o_ptr->art_flags, TR_SUST_INT);
            if (!artifact_bias)
                artifact_bias = BIAS_INT;
            break;
        case 3:
            add_flag(o_ptr->art_flags, TR_SUST_WIS);
            if (!artifact_bias)
                artifact_bias = BIAS_WIS;
            break;
        case 4:
            add_flag(o_ptr->art_flags, TR_SUST_DEX);
            if (!artifact_bias)
                artifact_bias = BIAS_DEX;
            break;
        case 5:
            add_flag(o_ptr->art_flags, TR_SUST_CON);
            if (!artifact_bias)
                artifact_bias = BIAS_CON;
            break;
        case 6:
            add_flag(o_ptr->art_flags, TR_SUST_CHR);
            if (!artifact_bias)
                artifact_bias = BIAS_CHR;
            break;
        case 7:
        case 8:
        case 14:
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
            break;
        case 9:
            add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
            if (!artifact_bias && one_in_(5))
                artifact_bias = BIAS_PRIESTLY;
            else if (!artifact_bias && one_in_(6))
                artifact_bias = BIAS_NECROMANTIC;
            break;
        case 10:
        case 11:
            add_flag(o_ptr->art_flags, TR_LITE);
            break;
        case 12:
        case 13:
            add_flag(o_ptr->art_flags, TR_LEVITATION);
            break;
        case 15:
        case 16:
        case 17:
            add_flag(o_ptr->art_flags, TR_SEE_INVIS);
            break;
        case 19:
        case 20:
            add_flag(o_ptr->art_flags, TR_SLOW_DIGEST);
            break;
        case 21:
        case 22:
            add_flag(o_ptr->art_flags, TR_REGEN);
            break;
        case 23:
            add_flag(o_ptr->art_flags, TR_TELEPORT);
            break;
        case 24:
        case 25:
        case 26:
            if (object_is_armour(o_ptr))
                random_misc(o_ptr);
            else
            {
                o_ptr->to_a = 4 + randint1(11);
            }
            break;
        case 27:
        case 28:
        case 29:
        {
            int bonus_h, bonus_d;
            add_flag(o_ptr->art_flags, TR_SHOW_MODS);
            bonus_h = 4 + (randint1(11));
            bonus_d = 4 + (randint1(11));
            if ((o_ptr->tval != TV_SWORD) && (o_ptr->tval != TV_POLEARM) && (o_ptr->tval != TV_HAFTED) && (o_ptr->tval != TV_DIGGING) && (o_ptr->tval != TV_GLOVES) && (o_ptr->tval != TV_RING))
            {
                bonus_h /= 2;
                bonus_d /= 2;
            }
            o_ptr->to_h += bonus_h;
            o_ptr->to_d += bonus_d;
            break;
        }
        case 30:
            add_flag(o_ptr->art_flags, TR_NO_MAGIC);
            break;
        case 31:
            add_flag(o_ptr->art_flags, TR_NO_TELE);
            break;
        case 32:
            add_flag(o_ptr->art_flags, TR_WARNING);
            break;

        case 18:
            switch (randint1(3))
            {
            case 1:
                add_flag(o_ptr->art_flags, TR_ESP_EVIL);
                if (!artifact_bias && one_in_(3))
                    artifact_bias = BIAS_LAW;
                break;
            case 2:
                add_flag(o_ptr->art_flags, TR_ESP_NONLIVING);
                if (!artifact_bias && one_in_(3))
                    artifact_bias = BIAS_MAGE;
                break;
            case 3:
                add_flag(o_ptr->art_flags, TR_TELEPATHY);
                if (!artifact_bias && one_in_(9))
                    artifact_bias = BIAS_MAGE;
                break;
            }
            break;

        case 33:
        {
            int idx[3];
            int n = randint1(3);

            idx[0] = randint1(8);

            idx[1] = randint1(7);
            if (idx[1] >= idx[0]) idx[1]++;

            idx[2] = randint1(6);
            if (idx[2] >= idx[0]) idx[2]++;
            if (idx[2] >= idx[1]) idx[2]++;

            while (n--) switch (idx[n])
            {
            case 1:
                add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);
                if (!artifact_bias && one_in_(4))
                    artifact_bias = BIAS_RANGER;
                break;
            case 2:
                add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);
                if (!artifact_bias && one_in_(3))
                    artifact_bias = BIAS_PRIESTLY;
                else if (!artifact_bias && one_in_(6))
                    artifact_bias = BIAS_NECROMANTIC;
                break;
            case 3:
                add_flag(o_ptr->art_flags, TR_ESP_DEMON);
                break;
            case 4:
                add_flag(o_ptr->art_flags, TR_ESP_ORC);
                break;
            case 5:
                add_flag(o_ptr->art_flags, TR_ESP_TROLL);
                break;
            case 6:
                add_flag(o_ptr->art_flags, TR_ESP_GIANT);
                break;
            case 7:
                add_flag(o_ptr->art_flags, TR_ESP_HUMAN);
                if (!artifact_bias && one_in_(6))
                    artifact_bias = BIAS_ROGUE;
                break;
            case 8:
                add_flag(o_ptr->art_flags, TR_ESP_GOOD);
                if (!artifact_bias && one_in_(3))
                    artifact_bias = BIAS_LAW;
                break;
            }
            break;
        }
    }
}

static void random_slay_aux(object_type *o_ptr)
{
    switch (randint1(29 + object_level/10))
    {
        case 1:
        case 2:
            add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
            break;
        case 3:
        case 4:
            add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
            break;
        case 5:
            add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
            break;
        case 6:
            add_flag(o_ptr->art_flags, TR_SLAY_ORC);
            break;
        case 7:
        case 8:
            add_flag(o_ptr->art_flags, TR_SLAY_TROLL);
            break;
        case 9:
        case 10:
            add_flag(o_ptr->art_flags, TR_SLAY_GIANT);
            break;
        case 11:
        case 12:
            if (randint1(150) <= object_level)
                add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
            else
                add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
            break;
        case 13:
        case 14:
            add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
            if (!artifact_bias && one_in_(9))
                artifact_bias = BIAS_PRIESTLY;
            break;
        case 15:
        case 16:
            add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
            if (!artifact_bias && one_in_(9))
                artifact_bias = BIAS_PRIESTLY;
            break;
        case 17:
        case 18:
        case 19:
            if (o_ptr->tval == TV_SWORD)
            {
                add_flag(o_ptr->art_flags, TR_VORPAL);
                if (!artifact_bias && one_in_(9))
                    artifact_bias = BIAS_WARRIOR;
            }
            else
                random_slay_aux(o_ptr);
            break;
        case 20:
            if (o_ptr->tval == TV_HAFTED || o_ptr->tval == TV_DIGGING)
                add_flag(o_ptr->art_flags, TR_IMPACT);
            else
                random_slay_aux(o_ptr);
            break;
        case 21:
            add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
            if (!artifact_bias)
                artifact_bias = BIAS_FIRE;
            break;
        case 22:
            add_flag(o_ptr->art_flags, TR_BRAND_COLD);
            if (!artifact_bias)
                artifact_bias = BIAS_COLD;
            break;
        case 23:
            add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
            if (!artifact_bias)
                artifact_bias = BIAS_ELEC;
            break;
        case 24:
            add_flag(o_ptr->art_flags, TR_BRAND_ACID);
            if (!artifact_bias)
                artifact_bias = BIAS_ACID;
            break;
        case 25:
            add_flag(o_ptr->art_flags, TR_BRAND_POIS);
            if (!artifact_bias && !one_in_(3))
                artifact_bias = BIAS_POIS;
            else if (!artifact_bias && one_in_(6))
                artifact_bias = BIAS_NECROMANTIC;
            else if (!artifact_bias)
                artifact_bias = BIAS_ROGUE;
            break;
        case 26:
            add_flag(o_ptr->art_flags, TR_VAMPIRIC);
            if (!artifact_bias)
                artifact_bias = BIAS_NECROMANTIC;
            break;
        case 27:
            if (have_flag(o_ptr->art_flags, TR_KILL_EVIL) && !one_in_(100))
            {
                random_slay_aux(o_ptr);
            }
            else if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL) && !one_in_(20))
            {
                random_slay_aux(o_ptr);
            }
            else
            {
                add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
                if (!artifact_bias)
                    artifact_bias = (one_in_(2) ? BIAS_MAGE : BIAS_PRIESTLY);
            }
            break;
        case 28:
            add_flag(o_ptr->art_flags, TR_CHAOTIC);
            if (!artifact_bias)
                artifact_bias = BIAS_CHAOS;
            break;
        case 29:
            add_flag(o_ptr->art_flags, TR_VAMPIRIC);
            if (!artifact_bias)
                artifact_bias = BIAS_NECROMANTIC;
            break;
        case 30:
            add_flag(o_ptr->art_flags, TR_KILL_DEMON);
            if (!artifact_bias && one_in_(3))
                artifact_bias = BIAS_PRIESTLY;
            break;
        case 31:
            add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
            if (!artifact_bias && one_in_(3))
                artifact_bias = BIAS_PRIESTLY;
            break;
        default:
            if (!have_flag(o_ptr->art_flags, TR_ORDER) && !have_flag(o_ptr->art_flags, TR_WILD) && one_in_(100))
                add_flag(o_ptr->art_flags, TR_ORDER);
            else if (!have_flag(o_ptr->art_flags, TR_WILD) && !have_flag(o_ptr->art_flags, TR_ORDER) &&  one_in_(100))
                add_flag(o_ptr->art_flags, TR_WILD);
            else 
            {
                if (randint1(2500) <= object_level)
                    add_flag(o_ptr->art_flags, TR_KILL_EVIL);
                else
                    add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
                if (!artifact_bias && one_in_(2))
                    artifact_bias = BIAS_LAW;
                else if (!artifact_bias && one_in_(9))
                    artifact_bias = BIAS_PRIESTLY;
            }
            break;
    }
}
static void random_slay(object_type *o_ptr)
{
    if (o_ptr->tval == TV_BOW)
    {
        if (o_ptr->sval == SV_CRIMSON || o_ptr->sval == SV_RAILGUN)
        {
            if (one_in_(2))
            {
                random_plus(o_ptr);
                has_pval = TRUE;
            }
            else
                one_high_resistance(o_ptr);
            return;
        }
        else if (o_ptr->sval == SV_HARP)
        {
            if (one_in_(2))
                random_resistance(o_ptr);
            else
            {
                random_plus(o_ptr);
                has_pval = TRUE;
            }
            return;
        }
        switch (randint1(6))
        {
            case 1:
                if (one_in_(100))
                {
                    random_slay_aux(o_ptr);
                    break;
                }
            case 2:
            case 3:
                add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
                if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
                if (!artifact_bias && one_in_(9))
                    artifact_bias = BIAS_RANGER;
                break;
            default:
                add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
                if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
                has_pval = TRUE;
                if (!artifact_bias && one_in_(9))
                    artifact_bias = BIAS_RANGER;
                break;
        }
        return;
    }

    switch (artifact_bias)
    {
    case BIAS_CHAOS:
        if (!(have_flag(o_ptr->art_flags, TR_CHAOTIC)))
        {
            add_flag(o_ptr->art_flags, TR_CHAOTIC);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_PRIESTLY:
        if((o_ptr->tval == TV_SWORD || o_ptr->tval == TV_POLEARM) &&
           !(have_flag(o_ptr->art_flags, TR_BLESSED)))
        {
            /* A free power for "priestly" random artifacts */
            add_flag(o_ptr->art_flags, TR_BLESSED);
        }
        break;

    case BIAS_NECROMANTIC:
        if (!(have_flag(o_ptr->art_flags, TR_VAMPIRIC)))
        {
            add_flag(o_ptr->art_flags, TR_VAMPIRIC);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)) && one_in_(2))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_POIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_RANGER:
        if (!(have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL)))
        {
            add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_ROGUE:
        if ((((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DAGGER)) ||
             ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_SPEAR))) &&
             !(have_flag(o_ptr->art_flags, TR_THROW)))
        {
            /* Free power for rogues... */
            add_flag(o_ptr->art_flags, TR_THROW);
        }
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_POIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_POIS:
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_POIS);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_FIRE:
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_FIRE)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_COLD:
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_COLD)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_COLD);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_ELEC:
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_ELEC)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_ACID:
        if (!(have_flag(o_ptr->art_flags, TR_BRAND_ACID)))
        {
            add_flag(o_ptr->art_flags, TR_BRAND_ACID);
            if (one_in_(2)) return;
        }
        break;

    case BIAS_LAW:
        if (!(have_flag(o_ptr->art_flags, TR_SLAY_EVIL)))
        {
            add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD)))
        {
            add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
            if (one_in_(2)) return;
        }
        if (!(have_flag(o_ptr->art_flags, TR_SLAY_DEMON)))
        {
            add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
            if (one_in_(2)) return;
        }
        break;
    }

    random_slay_aux(o_ptr);
}

static void get_random_name_aux(char *return_name, object_type *o_ptr, int power)
{
    /* Hack: BIAS_* got converted to bits but the artifact name files still use
       old values. Fortunately, the bit position in the new constants will 
       recover the old values.
       TODO: Change the parser (e.g. N:18:BIAS_WARRIOR -> N:BIAS_WARRIOR)
       so this doesn't happen again. 
    */
    int bias_hack = 0;
    int temp = artifact_bias;

    while (temp)
    {
        temp >>= 1;
        bias_hack++;
    }

    if (o_ptr->tval == TV_LITE)
    {
        cptr filename;
        switch (power)
        {
        case 0:
            filename = "lite_cursed.txt";
            break;
        case 1:
            filename = "lite_low.txt";
            break;
        case 2:
            filename = "lite_med.txt";
            break;
        default:
            filename = "lite_high.txt";
        }
        get_rnd_line(filename, bias_hack, return_name);
    }
    else if (o_ptr->tval == TV_RING)
    {
        cptr filename;
        switch (power)
        {
        case 0:
            filename = "ring_cursed.txt";
            break;
        case 1:
            filename = "ring_low.txt";
            break;
        case 2:
            filename = "ring_med.txt";
            break;
        default:
            filename = "ring_high.txt";
        }
        get_rnd_line(filename, bias_hack, return_name);
    }
    else if (o_ptr->tval == TV_AMULET)
    {
        cptr filename;
        switch (power)
        {
        case 0:
            filename = "amulet_cursed.txt";
            break;
        case 1:
            filename = "amulet_low.txt";
            break;
        case 2:
            filename = "amulet_med.txt";
            break;
        default:
            filename = "amulet_high.txt";
        }
        get_rnd_line(filename, bias_hack, return_name);
    }
    else
    {
        cptr filename;

        if (object_is_armour(o_ptr))
        {
            switch (power)
            {
            case 0:
                filename = "a_cursed.txt";
                break;
            case 1:
            case 2:
                filename = "a_med.txt";
                break;
            default:
                filename = "a_high.txt";
            }
        }
        else
        {
            switch (power)
            {
            case 0:
                filename = "w_cursed.txt";
                break;
            case 1:
            case 2:
                filename = "w_med.txt";
                break;
            default:
                filename = "w_high.txt";
            }
        }

        get_rnd_line(filename, bias_hack, return_name);
    }
}

void get_random_name(char *return_name, object_type *o_ptr, int power)
{
    int attempt;
    return_name[0] = '\0';
    for (attempt = 0;; ++attempt)
    {
        get_random_name_aux(return_name, o_ptr, power);
        if (return_name[0] != '\0') break;
        if (attempt > 100) break;
    }
}

void curse_object(object_type *o_ptr)
{
    int ct = randint1(2);
    int v = object_value_real(o_ptr) / 10000;
    int i;
    
    o_ptr->curse_flags |= TRC_CURSED;
    
    one_biff(o_ptr);
    ct--;

    ct += randint1(randint1(v)); /* TODO */
    
    for (i = 0; i < ct; i++)
    {
        int n = randint0(70 + v*v);
        if (n < 25)
        {
            do { o_ptr->curse_flags |= get_curse(0, o_ptr); } while (one_in_(2));
        }
        else if (n < 35)
            o_ptr->curse_flags |= get_curse(1, o_ptr);
        else if (n < 40)
            o_ptr->curse_flags |= TRC_HEAVY_CURSE;
        else if (n < 45)
            o_ptr->curse_flags |= get_curse(2, o_ptr);
        else if (n < 100)
            one_biff(o_ptr);
        else if (n < 105)
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
        else if (n < 107)
            add_flag(o_ptr->art_flags, TR_TY_CURSE);
        else if (n < 108)
            o_ptr->curse_flags |= TRC_PERMA_CURSE;
        else if (n < 250)
            one_biff(o_ptr);
        else
        {
            if (one_in_(2))
            {
                o_ptr->curse_flags |= TRC_PERMA_CURSE;
                if (one_in_(2))
                    add_flag(o_ptr->art_flags, TR_TY_CURSE);
                if (one_in_(2))
                    add_flag(o_ptr->art_flags, TR_NO_TELE);
                if (one_in_(2))
                    add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            }
            else
                one_biff(o_ptr);
        }
    }
}

s32b create_artifact(object_type *o_ptr, u32b mode)
{
    char    new_name[1024];
    int     powers = 0;
    int     power_level;
    s32b    total_flags;
    bool    a_cursed = FALSE;
    int     warrior_artifact_bias = 0;
    int     i;
    bool    has_resistance = FALSE, boosted_ac = FALSE, boosted_dam = FALSE, boosted_hit = FALSE, esp_count = 0;
    int     lev = object_level;
    bool    is_falcon_sword = FALSE;
    int     max_a = MAX(o_ptr->to_a, 30);
    int     max_h = MAX(o_ptr->to_h, 32);
    int     max_d = MAX(o_ptr->to_d, 32);

    if (no_artifacts) return 0;
    
    if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_FALCON_SWORD)
        is_falcon_sword = TRUE;

    immunity_hack = FALSE;
    slaying_hack = 0;
    has_pval = FALSE;

    if (o_ptr->tval == TV_BOW && o_ptr->sval == SV_HARP)
        has_pval = TRUE;

    /* Hack for Demeter. Torches start with a pval of 4000! Not sure about lanterns ... */
    if (o_ptr->tval == TV_LITE /*&& o_ptr->sval == SV_LITE_TORCH*/)
        o_ptr->pval = 0;

    if (lev > 127)
        lev = 127; /* no going to heaven or hell for uber nutso craziness */

    /* Reset artifact bias */
    artifact_bias = 0;

    /* Nuke enchantments */
    o_ptr->name1 = 0;
    o_ptr->name2 = 0;
    o_ptr->name3 = 0;

    for (i = 0; i < TR_FLAG_SIZE; i++)
        o_ptr->art_flags[i] |= k_info[o_ptr->k_idx].flags[i];

    if (o_ptr->pval) has_pval = TRUE;

    if ((mode & (CREATE_ART_SCROLL/* | CREATE_ART_GOOD*/)) && one_in_(4))
    {
        switch (p_ptr->pclass)
        {
            case CLASS_WARRIOR:
            case CLASS_BERSERKER:
            case CLASS_ARCHER:
            case CLASS_SAMURAI:
            case CLASS_CAVALRY:
            case CLASS_WEAPONSMITH:
            case CLASS_DUELIST:
            case CLASS_BLOOD_KNIGHT:
            case CLASS_WEAPONMASTER:
            case CLASS_RUNE_KNIGHT:
            case CLASS_MAULER:
                artifact_bias = BIAS_WARRIOR;
                break;
            case CLASS_MAGE:
            case CLASS_HIGH_MAGE:
            case CLASS_SORCERER:
            case CLASS_MAGIC_EATER:
            case CLASS_BLUE_MAGE:
            case CLASS_WARLOCK:
            case CLASS_BLOOD_MAGE:
            case CLASS_NECROMANCER:
            case CLASS_DEVICEMASTER:
                artifact_bias = BIAS_MAGE;
                warrior_artifact_bias = 20;
                break;
            case CLASS_PRIEST:
                artifact_bias = BIAS_PRIESTLY;
                warrior_artifact_bias = 30;
                break;
            case CLASS_ROGUE:
            case CLASS_NINJA:
            case CLASS_SCOUT:
                artifact_bias = BIAS_ROGUE;
                warrior_artifact_bias = 30;
                break;
            case CLASS_RANGER:
            case CLASS_SNIPER:
                artifact_bias = BIAS_RANGER;
                warrior_artifact_bias = 30;
                break;
            case CLASS_PALADIN:
                artifact_bias = BIAS_PRIESTLY;
                warrior_artifact_bias = 60;
                break;
            case CLASS_WARRIOR_MAGE:
            case CLASS_RED_MAGE:
            case CLASS_PSION:
                artifact_bias = BIAS_MAGE;
                warrior_artifact_bias = 20;
                break;
            case CLASS_CHAOS_WARRIOR:
                artifact_bias = BIAS_CHAOS;
                warrior_artifact_bias = 60;
                break;
            case CLASS_MONK:
            case CLASS_FORCETRAINER:
                artifact_bias = BIAS_PRIESTLY;
                warrior_artifact_bias = 40;
                break;
            case CLASS_MINDCRAFTER:
                artifact_bias = BIAS_PRIESTLY;
                warrior_artifact_bias = 20;
                break;
            case CLASS_BARD:
                artifact_bias = BIAS_PRIESTLY;
                warrior_artifact_bias = 20;
                break;
            case CLASS_TOURIST:
            case CLASS_ARCHAEOLOGIST:
            case CLASS_TIME_LORD:
                warrior_artifact_bias = 20;
                break;
            case CLASS_IMITATOR:
                artifact_bias = BIAS_RANGER;
                warrior_artifact_bias = 40;
                break;
            case CLASS_BEASTMASTER:
                artifact_bias = BIAS_CHR;
                warrior_artifact_bias = 30;
                break;
            case CLASS_MIRROR_MASTER:
                if (randint1(4) > 1) 
                {
                    artifact_bias = BIAS_MAGE;
                }
                else
                {
                    artifact_bias = BIAS_ROGUE;
                }
                warrior_artifact_bias = 20;
                break;

            default:
                artifact_bias = BIAS_WARRIOR;
                break;
        }
    }

    if ((mode & (CREATE_ART_SCROLL | CREATE_ART_GOOD)) && (randint1(100) <= warrior_artifact_bias))
        artifact_bias = BIAS_WARRIOR;

    strcpy(new_name, "");

    if (!(mode & CREATE_ART_GOOD) && one_in_(A_CURSED))
        a_cursed = TRUE;

    if (mode & CREATE_ART_CURSED)
        a_cursed = TRUE;

    if ( (o_ptr->tval == TV_AMULET || o_ptr->tval == TV_RING) 
      && object_is_cursed(o_ptr) )
    {
        a_cursed = TRUE;
    }

    powers = randint1(5) + 1;

    while (one_in_(powers) || one_in_(7 * 90/MAX(MIN(127, object_level), 1)) || one_in_(10 * 70/MAX(MIN(127, object_level), 1)))
        powers++;

    if (one_in_(WEIRD_LUCK))
        powers *= 2;

    if ( (o_ptr->tval == TV_LITE && o_ptr->sval != SV_LITE_JUDGE) 
      || o_ptr->tval == TV_AMULET
      || o_ptr->tval == TV_RING ) 
    {
        if (!one_in_(WEIRD_LUCK))
        {
            if (powers > 3) powers = powers*3/4;
            if (powers > 5) powers = 5;

            /* Artifacting high rings of damage is now possible ... */
            if (o_ptr->to_d)
                o_ptr->to_d = randint1((o_ptr->to_d + 1)/2) + randint1(o_ptr->to_d/2);
            if (o_ptr->to_a)
                o_ptr->to_a = randint1((o_ptr->to_a + 1)/2) + randint1(o_ptr->to_a/2);
        }
    }

    /* Playtesting shows that FA, SI and HL are too rare ... let's boost these a bit */
    switch (o_ptr->tval)
    {
    case TV_BOLT:
    case TV_SHOT:
    case TV_ARROW:
        o_ptr->number = 1;    /* Where's the best place for this? */
        break;

    case TV_RING:
        if (one_in_(2))
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
        if (one_in_(2))
            add_flag(o_ptr->art_flags, TR_SEE_INVIS);
        break;

    case TV_AMULET:
        if (one_in_(3))
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
        if (one_in_(3))
            add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
        break;

        
    case TV_LITE:
        if (one_in_(3))
            add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
        if (one_in_(10))
            add_flag(o_ptr->art_flags, TR_DARKNESS);
        break;

    case TV_BOOTS:
        if (one_in_(4))
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
        if (one_in_(7))
            add_flag(o_ptr->art_flags, TR_LEVITATION);
        break;

    case TV_GLOVES:
        if (one_in_(4))
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
        break;

    case TV_HELM:
        if (one_in_(4))
            add_flag(o_ptr->art_flags, TR_SEE_INVIS);
        break;
    }

    /* Main loop */
    while (powers > 0)
    {
        powers--;
        /* Attempt to craft artifacts by appropriate to type.  For example, arrows should
           not grant extra blows or +3 strength :)  More realistically, weapons should
           preferentially get slays while armor should favor resists */
        switch (o_ptr->tval)
        {
        case TV_BOLT:
        case TV_SHOT:
        case TV_ARROW:
            /* Ammo either boosts up damage dice, or gains slays */
            if ( randint1(225) < lev
              && slaying_hack == 0 ) /* OK: Slaying can now only happen once! */
            {
                if (slaying_hack == 0 && one_in_(3)) /* double damage */
                {
                    powers -= o_ptr->dd - 1;
                    o_ptr->dd *= 2;
                    slaying_hack += o_ptr->dd;
                }
                else
                {
                    powers++;
                    powers++;
                    do
                    {
                        o_ptr->dd++;
                        powers--;
                        slaying_hack++;
                    }
                    while (one_in_(o_ptr->dd));
                        
                    do
                    {
                        o_ptr->ds++;
                        powers--;
                        slaying_hack++;
                    }
                    while (one_in_(o_ptr->ds));
                }            
            }
            else
                random_slay(o_ptr);
            break;

        case TV_LITE:
            switch (randint1(7))
            {
            case 1: case 2:
                random_plus(o_ptr);
                has_pval = TRUE;
                break;
            case 3:
                if (one_in_(45))
                {
                    add_flag(o_ptr->art_flags, TR_TELEPATHY);
                }
                else if (one_in_(15))
                {
                    effect_add(o_ptr, EFFECT_CLAIRVOYANCE);
                }
                else if (one_in_(77))
                {
                    add_flag(o_ptr->art_flags, TR_SPELL_POWER);
                    add_flag(o_ptr->art_flags, TR_DEC_STR);
                    add_flag(o_ptr->art_flags, TR_DEC_DEX);
                    add_flag(o_ptr->art_flags, TR_DEC_CON);
                    has_pval = TRUE;
                }
                else
                {
                    random_plus(o_ptr);
                    has_pval = TRUE;
                }
                break;
            case 4: case 5: case 6:
                if (one_in_(3))
                    one_high_resistance(o_ptr);
                else
                    random_resistance(o_ptr);
                break;
            case 7:
                random_misc(o_ptr);
                break;
            }        
            break;

        case TV_SWORD:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_DIGGING:
            switch (randint1(7))
            {
            case 1: case 2:
                random_plus(o_ptr);
                has_pval = TRUE;
                break;
            case 3:
                if ( slaying_hack == 0 /* OK: Slaying can now only happen once! */
                    && (is_falcon_sword || !have_flag(o_ptr->art_flags, TR_BLOWS))
                    && randint1(225) < lev)
                {
                    int odds = o_ptr->dd * o_ptr->ds / 2;
                    if (odds < 3) odds = 3;
                    /* spiked code from EGO_SLAYING_WEAPON */
                    if (slaying_hack == 0 && one_in_(odds)) /* double damage */
                    {
                        powers -= o_ptr->dd - 1;
                        slaying_hack += o_ptr->dd;
                        o_ptr->dd *= 2;
                    }
                    else
                    {
                        powers++;
                        powers++;
                        do
                        {
                            o_ptr->dd++;
                            powers--;
                            slaying_hack++;
                        }
                        while (one_in_(o_ptr->dd));
                        
                        do
                        {
                            o_ptr->ds++;
                            powers--;
                            slaying_hack++;
                        }
                        while (one_in_(o_ptr->ds));
                    }            
                }
                else if (!boosted_dam && !boosted_hit && randint1(225) < lev)
                {
                    if (one_in_(7))
                    {
                        o_ptr->to_h = 10 + randint1(22);
                        o_ptr->to_d = 10 + randint1(22);
                        boosted_dam = TRUE;
                        boosted_hit = TRUE;
                        powers--;
                    }
                    else if (one_in_(2))
                    {
                        o_ptr->to_h = 10 + randint1(22);
                        boosted_hit = TRUE;
                    }
                    else
                    {
                        o_ptr->to_d = 10 + randint1(22);
                        boosted_dam = TRUE;
                    }
                }
                else
                    random_resistance(o_ptr);
                break;
            case 4:
                random_misc(o_ptr);
                break;
            case 5: case 6: case 7: 
                random_slay(o_ptr);
                break;
            }
            break;

        case TV_BOW:
            switch (randint1(8))
            {
            case 1: case 2:
                random_plus(o_ptr);
                has_pval = TRUE;
                break;
            case 3: case 4: case 5:
                random_slay(o_ptr);
                break;
            case 6:
                random_misc(o_ptr);
                break;
            case 7: case 8:
                random_resistance(o_ptr);
                break;
            }
            break;

        case TV_RING:
            switch (randint1(7))
            {
            case 1: case 2:
                random_plus(o_ptr);
                has_pval = TRUE;
                break;
            case 3:
                if (one_in_(10))
                {
                    add_flag(o_ptr->art_flags, TR_SPEED);
                    has_pval = TRUE;
                }
                else if ( (artifact_bias == BIAS_MAGE && one_in_(10))
                       || one_in_(50) )
                {
                    add_flag(o_ptr->art_flags, TR_DEC_MANA);
                }
                else if (one_in_(100))
                {
                    add_flag(o_ptr->art_flags, TR_WEAPONMASTERY);
                    has_pval = TRUE;
                }
                else if (one_in_(66) && randint1(150) < lev - 50)
                {
                    add_flag(o_ptr->art_flags, TR_BLOWS);
                    has_pval = TRUE;
                }
                else if (one_in_(15))
                {
                    add_flag(o_ptr->art_flags, TR_SUST_STR);
                    add_flag(o_ptr->art_flags, TR_SUST_INT);
                    add_flag(o_ptr->art_flags, TR_SUST_WIS);
                    add_flag(o_ptr->art_flags, TR_SUST_DEX);
                    add_flag(o_ptr->art_flags, TR_SUST_CON);
                    add_flag(o_ptr->art_flags, TR_SUST_CHR);
                    add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
                    powers--;
                }
                else if (one_in_(77))
                {
                    add_flag(o_ptr->art_flags, TR_SPELL_POWER);
                    add_flag(o_ptr->art_flags, TR_DEC_STR);
                    add_flag(o_ptr->art_flags, TR_DEC_DEX);
                    add_flag(o_ptr->art_flags, TR_DEC_CON);
                    has_pval = TRUE;
                }
                else if (one_in_(3))
                {
                    add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                    o_ptr->to_h = 4 + (randint1(11));
                    o_ptr->to_d = 4 + (randint1(11));
                }
                else
                {
                    one_high_resistance(o_ptr);
                }
                break;
            case 4: case 5: case 6:
                random_resistance(o_ptr);
                break;
            case 7:
                random_misc(o_ptr);
                break;
            }
            break;

        case TV_AMULET:
            switch (randint1(7))
            {
            case 1: case 2:
                random_plus(o_ptr);
                has_pval = TRUE;
                break;
            case 3:
                if (one_in_(45))
                {
                    add_flag(o_ptr->art_flags, TR_TELEPATHY);
                }
                else if ( (artifact_bias == BIAS_MAGE && one_in_(10))
                       || one_in_(50) )
                {
                    add_flag(o_ptr->art_flags, TR_DEC_MANA);
                }
                else if (one_in_(7))
                {
                    add_flag(o_ptr->art_flags, TR_REFLECT);
                }
                else if (one_in_(30))
                {
                    add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
                    has_pval = TRUE;
                }
                else if (one_in_(77))
                {
                    add_flag(o_ptr->art_flags, TR_SPELL_POWER);
                    add_flag(o_ptr->art_flags, TR_DEC_STR);
                    add_flag(o_ptr->art_flags, TR_DEC_DEX);
                    add_flag(o_ptr->art_flags, TR_DEC_CON);
                    has_pval = TRUE;
                }
                else if (one_in_(3))
                {
                    add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                    o_ptr->to_h = 4 + (randint1(11));
                    o_ptr->to_d = 4 + (randint1(11));
                }
                else
                {
                    one_high_resistance(o_ptr);
                }
                break;
            case 4: case 5: case 6:
                random_resistance(o_ptr);
                break;
            case 7:
                random_misc(o_ptr);
                break;
            }
            break;

        default: /* Assume Armor */
            switch (randint1(7))
            {
                case 1: case 2:
                    random_plus(o_ptr);
                    has_pval = TRUE;
                    break;
                case 3:
                    if (!has_resistance 
                      && (object_is_body_armour(o_ptr) || object_is_shield(o_ptr)) 
                      && one_in_(4) )
                    {
                        add_flag(o_ptr->art_flags, TR_RES_ACID);
                        add_flag(o_ptr->art_flags, TR_RES_COLD);
                        add_flag(o_ptr->art_flags, TR_RES_FIRE);
                        add_flag(o_ptr->art_flags, TR_RES_ELEC);
                        if (one_in_(3))
                            add_flag(o_ptr->art_flags, TR_RES_POIS);
                        has_resistance = TRUE;
                        powers -= 3;
                    }
                    else if (o_ptr->tval == TV_BOOTS && one_in_(3))
                    {
                        add_flag(o_ptr->art_flags, TR_SPEED);
                        has_pval = TRUE;
                    }
                    else if (o_ptr->tval == TV_BOOTS && one_in_(2))
                    {
                        add_flag(o_ptr->art_flags, TR_LEVITATION);
                    }
                    else if ( (o_ptr->tval == TV_CROWN && one_in_(2 + esp_count*10))
                           || (o_ptr->tval == TV_HELM && one_in_(7 + esp_count*10)) )
                    {
                        if (add_esp_strong(o_ptr))
                            add_esp_weak(o_ptr, TRUE);
                        else
                            add_esp_weak(o_ptr, FALSE);
                        ++esp_count;
                    }
                    else if (!boosted_ac && randint1(225) < lev)
                    {
                        o_ptr->to_a = 20 + randint1(20);
                        if (object_is_body_armour(o_ptr) && one_in_(7))
                            o_ptr->ac += 5;

                        boosted_ac = TRUE;
                        powers--;
                    }
                    else if (o_ptr->tval == TV_GLOVES && one_in_(2))
                    {
                        add_flag(o_ptr->art_flags, TR_SHOW_MODS);
                        o_ptr->to_h = 4 + (randint1(11));
                        o_ptr->to_d = 4 + (randint1(11));
                    }
                    else if (o_ptr->tval == TV_GLOVES && one_in_(2))
                    {
                        add_flag(o_ptr->art_flags, TR_DEX);
                        has_pval = TRUE;
                    }
                    else if (o_ptr->tval == TV_GLOVES && one_in_(77))
                    {
                        add_flag(o_ptr->art_flags, TR_BLOWS);
                        has_pval = TRUE;
                    }
                    else
                        one_high_resistance(o_ptr);
                    break;
                case 4:
                    if (!(object_is_body_armour(o_ptr) || object_is_shield(o_ptr))
                      || one_in_(3) )
                    {
                        random_misc(o_ptr);
                        break;
                    }
                case 5: case 6: case 7: 
                    random_resistance(o_ptr);                    
                    break;
            }
        }
    };

    if (has_pval)
    {
        if (have_flag(o_ptr->art_flags, TR_BLOWS))
        {
            if (o_ptr->tval == TV_RING || o_ptr->tval == TV_GLOVES)
            {
                o_ptr->pval = randint1(2);
                if (one_in_(30)) o_ptr->pval++;
            }
            else
            {
                o_ptr->pval = randint1(3);
                if (one_in_(15)) o_ptr->pval++;
                if (is_falcon_sword)
                    o_ptr->pval += randint1(2);
            }
        }
        else
        {
            /* Hengband:  1: 0.0%  2:20.0%  3:32.0%  4+:48.0%  
               Chengband: 1:16.7%  2:28.6%  3:28.6%  4+:26.0% 
               4+ usually becomes a 4. The only tweak I made was changing
               "|| one_in_(pval)" to "|| one_in_(pval+5)."
            */
            if (mode & CREATE_ART_SCROLL)
                o_ptr->pval = 0; /* Surprise! Always re-roll the pval ... */
            do
            {
                o_ptr->pval++;
            }
            while (o_ptr->pval < randint1(5) || one_in_(o_ptr->pval+5));
        }
    }

    if (o_ptr->pval > 4 && !one_in_(WEIRD_LUCK))
        o_ptr->pval = 4;

    if (o_ptr->tval == TV_DIGGING && !have_flag(o_ptr->art_flags, TR_BLOWS))
    {
        o_ptr->pval += randint1(2);
        if (o_ptr->pval > 6)
            o_ptr->pval = 6;
    }

    if (have_flag(o_ptr->art_flags, TR_WEAPONMASTERY) && o_ptr->pval > 2)
    {
        if (one_in_(30))
            o_ptr->pval = 3;
        else
            o_ptr->pval = 2;
    }

    if (have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY) && o_ptr->pval > 2)
    {
        if (one_in_(30))
            o_ptr->pval = 3;
        else
            o_ptr->pval = 2;
    }

    /* give it some plusses... */
    if (object_is_armour(o_ptr))
    {
        if (!boosted_ac)
            o_ptr->to_a += randint1(o_ptr->to_a > 19 ? 1 : 20 - o_ptr->to_a);

        if (o_ptr->to_a > max_a)
            o_ptr->to_a = max_a;
    }
    else if (object_is_weapon_ammo(o_ptr))
    {
        if (!boosted_dam)
            o_ptr->to_d += randint1(o_ptr->to_d > 19 ? 1 : 20 - o_ptr->to_d);

        if (!boosted_hit)
            o_ptr->to_h += randint1(o_ptr->to_h > 19 ? 1 : 20 - o_ptr->to_h);

        if (o_ptr->to_h > max_h)
            o_ptr->to_h = max_h;

        if (o_ptr->to_d > max_d)
            o_ptr->to_d = max_d;

        if ((have_flag(o_ptr->art_flags, TR_WIS)) && (o_ptr->pval > 0)) add_flag(o_ptr->art_flags, TR_BLESSED);
    }

    /* Fix up Damage Dice for (Wild) and (Order) weapons */
    if (have_flag(o_ptr->art_flags, TR_ORDER))
    {
        remove_flag(o_ptr->art_flags, TR_WILD);
        o_ptr->dd = o_ptr->dd * o_ptr->ds;
        o_ptr->ds = 1;
    }
    else if (have_flag(o_ptr->art_flags, TR_WILD))
    {
        o_ptr->ds = o_ptr->dd * o_ptr->ds;
        o_ptr->dd = 1;
    }

    /* Just to be sure */
    add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
    add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
    add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
    add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

    if ( !have_flag(o_ptr->art_flags, TR_ACTIVATE)
      && !object_is_ammo(o_ptr) )
    {
        int odds = object_is_armour(o_ptr) ? ACTIVATION_CHANCE * 2 : ACTIVATION_CHANCE;
        if (one_in_(odds))
        {
            effect_add_random(o_ptr, artifact_bias);
        }
    }

    if (object_is_armour(o_ptr) || o_ptr->tval == TV_RING || o_ptr->tval == TV_AMULET)
    {
        int lower = 10;

        if (o_ptr->tval == TV_RING)
            lower = 15;

        while ((o_ptr->to_d+o_ptr->to_h) > 20)
        {
            if (one_in_(o_ptr->to_d) && one_in_(o_ptr->to_h)) break;
            o_ptr->to_d -= (s16b)randint0(3);
            o_ptr->to_h -= (s16b)randint0(3);
        }
        while ((o_ptr->to_d+o_ptr->to_h) > lower && !immunity_hack)
        {
            if (one_in_(o_ptr->to_d) || one_in_(o_ptr->to_h)) break;
            o_ptr->to_d -= (s16b)randint0(3);
            o_ptr->to_h -= (s16b)randint0(3);
        }
    }

    if ((artifact_bias == BIAS_MAGE || artifact_bias == BIAS_INT) 
      && (o_ptr->tval == TV_GLOVES)) 
    {
        add_flag(o_ptr->art_flags, TR_FREE_ACT);
    }

    if (o_ptr->tval == TV_BOW && o_ptr->sval == SV_HARP)
    {
        o_ptr->to_h = 0;
        o_ptr->to_d = 0;
        remove_flag(o_ptr->art_flags, TR_SHOW_MODS);
    }

    /* Hack: Apply extra might at the end (so only once) */
    if (o_ptr->tval == TV_BOW && have_flag(o_ptr->art_flags, TR_XTRA_MIGHT) && o_ptr->mult)
    {
        o_ptr->mult  += 25 + m_bonus(17, object_level) * 5;
        remove_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
    }

    if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_POISON_NEEDLE))
    {
        o_ptr->to_h = 0;
        o_ptr->to_d = 0;
        remove_flag(o_ptr->art_flags, TR_BLOWS);
        remove_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
        remove_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
        remove_flag(o_ptr->art_flags, TR_SLAY_EVIL);
        remove_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
        remove_flag(o_ptr->art_flags, TR_SLAY_DEMON);
        remove_flag(o_ptr->art_flags, TR_SLAY_ORC);
        remove_flag(o_ptr->art_flags, TR_SLAY_TROLL);
        remove_flag(o_ptr->art_flags, TR_SLAY_GIANT);
        remove_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
        remove_flag(o_ptr->art_flags, TR_KILL_DRAGON);
        remove_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
        remove_flag(o_ptr->art_flags, TR_VORPAL);
        remove_flag(o_ptr->art_flags, TR_BRAND_POIS);
        remove_flag(o_ptr->art_flags, TR_BRAND_ACID);
        remove_flag(o_ptr->art_flags, TR_BRAND_ELEC);
        remove_flag(o_ptr->art_flags, TR_BRAND_FIRE);
        remove_flag(o_ptr->art_flags, TR_BRAND_COLD);
    }

    /* Hack: Lights won't value at all unless they are artifacts and the
       only way an object is marked as a rand-art is via the art_name field. */
    if (o_ptr->tval == TV_LITE)
        o_ptr->art_name = quark_add("Temp");

    if (object_is_melee_weapon(o_ptr))
        adjust_weapon_weight(o_ptr);

    total_flags = new_object_cost(o_ptr);
    if (cheat_peek) msg_format("Score: %d", total_flags);

    if (object_is_jewelry(o_ptr))
    {
        if (total_flags <= 0) power_level = 0;
        else if (total_flags < 15000) power_level = 1;
        else if (total_flags < 60000) power_level = 2;
        else power_level = 3;
    }
    else if (!object_is_weapon_ammo(o_ptr))
    {
        /* For armors */
        if (total_flags <= 0) power_level = 0;
        else if (total_flags < 30000) power_level = 1;
        else if (total_flags < 70000) power_level = 2;
        else 
        {
            power_level = 3;
            if (one_in_(17)) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            if (one_in_(5)) add_flag(o_ptr->art_flags, TR_HOLD_LIFE);
        }
    }
    else
    {
        /* For weapons */
        if (total_flags <= 0) power_level = 0;
        else if (total_flags <  50000) power_level = 1;
        else if (total_flags < 100000) power_level = 2;
        else 
        {
            power_level = 3;
            if (one_in_(17)) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            if (one_in_(5)) add_flag(o_ptr->art_flags, TR_FREE_ACT);
            if (one_in_(5)) add_flag(o_ptr->art_flags, TR_SEE_INVIS);
        }
    }

    if (mode & CREATE_ART_SCROLL)
    {
        char dummy_name[80] = "";
        cptr ask_msg = "What do you want to call the artifact? ";

        /* Identify it fully */
        object_aware(o_ptr);
        object_known(o_ptr);

        /* Mark the item as fully known */
        o_ptr->ident |= (IDENT_MENTAL);

        (void)screen_object(o_ptr, 0L);

        if (!get_string(ask_msg, dummy_name, sizeof dummy_name)
            || !dummy_name[0])
        {
            get_random_name(new_name, o_ptr, power_level);
        }
        else
            sprintf(new_name, "'%s'", dummy_name);

        virtue_add(VIRTUE_INDIVIDUALISM, 2);
        virtue_add(VIRTUE_ENCHANTMENT, 5);
    }
    else
    {
        get_random_name(new_name, o_ptr, power_level);
        if (a_cursed) /* Curse after naming for flavor. ?Art will never curse */
        {
            curse_object(o_ptr);
            if (has_pval && !o_ptr->pval)
                o_ptr->pval = randint1(5);
            total_flags = new_object_cost(o_ptr);
        }
    }

    if (cheat_xtra)
    {
        if (artifact_bias) msg_format("Biased artifact: %d.", artifact_bias);
        else msg_print("No bias in artifact.");
    }

    /* Save the inscription */
    o_ptr->art_name = quark_add(new_name);

    /* Window stuff */
    p_ptr->window |= (PW_INVEN | PW_EQUIP);

    return total_flags;
}

void get_bloody_moon_flags(object_type *o_ptr)
{
    int dummy, i;

    for (i = 0; i < TR_FLAG_SIZE; i++)
        o_ptr->art_flags[i] = a_info[ART_BLOOD].flags[i];

    dummy = randint1(2) + randint1(2);
    for (i = 0; i < dummy; i++)
    {
        int flag = randint0(26);
        if (flag >= 20) add_flag(o_ptr->art_flags, TR_KILL_UNDEAD + flag - 20);
        else if (flag == 19) add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
        else if (flag == 18) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
        else add_flag(o_ptr->art_flags, TR_CHAOTIC + flag);
    }

    dummy = randint1(2);
    for (i = 0; i < dummy; i++) one_resistance(o_ptr);

    for (i = 0; i < 2; i++)
    {
        int tmp = randint0(11);
        if (tmp < 6) add_flag(o_ptr->art_flags, TR_STR + tmp);
        else add_flag(o_ptr->art_flags, TR_STEALTH + tmp - 6);
    }
}


void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr)
{
    bool give_resistance = FALSE, give_power = FALSE;

    if (o_ptr->name1 == ART_GOTHMOG)
    {
        if (prace_is_(RACE_MON_DEMON))
        {
            o_ptr->dd = 6;
            o_ptr->ds = 6;
            o_ptr->to_h = 22;
            o_ptr->to_d = 25;
        }
        else
        {
            o_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE);
        }
    }

    if (o_ptr->name1 == ART_TWILIGHT)
    {
        if (giant_is_(GIANT_FIRE)) /* Boss reward for Fire Giants */
        {
            o_ptr->to_h = 10;
            o_ptr->to_d = 10;
            o_ptr->to_a = 0;
            add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
            give_resistance = TRUE;
        }
        else
        {
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            add_flag(o_ptr->art_flags, TR_TY_CURSE);
            o_ptr->curse_flags |=
                (TRC_CURSED | TRC_HEAVY_CURSE);
            o_ptr->curse_flags |= get_curse(2, o_ptr);
        }
    }

    if (o_ptr->name1 == ART_STORMBRINGER)
    {
        if (prace_is_(RACE_MON_SWORD))
        {
            add_flag(o_ptr->art_flags, TR_BLOWS); /* Just like the good 'ol days :) */
        }
        else
        {
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            add_flag(o_ptr->art_flags, TR_DRAIN_EXP);
            o_ptr->curse_flags |=
                (TRC_CURSED | TRC_HEAVY_CURSE);
        }
    }

    if (o_ptr->name1 == ART_DESTROYER)
    {
        if (prace_is_(RACE_MON_GOLEM))
        {
        }
        else
        {
            add_flag(o_ptr->art_flags, TR_BLOWS);
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            add_flag(o_ptr->art_flags, TR_DRAIN_EXP);
            add_flag(o_ptr->art_flags, TR_TY_CURSE);
            o_ptr->curse_flags |=
                (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE);
        }
    }

    if (o_ptr->name1 == ART_TERROR) /* Terror Mask is for warriors... */
    {
        if (p_ptr->pclass == CLASS_WARRIOR || 
            p_ptr->pclass == CLASS_CAVALRY || 
            p_ptr->pclass == CLASS_BERSERKER ||
            p_ptr->pclass == CLASS_MAULER )
        {
            give_power = TRUE;
            give_resistance = TRUE;
        }
        else
        {
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
            add_flag(o_ptr->art_flags, TR_TY_CURSE);
            o_ptr->curse_flags |=
                (TRC_CURSED | TRC_HEAVY_CURSE);
            o_ptr->curse_flags |= get_curse(2, o_ptr);
            return;
        }
    }
    if (o_ptr->name1 == ART_STONEMASK)
    {
        if (p_ptr->prace == RACE_MON_VAMPIRE)
        {
            add_flag(o_ptr->art_flags, TR_CHR);
            o_ptr->to_a = 20;
        }
        else
            add_flag(o_ptr->art_flags, TR_VULN_LITE);
    }

    if (o_ptr->name1 == ART_MURAMASA)
    {
        if (p_ptr->pclass != CLASS_SAMURAI && p_ptr->pclass != CLASS_BLOOD_KNIGHT)
        {
            add_flag(o_ptr->art_flags, TR_NO_MAGIC);
            o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
        }
    }

    if (o_ptr->name1 == ART_DR_JONES)
    {
        if (p_ptr->pclass == CLASS_ARCHAEOLOGIST)
        {
            give_power = TRUE;
            give_resistance = TRUE;
            o_ptr->pval += 2;
            add_flag(o_ptr->art_flags, TR_SEARCH);
            add_flag(o_ptr->art_flags, TR_FREE_ACT);
        }
    }

    if (o_ptr->name1 == ART_XIAOLONG)
    {
        if (p_ptr->pclass == CLASS_MONK)
            add_flag(o_ptr->art_flags, TR_BLOWS);
    }

    if (o_ptr->name1 == ART_BLOOD)
    {
        get_bloody_moon_flags(o_ptr);
    }

    if (o_ptr->name1 == ART_HEAVENLY_MAIDEN)
    {
        if (p_ptr->psex != SEX_FEMALE)
        {
            add_flag(o_ptr->art_flags, TR_AGGRAVATE);
        }
    }

    if (a_ptr->gen_flags & (TRG_XTRA_POWER)) give_power = TRUE;
    if (a_ptr->gen_flags & (TRG_XTRA_H_RES)) give_resistance = TRUE;
    if (a_ptr->gen_flags & (TRG_XTRA_RES_OR_POWER))
    {
        /* Give a resistance OR a power */
        if (one_in_(2)) give_resistance = TRUE;
        else give_power = TRUE;
    }

    if (give_power)
    {
        one_ability(o_ptr);
    }

    if (give_resistance)
    {
        one_high_resistance(o_ptr);
    }
}

bool reforge_artifact(object_type *src, object_type *dest, int fame)
{
    bool        result = FALSE;
    object_type forge = {0};    
    object_type best = {0}, worst = {0};
    int         base_power, best_power = -10000000, power = 0, worst_power = 10000000;
    int         min_power, max_power;
    int         old_level, i;

    /* Score the Original */
    base_power = object_value_real(src);

    /* Pay a Power Tax! */
    base_power = base_power/2 + randint1(base_power*fame/500);

    /* Setup thresholds. For weak objects, its better to use a generous range ... */
    if (base_power < 1000)
    {
        min_power = 0;
        max_power = base_power + 5000;
    }
    else
    {
        min_power = 7 * base_power / 10;
        max_power = 3 * base_power / 2;
    }

    /* Better Fame means better results! */
    old_level = object_level;
    object_level = MIN(fame/2, 75);

    for (i = 0; i < 1000 && !result; i++)
    {
        object_copy(&forge, dest);
        create_artifact(&forge, CREATE_ART_GOOD);
        power = object_value_real(&forge);

        if (power > best_power)
        {
            object_copy(&best, &forge);
            best_power = power;
        }
        if (power < worst_power && min_power < power)
        {
            object_copy(&worst, &forge);
            worst_power = power;
        }

        if (power >= min_power && power <= max_power)
        {
            object_copy(dest, &forge);
            result = TRUE;
        }
    }

    object_level = old_level;

    if (!result)
    {
        /* Failed! Return best or worst */
        if (worst_power > min_power)
            object_copy(dest, &worst);
        else
            object_copy(dest, &best);

        result = TRUE;
    }

    /* Flavor: Keep name of source artifact if possible */
    if (src->name1)
    {
        dest->name3 = src->name1;
        dest->art_name = quark_add(a_name + a_info[src->name1].name);
    }
    else
        dest->art_name = src->art_name;

    return result;
}

bool create_replacement_art(int a_idx, object_type *o_ptr)
{
    object_type        forge1 = {0};
    object_type        forge2 = {0};
    object_type        best = {0}, worst = {0};
    int                base_power, best_power, power = 0, worst_power = 10000000;
    int                old_level;
    artifact_type  *a_ptr = &a_info[a_idx];
    int                i;

    if (!a_ptr->name) return FALSE;
    if (no_artifacts) return FALSE;

    /* Score the Original */
    if (!create_named_art_aux(a_idx, &forge1)) return FALSE;
    if (object_is_weapon_ammo(&forge1))
    {
        forge1.to_h = MAX(10, forge1.to_h);
        forge1.to_d = MAX(10, forge1.to_d);
    }
    if (object_is_armour(&forge1))
    {
        forge1.to_a = MAX(10, forge1.to_a);
    }
    base_power = object_value_real(&forge1);
    
    best_power = -10000000;
    power = 0;
    old_level = object_level;

    if (object_level < a_ptr->level)
        object_level = a_ptr->level;

    for (i = 0; i < 10000; i++)
    {
        object_prep(&forge2, forge1.k_idx);
        create_artifact(&forge2, CREATE_ART_GOOD);
        power = object_value_real(&forge2);

        if (power > best_power)
        {
            object_copy(&best, &forge2);
            best_power = power;
        }
        if (power < worst_power)
        {
            object_copy(&worst, &forge2);
            worst_power = power;
        }

        if (power > base_power * 7 / 10 && power < base_power * 2)
        {
            object_level = old_level;
            object_copy(o_ptr, &forge2);
            o_ptr->name3 = a_idx;
            o_ptr->weight = forge1.weight;
            return TRUE;
        }
    }

    /* Failed! Return best or worst */
    object_level = old_level;
    if (worst_power > base_power)
        object_copy(o_ptr, &worst);
    else
        object_copy(o_ptr, &best);

    o_ptr->name3 = a_idx;
    o_ptr->weight = forge1.weight;

    return TRUE;
}

/*
 * Create the artifact of the specified number
 */
bool create_named_art_aux(int a_idx, object_type *o_ptr)
{
    int k_idx;
    artifact_type *a_ptr = &a_info[a_idx];

    if (!a_ptr->name) return FALSE;
    k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
    if (!k_idx) return FALSE;

    object_prep(o_ptr, k_idx);

    o_ptr->name1 = a_idx;
    o_ptr->pval = a_ptr->pval;
    o_ptr->ac = a_ptr->ac;
    o_ptr->dd = a_ptr->dd;
    o_ptr->ds = a_ptr->ds;
    o_ptr->mult = a_ptr->mult;
    o_ptr->to_a = a_ptr->to_a;
    o_ptr->to_h = a_ptr->to_h;
    o_ptr->to_d = a_ptr->to_d;
    o_ptr->weight = a_ptr->weight;

    if (a_ptr->gen_flags & TRG_CURSED) o_ptr->curse_flags |= (TRC_CURSED);
    if (a_ptr->gen_flags & TRG_HEAVY_CURSE) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
    if (a_ptr->gen_flags & TRG_PERMA_CURSE) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
    if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
    if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
    if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

    random_artifact_resistance(o_ptr, a_ptr);

    return TRUE;
}

bool create_named_art(int a_idx, int y, int x)
{
    if (no_artifacts) return FALSE;

    if (random_artifacts)
    {
        object_type forge;
        if (create_replacement_art(a_idx, &forge))
            return drop_near(&forge, -1, y, x) ? TRUE : FALSE;
    }
    else
    {
        object_type forge;
        if (create_named_art_aux(a_idx, &forge))
            return drop_near(&forge, -1, y, x) ? TRUE : FALSE;
    }
    return FALSE;
}
