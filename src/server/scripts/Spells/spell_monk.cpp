#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

class spell_monk_disable : public SpellScriptLoader
{
public:
    spell_monk_disable() : SpellScriptLoader("spell_monk_disable") { }

    class spell_monk_disable_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_disable_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void PeriodicDummy(AuraEffect const* aurEff)
        {
            if(GetUnitOwner() && GetCaster())
            {
                if(GetUnitOwner()->IsWithinDist(GetCaster(), 10.f, true))
                    aurEff->GetBase()->RefreshDuration();
            }
        }


        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_disable_AuraScript::PeriodicDummy, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_monk_disable_AuraScript();
    }

    class spell_monk_disable_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_disable_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void Hit()
        {
            if(GetExplTargetUnit() && GetExplTargetUnit()->HasAura(116095))
            {
                GetExplTargetUnit()->RemoveAura(116095);
                PreventHitAura();
                GetCaster()->CastSpell(GetExplTargetUnit(), 116706, true);
            }

            //Cant apply twice
            if(GetExplTargetUnit() && GetExplTargetUnit()->HasAura(116706))
                PreventHitAura();
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_monk_disable_SpellScript::Hit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_disable_SpellScript();
    }
};


void AddSC_monk_spell_scripts()
{
    new spell_monk_disable();
}