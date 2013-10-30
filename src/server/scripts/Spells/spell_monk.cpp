#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "MapManager.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"

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

// 119996 - transcendence_transfert
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
						return SPELL_FAILED_OUT_OF_RANGE; //Not OK

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
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_monk_transcendence_transfert_SpellScript();
    }
};

// 101643 -- transcendence
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

// 54569 -- trasncendence_spirit
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


void AddSC_monk_spell_scripts()
{
    new spell_monk_disable();
    new spell_monk_paralysis();
    new spell_monk_transcendence_transfert();
	new spell_monk_transcendence();
	new npc_transcendence_spirit();
}