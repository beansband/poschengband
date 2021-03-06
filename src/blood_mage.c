#include "angband.h"


static void _blood_rite_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Blood Rite");
        break;
    case SPELL_DESC:
        var_set_string(res, "Temporarily increase the cost and effectiveness of your spells.");
        break;
    case SPELL_CAST:
        var_set_bool(res, FALSE);
        if (p_ptr->tim_blood_rite)
        {
            msg_print("The Blood Rite is already active.");
            return;
        }
        set_tim_blood_rite(10, FALSE);
        var_set_bool(res, TRUE);
        break;
    default:
        default_spell(cmd, res);
        break;
    }
}

static int _get_powers(spell_info* spells, int max)
{
    int ct = 0;

    spell_info* spell = &spells[ct++];
    spell->level = 30;
    spell->cost = 100;
    spell->fail = calculate_fail_rate(spell->level, 30, p_ptr->stat_ind[A_INT]);
    spell->fn = _blood_rite_spell;

    return ct;
}

static void _calc_bonuses(void)
{
    p_ptr->regenerate = TRUE;
}

static void _on_cast(const spell_info *spell)
{
    int cut = spell->level - p_ptr->lev/2;
    if (cut > 0 && one_in_(13))
        set_cut(p_ptr->cut + cut, FALSE);
}

static caster_info * _caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "blood spell";
        me.which_stat = A_INT;
        me.weight = 430;
        me.options = CASTER_USE_HP | CASTER_ALLOW_DEC_MANA | CASTER_GLOVE_ENCUMBRANCE;
        me.on_cast = _on_cast;
        init = TRUE;
    }
    return &me;
}

class_t *blood_mage_get_class_t(void)
{
    static class_t me = {0};
    static bool init = FALSE;

    if (!init)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 30,  40,  38,   3,  16,  20,  34,  20};
    skills_t xs = {  7,  15,  11,   0,   0,   0,   6,   7};

        me.name = "Blood-Mage";
        me.desc = "A Blood Mage is similar to a normal mage in his selection and "
                    "variety of spells, but differs in that he has no separate "
                    "mana pool.  Instead, all his spells are powered by "
                    "his health.  Moreover, due to the Blood Mage's abnormal constitution, "
                    "all healing is much less effective than normal. In fact, the Blood "
                    "Mage completely eschews all healing magic as this disrupts the flow "
                    "of blood that is the essence of their power. They completely shun "
                    "the realm of Life as anathema to all that is sacred.";

        me.stats[A_STR] = -4;
        me.stats[A_INT] =  3;
        me.stats[A_WIS] = -2;
        me.stats[A_DEX] =  1;
        me.stats[A_CON] =  2;
        me.stats[A_CHR] = -2;
        me.base_skills = bs;
        me.extra_skills = xs;
        me.life = 108;
        me.base_hp = 10;
        me.exp = 135;
        me.pets = 30;

        me.calc_bonuses = _calc_bonuses;
        me.caster_info = _caster_info;
        /* TODO: This class uses spell books, so we are SOL
        me.get_spells = _get_spells;*/
        me.get_powers = _get_powers;
        me.character_dump = spellbook_character_dump;
        init = TRUE;
    }

    return &me;
}

