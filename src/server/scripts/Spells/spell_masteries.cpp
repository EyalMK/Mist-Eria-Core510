#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"

enum MasterySpells
{
    MASTERY_WARRIOR_ARMS                = 76838,
    MASTERY_WARRIOR_FURY                = 76856,
    MASTERY_WARRIOR_PROTECTION          = 76857,

    MASTERY_PALADIN_HOLY                = 76669,
    MASTERY_PALADIN_PROTECTION          = 76671,
    MASTERY_PALADIN_RETRIBUTION         = 76672,
    
    MASTERY_HUNTER_BEASTMASTER          = 76657,
    MASTERY_HUNTER_MARKSMAN             = 76659,
    MASTERY_HUNTER_SURVIVAL             = 76658,

    MASTERY_ROGUE_ASSASSINATION         = 76803,
    MASTERY_ROGUE_COMBAT                = 76806,
    MASTERY_ROGUE_SUBTLETY              = 76808,

    MASTERY_PRIEST_DISCIPLINE           = 77484,
    MASTERY_PRIEST_HOLY                 = 77485,
    MASTERY_PRIEST_SHADOW               = 77486,

    MASTERY_DEATHKNIGHT_BLOOD           = 77513,
    MASTERY_DEATHKNIGHT_FROST           = 77514,
    MASTERY_DEATHKNIGHT_UNHOLY          = 77515,

    MASTERY_SHAMAN_ELEMENTAL            = 77222,
    MASTERY_SHAMAN_ENHANCEMENT          = 77223,
    MASTERY_SHAMAN_RESTORATION          = 77226,

    MASTERY_MAGE_ARCANE                 = 76547,
    MASTERY_MAGE_FIRE                   = 12846,
    MASTERY_MAGE_FROST                  = 76613,

    MASTERY_WARLOCK_AFFLICTION          = 77215,
    MASTERY_WARLOCK_DEMONOLOGY          = 77219,
    MASTERY_WARLOCK_DESTRUCTION         = 77220,

    MASTERY_MONK_BREWMASTER             = 117906,
    MASTERY_MONK_MISTWEAVER             = 117907,
    MASTERY_MONK_WINDWALKER             = 115636,

    MASTERY_DRUID_BALANCE               = 77492,
    MASTERY_DRUID_FERAL                 = 77493,
    MASTERY_DRUID_GUARDIAN              = 77494,
    MASTERY_DRUID_RESTORATION           = 77495
};

enum WarriorSpells
{
	SPELL_WARR_ENRAGE		= 12880,
};

// Warrior spell : Enrage 12880
class spell_mastery_unshackled_fury : public SpellScriptLoader
{
    public:
        spell_mastery_unshackled_fury() : SpellScriptLoader("spell_mastery_unshackled_fury") { }

        class spell_mastery_unshackled_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_unshackled_fury_SpellScript);

            void HandleOnHit()
            {
				Player* player = GetCaster()->ToPlayer();
				float Mastery = (player->GetFloatValue(PLAYER_MASTERY) + player->GetRatingBonusValue(CR_MASTERY));
				int32 bp = int32(Mastery);

				if (player->HasAura(MASTERY_WARRIOR_FURY) && player->getLevel() >= 80)
					player->CastCustomSpell(player, SPELL_WARR_ENRAGE, &bp, NULL, NULL, true);
				else player->CastSpell(player, SPELL_WARR_ENRAGE, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_mastery_unshackled_fury_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mastery_unshackled_fury_SpellScript();
        }
};

void AddSC_masteries_spell_scripts()
{
	new spell_mastery_unshackled_fury();
}