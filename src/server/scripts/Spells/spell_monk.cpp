#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

// 116095 - Disable
class spell_monk_disable : public SpellScriptLoader
{
public:
    spell_monk_disable() : SpellScriptLoader("spell_monk_disable") { }

    class spell_monk_disable_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_disable_AuraScript);

        void PeriodicDummy(AuraEffect const* aurEff)
        {
            if(GetUnitOwner() && GetCaster())
            {
                if(GetUnitOwner()->IsWithinDist(GetCaster(), 10.f, true))
                    aurEff->GetBase()->RefreshDuration();
            }
        }

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if(aurEff->GetBase()->GetCharges())
            {
                GetCaster()->CastSpell(GetUnitOwner(), 116706, true);
            }
        }


        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_disable_AuraScript::PeriodicDummy, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            AfterEffectApply += AuraEffectApplyFn(spell_monk_disable_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_monk_disable_AuraScript();
    }
    
    class spell_monk_disable_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_disable_SpellScript);

        void Hit()
        {
            if (Unit* target = GetExplTargetUnit())
            {
                //Cant apply twice
                if(target->HasAura(116706))
                {
                    PreventHitAura();
                    return;
                }

                if(target->HasAura(116095))
                {
                    GetCaster()->CastSpell(target, 116706, true);
                    target->RemoveAura(116095);
                    PreventHitAura();
                }
                
            }
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