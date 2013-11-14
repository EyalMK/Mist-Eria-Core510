#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "MapManager.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "Spell.h"

enum MonkSpells
{
    SPELL_MONK_TOUCH_DEATH					= 115080,
	SPELL_MONK_LEGACY_OF_THE_EMPEROR		= 117666,
	SPELL_MONK_LEGACY_OF_THE_WHITE_TIGER	= 116781,
	SPELL_MONK_TOUCH_OF_DEATH				= 115080,
};

//119611 - Renewing mist
class spell_monk_renewing_mist : public SpellScriptLoader
{
public:
    spell_monk_renewing_mist() : SpellScriptLoader("spell_monk_renewing_mist") { }


    class spell_monk_renewing_mist_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_renewing_mist_AuraScript);

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            GetCaster()->CastSpell(GetCaster(), 123757); //aura for spell elevation
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_renewing_mist_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_monk_renewing_mist_AuraScript();
    }
};

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

        void Cast()
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
                    AuraEffect* tickEffect = target->GetAuraEffect(116095, EFFECT_1, GetCaster()->GetGUID());
                    if(tickEffect && tickEffect->GetTickNumber() > 1)
                    {
                        GetCaster()->CastSpell(target, 116706, true);
                        target->RemoveAura(116095);
                    }
                    PreventHitAura();
                }
                
            }
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_monk_disable_SpellScript::Cast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_disable_SpellScript();
    }
    
};

class spell_monk_paralysis : public SpellScriptLoader
{
public:
    spell_monk_paralysis() : SpellScriptLoader("spell_monk_paralysis") { }


    class spell_monk_paralysis_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_monk_paralysis_AuraScript);

        void OnAmountCalc(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if(GetCaster() && GetTarget() && GetCaster()->isInBack(GetTarget()))
            {
                SetMaxDuration(GetMaxDuration()*2);
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_monk_paralysis_AuraScript::OnAmountCalc, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_monk_paralysis_AuraScript();
    }
};

// 119996 - Transcendence Transfert
class spell_monk_transcendence_transfert : public SpellScriptLoader
{
public:
    spell_monk_transcendence_transfert() : SpellScriptLoader("spell_monk_transcendence_transfert") { }

    class spell_monk_transcendence_transfert_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_transcendence_transfert_SpellScript);

        SpellCastResult CheckCast()
        {
            if(GetCaster() && GetCaster()->ToPlayer())
            {
                if(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()) != NULL && GetCaster()->ToPlayer()->GetExactDist2d(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionX(), GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionY()) <= 40.0f)
                    return SPELL_CAST_OK;
                else if(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()) == NULL)
                    return SPELL_FAILED_NO_PET;
                else if(GetCaster()->ToPlayer()->GetExactDist2d(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionX(), GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionY()) <= 40.0f)
                    return SPELL_FAILED_OUT_OF_RANGE;

            }
        }

        void Cast()
        {
            if(GetCaster() && GetCaster()->ToPlayer())
            {
                if(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()) != NULL && GetCaster()->ToPlayer()->GetExactDist2d(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionX(), GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->GetPositionY()) <= 40.0f)
                {
                    float petX, petY, petZ, casterX, casterY, casterZ;
                    uint32 petMapId;
                    uint32 displayIdCaster;
                    (GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()))->GetPosition(petX, petY, petZ);
                    petMapId = (GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()))->GetMapId();
                    GetCaster()->GetPosition(casterX, casterY, casterZ);

                    (GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()))->FarTeleportTo(GetCaster()->GetMap(), casterX, casterY, casterZ, 0.0f);

                    if(GetCaster()->ToPlayer())
                        GetCaster()->ToPlayer()->TeleportTo(petMapId, petX, petY, petZ, 0.0f);

                    GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->DespawnOrUnsummon();
                    GetCaster()->ToPlayer()->SetTranscendenceSpirit(GetCaster()->SummonCreature(54569, casterX, casterY, casterZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 900*IN_MILLISECONDS));
                    displayIdCaster = GetCaster()->GetDisplayId();
                    GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->SetDisplayId(displayIdCaster);
                    GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->CastSpell(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()), 124416);
                    GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->CastSpell(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()), 119053);

                }
            }

        }

        void Register()
        {
            OnCast += SpellCastFn(spell_monk_transcendence_transfert_SpellScript::Cast);
            OnCheckCast += SpellCheckCastFn(spell_monk_transcendence_transfert_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_transcendence_transfert_SpellScript();
    }
};

// 101643 Transcendence
class spell_monk_transcendence : public SpellScriptLoader
{
public:
    spell_monk_transcendence() : SpellScriptLoader("spell_monk_transcendence") { }


    class spell_monk_transcendence_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_transcendence_SpellScript);

        void Cast()
        {
            if(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()))
            {
                GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->DespawnOrUnsummon();
                GetCaster()->ToPlayer()->SetTranscendenceSpirit(NULL);
            }

            float x, y, z;
            uint32 displayIdCaster;
            GetCaster()->GetPosition(x,y,z);
            GetCaster()->ToPlayer()->SetTranscendenceSpirit(GetCaster()->SummonCreature(54569, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 900*IN_MILLISECONDS));
            displayIdCaster = GetCaster()->GetDisplayId();
            GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->SetDisplayId(displayIdCaster);
            GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->CastSpell(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()), 124416);
            GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster())->CastSpell(GetCaster()->ToPlayer()->GetTranscendenceSpirit(GetCaster()), 119053);
        }


        void Register()
        {
            OnCast += SpellCastFn(spell_monk_transcendence_SpellScript::Cast);
        }

    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_transcendence_SpellScript();
    }
};

// 54569 - Trasncendence Spirit
class npc_transcendence_spirit : public CreatureScript 
{
public:
    npc_transcendence_spirit() : CreatureScript("npc_transcendence_spirit") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_transcendence_spiritAI(creature);
    }

    struct npc_transcendence_spiritAI : public ScriptedAI
    {
        npc_transcendence_spiritAI(Creature *creature) : ScriptedAI(creature)
        {
        }

        EventMap events;
        uint32 secondTimer;

        void Reset()
        {
            secondTimer = 900000;
        }

        void JustDied(Unit *pWho)
        {
            me->GetOwner()->ToPlayer()->SetTranscendenceSpirit(NULL);
        }

        void CorpseRemoved(uint32& /*respawnDelay*/)
        {
            me->GetOwner()->ToPlayer()->SetTranscendenceSpirit(NULL);
        }

        void EnterCombat(Unit* /*who*/)
        {

        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);

            if(secondTimer <= diff)
            {
                me->GetOwner()->ToPlayer()->SetTranscendenceSpirit(NULL);
                me->DespawnOrUnsummon();
            } else secondTimer -= diff;
        }
    };
};

// 116670 - Elevation
class spell_monk_elevation : public SpellScriptLoader
{
public:
    spell_monk_elevation() : SpellScriptLoader("spell_monk_elevation") { }


    class spell_monk_elevation_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monk_elevation_SpellScript);

        void HandleEffect(SpellEffIndex /*effIndex*/)
        {
            if(Player* p = GetCaster()->ToPlayer())
            {
                SetHitDamage(p->GetSpellDamage(1, urand(108, 124), 90, urand(7210, 8379), 0.f, 68.f));
                SetHitHeal(p->GetSpellDamage(1, urand(108, 124), 90, urand(7210, 8379), 0.f, 68.f));
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_monk_elevation_SpellScript::HandleEffect, EFFECT_1, SPELL_EFFECT_HEAL);
        }

    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_elevation_SpellScript();
    }
};

// Touch of death : 115080
class spell_monk_touch_of_death : public SpellScriptLoader
{
    public:
        spell_monk_touch_of_death() : SpellScriptLoader("spell_monk_touch_of_death") { }

        class spell_monk_touch_of_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_touch_of_death_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MONK_TOUCH_OF_DEATH))
                    return false;
                return true;
            }

			SpellCastResult CheckCast()
            {
				if (GetHitUnit()->GetTypeId() == TYPEID_PLAYER)
				{
					if (GetHitUnit()->GetHealth() > GetHitUnit()->GetMaxHealth()*0.10f)
						return SPELL_FAILED_ERROR;
				}

				if (GetHitUnit()->GetTypeId() != TYPEID_PLAYER && GetCaster()->GetHealth() < GetHitUnit()->GetHealth())
					return SPELL_FAILED_ERROR;

				if (GetCaster() == GetHitUnit())
					return SPELL_FAILED_NO_VALID_TARGETS;

				return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster();
				Unit* target = GetHitUnit();
				uint32 damage = 0;

				if (caster->GetTypeId() == TYPEID_PLAYER && target->GetHealth() <= target->GetMaxHealth()*0.10f)
				{
					damage = GetHitUnit()->GetHealth();
					SetHitDamage(damage);
				}

				if (target->GetTypeId() != TYPEID_PLAYER && caster->GetHealth() >= target->GetHealth())
				{
					damage = GetHitUnit()->GetHealth();
					SetHitDamage(damage);
				}
			}

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_monk_touch_of_death_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
				OnCheckCast += SpellCheckCastFn(spell_monk_touch_of_death_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_monk_touch_of_death_SpellScript();
        }
};

class spell_monk_legacy_of_the_white_tiger : public SpellScriptLoader
{
    public:
        spell_monk_legacy_of_the_white_tiger() : SpellScriptLoader("spell_monk_legacy_of_the_white_tiger") { }

        class spell_monk_legacy_of_the_white_tiger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_legacy_of_the_white_tiger_SpellScript);

            void Cast()
            {
                Unit* caster = GetCaster()->ToPlayer();

				if (caster->HasAura(SPELL_MONK_LEGACY_OF_THE_EMPEROR))
					caster->RemoveAura(SPELL_MONK_LEGACY_OF_THE_EMPEROR);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_monk_legacy_of_the_white_tiger_SpellScript::Cast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_monk_legacy_of_the_white_tiger_SpellScript();
        }
};

void AddSC_monk_spell_scripts()
{
    new spell_monk_disable();
    new spell_monk_paralysis();
    new spell_monk_transcendence_transfert();
    new spell_monk_transcendence();
    new npc_transcendence_spirit();
    new spell_monk_renewing_mist();
    new spell_monk_elevation();
    new spell_monk_touch_of_death();
	new spell_monk_legacy_of_the_white_tiger();
}
