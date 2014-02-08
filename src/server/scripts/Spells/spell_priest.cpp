/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "ScriptedCreature.h"

enum PriestSpells
{
    SPELL_PRIEST_DIVINE_AEGIS                       = 47753,
    SPELL_PRIEST_EMPOWERED_RENEW                    = 63544,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL                 = 55673,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL    = 56161,
    SPELL_PRIEST_GLYPH_OF_SHADOW                    = 107906,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL               = 48153,
    SPELL_PRIEST_LEAP_OF_FAITH                      = 73325,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT               = 92832,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT_TRIGGER       = 92833,
    SPELL_PRIEST_LEAP_OF_FAITH_TRIGGERED            = 92572,
    SPELL_PRIEST_MANA_LEECH_PROC                    = 34650,
    SPELL_PRIEST_PENANCE_R1                         = 47540,
    SPELL_PRIEST_PENANCE_R1_DAMAGE                  = 47758,
    SPELL_PRIEST_PENANCE_R1_HEAL                    = 47757,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1               = 33201,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED        = 33619,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH    = 107903,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH       = 107904,
    SPELL_PRIEST_T9_HEALING_2P                      = 67201,
    SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL              = 64085,
	SPELL_PRIEST_RENEW                              = 139,
	SPELL_PRIEST_VOID_SHIFT                         = 108968,
	
	// Mind spike
	SPELL_PRIEST_MIND_SPIKE							= 73510,
	
	// Shadow Word : Death
    SPELL_PRIEST_SHADOW_WORD_DEATH_BASE            = 32379, // This one is the spell in the spellbook
    SPELL_PRIEST_SHADOW_WORD_DEATH_GLYPH            = 120583, // WARNING : This one overrides the caster's spell's bar !
    SPELL_PRIEST_SHADOW_WORD_DEATH_OVERRIDED        = 129176, // This one is the spell that overrides 32379
    SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE         = 125927, // This one is the spell that add a shadow orb
    SPELL_PRIEST_SHADOW_WORD_DEATH_DAMAGES          = 32409, // If target doesn't die, we damage the caster
};

enum PriestSpellIcons
{
    PRIEST_ICON_ID_BORROWED_TIME                    = 2899,
    PRIEST_ICON_ID_EMPOWERED_RENEW_TALENT           = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING               = 2874,
};

// -47509 - Divine Aegis
class spell_pri_divine_aegis : public SpellScriptLoader
{
    public:
        spell_pri_divine_aegis() : SpellScriptLoader("spell_pri_divine_aegis") { }

        class spell_pri_divine_aegis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_divine_aegis_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_DIVINE_AEGIS))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                int32 absorb = CalculatePct(int32(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount());

                // Multiple effects stack, so let's try to find this aura.
                if (AuraEffect const* aegis = eventInfo.GetProcTarget()->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS, EFFECT_0))
                    absorb += aegis->GetAmount();

                absorb = std::min(absorb, eventInfo.GetProcTarget()->getLevel() * 125);

                GetTarget()->CastCustomSpell(SPELL_PRIEST_DIVINE_AEGIS, SPELLVALUE_BASE_POINT0, absorb, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_divine_aegis_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_divine_aegis_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_divine_aegis_AuraScript();
        }
};

// 55680 - Glyph of Prayer of Healing
class spell_pri_glyph_of_prayer_of_healing : public SpellScriptLoader
{
    public:
        spell_pri_glyph_of_prayer_of_healing() : SpellScriptLoader("spell_pri_glyph_of_prayer_of_healing") { }

        class spell_pri_glyph_of_prayer_of_healing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_glyph_of_prayer_of_healing_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL);
                int32 heal = int32(CalculatePct(int32(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount()) / triggeredSpellInfo->GetMaxTicks());
                GetTarget()->CastCustomSpell(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL, SPELLVALUE_BASE_POINT0, heal, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_glyph_of_prayer_of_healing_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_glyph_of_prayer_of_healing_AuraScript();
        }
};

// 47788 - Guardian Spirit
class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL))
                    return false;
                return true;
            }

            bool Load()
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
                // remove the aura now, we don't want 40% healing bonus
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_1);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

// 92833 - Leap of Faith
class spell_pri_leap_of_faith_effect_trigger : public SpellScriptLoader
{
    public:
        spell_pri_leap_of_faith_effect_trigger() : SpellScriptLoader("spell_pri_leap_of_faith_effect_trigger") { }

        class spell_pri_leap_of_faith_effect_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_leap_of_faith_effect_trigger_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_LEAP_OF_FAITH_EFFECT))
                    return false;
                return true;
            }

            void HandleEffectDummy(SpellEffIndex /*effIndex*/)
            {
                Position destPos;
                GetHitDest()->GetPosition(&destPos);

                SpellCastTargets targets;
                targets.SetDst(destPos);
                targets.SetUnitTarget(GetCaster());
                GetHitUnit()->CastSpell(targets, sSpellMgr->GetSpellInfo(GetEffectValue()), NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_leap_of_faith_effect_trigger_SpellScript::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_leap_of_faith_effect_trigger_SpellScript();
        }
};

// -7001 - Lightwell Renew
class spell_pri_lightwell_renew : public SpellScriptLoader
{
    public:
        spell_pri_lightwell_renew() : SpellScriptLoader("spell_pri_lightwell_renew") { }

        class spell_pri_lightwell_renew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lightwell_renew_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // Bonus from Glyph of Lightwell
                    if (AuraEffect* modHealing = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_LIGHTWELL, EFFECT_0))
                        AddPct(amount, modHealing->GetAmount());
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_lightwell_renew_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_lightwell_renew_AuraScript();
        }
};

// 8129 - Mana Burn
class spell_pri_mana_burn : public SpellScriptLoader
{
    public:
        spell_pri_mana_burn() : SpellScriptLoader("spell_pri_mana_burn") { }

        class spell_pri_mana_burn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mana_burn_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* unitTarget = GetHitUnit())
                    unitTarget->RemoveAurasWithMechanic((1 << MECHANIC_FEAR) | (1 << MECHANIC_POLYMORPH));
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pri_mana_burn_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mana_burn_SpellScript;
        }
};

// 28305 - Mana Leech (Passive) (Priest Pet Aura)
class spell_pri_mana_leech : public SpellScriptLoader
{
    public:
        spell_pri_mana_leech() : SpellScriptLoader("spell_pri_mana_leech") { }

        class spell_pri_mana_leech_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mana_leech_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_MANA_LEECH_PROC))
                    return false;
                return true;
            }

            bool Load()
            {
                _procTarget = NULL;
                return true;
            }

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                _procTarget = GetTarget()->GetOwner();
                return _procTarget;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(_procTarget, SPELL_PRIEST_MANA_LEECH_PROC, true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_mana_leech_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_mana_leech_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:
            Unit* _procTarget;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_mana_leech_AuraScript();
        }
};

// 49821 - Mind Sear
class spell_pri_mind_sear : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") { }

        class spell_pri_mind_sear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_sear_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove_if(Trinity::ObjectGUIDCheck(GetCaster()->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT)));
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mind_sear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_sear_SpellScript();
        }
};

// 47948 - Pain and Suffering (Proc)
class spell_pri_pain_and_suffering_proc : public SpellScriptLoader
{
    public:
        spell_pri_pain_and_suffering_proc() : SpellScriptLoader("spell_pri_pain_and_suffering_proc") { }

        class spell_pri_pain_and_suffering_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_pain_and_suffering_proc_SpellScript);

            void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
            {
                // Refresh Shadow Word: Pain on target
                if (Unit* unitTarget = GetHitUnit())
                    if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0, 0, GetCaster()->GetGUID()))
                        aur->GetBase()->RefreshDuration();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_pain_and_suffering_proc_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_pain_and_suffering_proc_SpellScript;
        }
};

// 47540 - Penance
class spell_pri_penance : public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* spellInfo)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_PENANCE_R1))
                    return false;
                // can't use other spell than this penance due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(SPELL_PRIEST_PENANCE_R1) != sSpellMgr->GetFirstSpellInChain(spellInfo->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellInfo->Id);
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank, true))
                    return false;
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    if (!unitTarget->isAlive())
                        return;

                    uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);

                    if (caster->IsFriendlyTo(unitTarget))
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank), false, 0);
                    else
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank), false, 0);
                }
            }

            SpellCastResult CheckCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Unit* target = GetExplTargetUnit())
                    if (!caster->IsFriendlyTo(target) && !caster->IsValidAttackTarget(target))
                        return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_penance_SpellScript;
        }
};

// 17 - Power Word: Shield
class spell_pri_power_word_shield : public SpellScriptLoader
{
    public:
        spell_pri_power_word_shield() : SpellScriptLoader("spell_pri_power_word_shield") { }

        class spell_pri_power_word_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_word_shield_AuraScript);

            /*bool Validate(SpellInfo const* spellInfo)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_R1))
                    return false;

				if (GetTarget()->HasAura(6788)) // Weakened Soul
					return false;

                return true;
            }*/

			bool Validate(const SpellInfo* spellInfo)
			{
				return true ;
			}
			
            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield Aura : Entering DoEffectCalcAmount Handler");
                canBeRecalculated = false;
                if (Unit* caster = GetCaster())
                {
                    // +80.68% from sp bonus
                    float bonus = 0.8068f;

                    // Borrowed Time
                    if (AuraEffect const* borrowedTime = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_BORROWED_TIME, EFFECT_1))
                        bonus += CalculatePct(1.0f, borrowedTime->GetAmount());

                    bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());

                    // Improved PW: Shield: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
                    // Improved PW: Shield is only applied at the spell healing bonus because it was already applied to the base value in CalculateSpellDamage
                    bonus = caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), bonus);
                    bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

                    amount += int32(bonus);

                    // Twin Disciplines
                    if (AuraEffect const* twinDisciplines = caster->GetAuraEffect(SPELL_AURA_ADD_PCT_MODIFIER, SPELLFAMILY_PRIEST, 0x400000, 0, 0, GetCasterGUID()))
                        AddPct(amount, twinDisciplines->GetAmount());

                    // Focused Power
                    amount *= caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_DONE_PERCENT);


					/*GetTarget()->CastSpell(GetTarget(), 6788, true);*/	
                }
            }

            void ReflectDamage(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield Aura : Entering AfterEffectAbsorb Handler");
                Unit* target = GetTarget();
                if (dmgInfo.GetAttacker() == target)
                    return;

                if (Unit* caster = GetCaster())
                    if (AuraEffect* talentAurEff = caster->GetAuraEffectOfRankedSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_R1, EFFECT_0))
                    {
                        int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
                        target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
                    }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_power_word_shield_AuraScript::ReflectDamage, EFFECT_0);
				/*OnEffectApply += AuraEffectApplyFn(spell_pri_power_word_shield_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);*/
            }
        };

		class spell_pri_power_word_shield_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_pri_power_word_shield_SpellScript);

			bool Validate(const SpellInfo* spellInfo)
			{
				return true ;
			}

			bool Load()
			{
				return true ;
			}

			/*void handleApplyDebuffOnEffectApplyAuraHitTarget(SpellEffIndex effectIndex)
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering OnEffectHitTarget Handler");
				if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}
			}

			void handleOnEffectHit(SpellEffIndex effectIndex)
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering OnEffectHit Handler");
				if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}
			}*/

			void handleBeforeHitPhase()
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering BeforeHit Handler");
				/*if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}*/
			}

			void handleOnHitPhase()
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering OnHit Handler");
				/*if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}*/
			}

			void handleAfterHitPhase()
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering AfterHit Handler");
				if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}
			}

			/*void handleOnCastPhase()
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering OnCast Handler");
				if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}
			}

			void handleAfterCast()
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : Entering AfterCast Handler");
				if(GetHitUnit())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PW Shield : GetHitUnit() not null, applying debuff");
					GetHitUnit()->CastSpell(GetHitUnit(), 6788, TRIGGERED_FULL_MASK);
				}
			}*/

			void Register()
			{
				/*OnCast += SpellCastFn(spell_pri_power_word_shield_SpellScript::handleOnCastPhase);
				AfterCast += SpellCastFn(spell_pri_power_word_shield_SpellScript::handleAfterCast);

				OnEffectHit += SpellEffectFn(spell_pri_power_word_shield_SpellScript::handleOnEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
				OnEffectHitTarget += SpellEffectFn(spell_pri_power_word_shield_SpellScript::handleApplyDebuffOnEffectApplyAuraHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA) ;*/

				BeforeHit += SpellHitFn(spell_pri_power_word_shield_SpellScript::handleBeforeHitPhase);
				OnHit += SpellHitFn(spell_pri_power_word_shield_SpellScript::handleOnHitPhase);
				AfterHit += SpellHitFn(spell_pri_power_word_shield_SpellScript::handleAfterHitPhase);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_pri_power_word_shield_SpellScript();
		}

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_power_word_shield_AuraScript();
        }
};

// 33110 - Prayer of Mending Heal
class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

        class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetOriginalCaster())
                {
                    if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_PRIEST_T9_HEALING_2P, EFFECT_0))
                    {
                        int32 heal = GetHitHeal();
                        AddPct(heal, aurEff->GetAmount());
                        SetHitHeal(heal);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_prayer_of_mending_heal_SpellScript();
        }
};

// 17 - Reflective Shield
class spell_pri_reflective_shield_trigger : public SpellScriptLoader
{
    public:
        spell_pri_reflective_shield_trigger() : SpellScriptLoader("spell_pri_reflective_shield_trigger") { }

        class spell_pri_reflective_shield_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_reflective_shield_trigger_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_R1))
                    return false;
                return true;
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetAttacker() == target)
                    return;

                if (GetCaster())
                    if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_R1, EFFECT_0))
                    {
                        int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
                        target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
                    }
            }

            void Register()
            {
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_reflective_shield_trigger_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_reflective_shield_trigger_AuraScript();
        }
};

// 139 - Renew
class spell_pri_renew : public SpellScriptLoader
{
    public:
        spell_pri_renew() : SpellScriptLoader("spell_pri_renew") { }

        class spell_pri_renew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_renew_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // Empowered Renew
                    if (AuraEffect const* empoweredRenewAurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_EMPOWERED_RENEW_TALENT, EFFECT_1))
                    {
                        uint32 heal = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), GetEffect(EFFECT_0)->GetAmount(), DOT);
                        heal = GetTarget()->SpellHealingBonusTaken(caster, GetSpellInfo(), heal, DOT);

                        int32 basepoints0 = empoweredRenewAurEff->GetAmount() * GetEffect(EFFECT_0)->GetTotalTicks() * int32(heal) / 100;
                        caster->CastCustomSpell(GetTarget(), SPELL_PRIEST_EMPOWERED_RENEW, &basepoints0, NULL, NULL, true, NULL, aurEff);
                    }
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_pri_renew_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_renew_AuraScript();
        }
};

/**
  * A BasicEvent to tell if yes or no we can reset the cooldown
  */
class ShadowWordDeathResetCooldownEvent : public BasicEvent {
public :
    ShadowWordDeathResetCooldownEvent(Player* player) : _player(player) {}

    bool Execute(uint64 /*e_time*/, uint32 /*e_id*/) {
        if(_player)
            _player->SetCanShadowWordDeathReset(true);
    }

private :
    Player* _player ;
};

/**
 * Script for Shadow Word : Death (32379 (base) and 129176 (override with the glyph)
 * SQL Query : DELETE FROM spell_script_names WHERE spell_id IN (32379, 129176) ;
 *             INSERT INTO spell_script_names VALUES (32379, "spell_pri_shadow_word_death"), (129176, "spell_pri_shadow_word_death");
 * This spell is a bit complex, so let's go for some documentation :
 *
 * 32373 is the basic spell, the one the caster will always have in his bookspell while he has not chosen a specialisation
 * The casting condition is the following : target must have less than 20% HP
 * If the caster has the shadow specialisation, casting the spell will also energize the caster for ONE shadow orb (using the spell 125927)
 * If the target doesn't die, the cooldown on the spell will be instantly reset, but the spell will not give a shadow orb to the caster for nine seconds
 *
 * 129176 is the spell that overrides 32379 (when the caster has the glyph of Shadow Word : Death
 * This one is a bit more complicated : it's damages are the quarter of a normal Shadow Word : Death only if the target is upper 20% HP
 * If the target is under 20% HP, damage are quadruplated
 * Also, if the creature doesn't die, there are two type of events :
 *  If the creature is below 20% HP, but not yet dead, the cooldown is reset BUT THE SPELL WILL NOT GENERATE A SHADOW ORB AFTER NEXT CAST (only every 9 seconds)
 *  In all cases, we have to damage the caster for the amount (there is a periodic effect, spell 32409)
 * If the creature dies, we do not damage the caster
 */
class spell_pri_shadow_word_death : public SpellScriptLoader {
public :
    /// Constructor for the loader
    spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

    class spell_pri_shadow_word_death_SpellScript : public SpellScript {
        PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

        /// Make sure we have every spell loaded in memory
        bool Validate(const SpellInfo */*spellInfo*/) {
            if(sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOW_WORD_DEATH_BASE)
                    && sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE)
                    && sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOW_WORD_DEATH_DAMAGES)
                    && sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOW_WORD_DEATH_OVERRIDED)
                    && sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOW_WORD_DEATH_GLYPH))
                return true ;
            else {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: SW : Death: Unable to get SpellInfo for at least one spell !");
                return false ;
            }
        }

        /// Load everything
        bool Load() {
            // Check for the aura, to call the right script
            if(Unit* caster = GetCaster()) {
                if(Player* player = caster->ToPlayer()) {
                    if(player->HasAura(SPELL_PRIEST_SHADOW_WORD_DEATH_GLYPH))
                        glyphed = true ;
                    else
                        glyphed = false ;

                    return true ;
                }
                return false ;
            }
            return false ;
        }

        /// This part of the script is only for the glyphed version of the spell
        void HandleDamage(SpellEffIndex effectIndex) {
            if(glyphed) {
                if(Unit* hitUnit = GetHitUnit()) {
                    damages = GetHitDamage() ;
                    if(hitUnit->GetHealthPct() <= 20) {
                        SetHitDamage(GetHitDamage() * 4);
                        damages *= 4 ;
                    }
                }
            }
        }

        /// Handle everything linked to after hit : damages to caster, shadow orbs etc...
        void HandleAfterHit() {
            if(Unit* hitUnit = GetHitUnit()) {
                if(hitUnit->isAlive()) { // Everything works only if target is still alive
                    if(Player* player = GetCaster()->ToPlayer()) {
                        if(player->GetPrimaryTalentTree(player->GetActiveSpec()) == TALENT_TREE_PRIEST_SHADOW
                                && player->CanShadowWordDeathReset()) { // If we are a shadow priest and the spell can reset
                            player->CastSpell(player, SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE, true); // Add a shadow orb
                            player->SetCanShadowWordDeathReset(false); // Prevents multi-resetting
							BasicEvent* event = new ShadowWordDeathResetCooldownEvent(player) ;
                            player->m_Events.AddEvent(event, 9000); // Prepare next reset
                            player->RemoveSpellCooldown(glyphed ? SPELL_PRIEST_SHADOW_WORD_DEATH_OVERRIDED : SPELL_PRIEST_SHADOW_WORD_DEATH_BASE); // Remove cooldown
                        }

                        if(glyphed)
                            // Deal damages since target is not dead
                            player->CastCustomSpell(player, SPELL_PRIEST_SHADOW_WORD_DEATH_DAMAGES, &damages, NULL, NULL, true, NULL, NULL, player->GetGUID());
                    }
                } else {
					if(player->GetPrimaryTalentTree(player->GetActiveSpec()) == TALENT_TREE_PRIEST_SHADOW
                                && player->CanShadowWordDeathReset()) { // If we are a shadow priest and the spell can reset
                        player->CastSpell(player, SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE, true); // Add a shadow orb
					}
				}
            }
        }

        /// Register everything
        void Register() {
            OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_word_death_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            AfterHit += SpellHitFn(spell_pri_shadow_word_death_SpellScript::HandleAfterHit);
        }

        int32 damages ; /// Stores the amount of damages spell has done to the target, to damage the caster if target didn't die
        bool glyphed ; /// Okay, we set this during Load(), so we call the right script
    };

    SpellScript* GetSpellScript() const {
        return new spell_pri_shadow_word_death_SpellScript();
    }
};

// 15473 - Shadowform
class spell_pri_shadowform : public SpellScriptLoader
{
    public:
        spell_pri_shadowform() : SpellScriptLoader("spell_pri_shadowform") { }

        class spell_pri_shadowform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowform_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_shadowform_AuraScript();
        }
};

// 34914 - Vampiric Touch
class spell_pri_vampiric_touch : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetUnitOwner())
                        if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                        {
                            int32 damage = aurEff->GetAmount() * 8;
                            // backfire damage
                            caster->CastCustomSpell(target, SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, &damage, NULL, NULL, true, NULL, aurEff);
                        }
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }
};

// 81208 Chakra: Serenity

/* 

MUST ADD TO DATABASE : 

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (81208, 'spell_pri_chakra_serenity_proc');

*/

#define PRIEST_SPELL_RENEW 139

class spell_pri_chakra_serenity_proc : public SpellScriptLoader
{
public:
    spell_pri_chakra_serenity_proc() : SpellScriptLoader("spell_pri_chakra_serenity_proc") { }

    class spell_pri_chakra_serenity_proc_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_chakra_serenity_proc_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* target = GetHitUnit();

            if (!target)
                return;

            if (Aura* renew = target->GetAura(PRIEST_SPELL_RENEW, GetCaster()->GetGUID()))
                renew->RefreshDuration();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_chakra_serenity_proc_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_chakra_serenity_proc_SpellScript();
    }
};

/*

MUST APPLY THIS TO DATABASE :

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (108968, 'spell_pri_void_shift');

*/

// Void Shift - 108968
class spell_pri_void_shift : public SpellScriptLoader
{
    public:
        spell_pri_void_shift() : SpellScriptLoader("spell_pri_void_shift") { }

        class spell_pri_void_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_void_shift_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VOID_SHIFT))
                    return false;
                return true;
            }

            SpellCastResult CheckTarget()
            {
                if (GetExplTargetUnit())
                    if (GetExplTargetUnit()->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        float playerPct;
                        float targetPct;

                        playerPct = _player->GetHealthPct();
                        targetPct = target->GetHealthPct();

                        if (playerPct < 25.0f)
                            playerPct = 25.0f;
                        if (targetPct < 25.0f)
                            targetPct = 25.0f;

                        playerPct /= 100.0f;
                        targetPct /= 100.0f;

                        _player->SetHealth(_player->GetMaxHealth() * targetPct);
                        target->SetHealth(target->GetMaxHealth() * playerPct);
                    }
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pri_void_shift_SpellScript::CheckTarget);
                OnEffectHitTarget += SpellEffectFn(spell_pri_void_shift_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_void_shift_SpellScript;
        }
};

/**
 * Shadow Priest Spell : Mind spike (73510)
 * Updated 5.1.0
 * SQL Query : INSERT INTO spell_script_names VALUES (73510, "spell_pri_mind_spike");
 */
class spell_pri_mind_spike : public SpellScriptLoader {
public :
    /// Constructor
    spell_pri_mind_spike() : SpellScriptLoader("spell_pri_mind_spike") { }

    /// Script
    class spell_pri_mind_spike_SpellScript : public SpellScript {
        /// Get everything we need
        PrepareSpellScript(spell_pri_mind_spike_SpellScript)

        /// Validate the spell
        bool Validate(const SpellInfo */*spellInfo*/) {
            //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Validate");
            if(!sSpellMgr->GetSpellInfo(SPELL_PRIEST_MIND_SPIKE))
                return false ;

            return true ;
        }

        /// Init everything
        bool Load() {
            //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Load");
            // If no caster or caster is not a player, return ;
            if(GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER)
                return true ;

            return false ;
        }

        /// The dummy effect handles the removal of Shadow damage-over-time spells on the target
        /// We will get the target, then his auras, and then we will remove the ones that fit the following requirements :
        /// 1) Shadow damage-over-time spell
        /// 2) Caster is the same caster as the one casting this spell
        void HandleDummy(SpellEffIndex effectIndex) {
            //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: HandleDummy");
            if(Player* caster = GetCaster()->ToPlayer()) {// Since we checked caster during load, he will not be null
                if(Unit* target = GetHitUnit()) {
                    //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: caster && target not null");
                    Unit::AuraApplicationMap const& appliedAuras = target->GetAppliedAuras() ;
                    for(Unit::AuraApplicationMap::const_iterator iter = appliedAuras.begin() ; iter != appliedAuras.end() ; ++iter) {
                        //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Looping in AuraApplicationMap");
                        if(AuraApplication* auraApp = iter->second) {
                            //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Pointer to AuraApplication");
                            if(Aura* base = auraApp->GetBase()) {
                                // 1. Check the caster
                                //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Checking casters : aura's caster == %u, caster == % u, auraName == %s",
                                               //base->GetCasterGUID(), caster->GetGUID(), base->GetSpellInfo()->SpellName);
                                if(base->GetCasterGUID() != caster->GetGUID())
                                    continue ;

                                //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Checking mask : school mask == %u",
                                               //base->GetSpellInfo()->SchoolMask & SPELL_SCHOOL_MASK_SHADOW);
                                // 2. Caster is the same, now check if it is a shadow spell
                                if(base->GetSpellInfo()->SchoolMask & SPELL_SCHOOL_MASK_SHADOW == 0)
                                    continue ;

                                //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: checking effects");
                                // 3. Now check the effects ; it's a bit long, we have to loop over each effect to see if it is periodic, but we cannot do it another way
                                for(uint8 i = 0 ; i < MAX_SPELL_EFFECTS ; ++i) {
                                    if(AuraEffect* auraEff = base->GetEffect(i))
                                        if(auraEff->IsPeriodic()) {
                                            //sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Mind Spike: Found periodic effect, index == %u",
                                                           //uint32(i));
                                            target->RemoveAura(base, AURA_REMOVE_BY_ENEMY_SPELL);
                                            break ; // We have found a periodic effect ; stop looping over effects, return looping over auras
                                        }
                                }
                            }
                        }
                    }
                }
            }
        }

        /// Now register the spell
        void Register() {
            OnEffectHitTarget += SpellEffectFn(spell_pri_mind_spike_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    /// Get a pointer to the script as a pointer to SpellScript
    SpellScript* GetSpellScript() const {
        return new spell_pri_mind_spike_SpellScript() ;
    }
};

/**
 * NPC summoned by Shadowfiend (34433)
 * SQL Query : UPDATE creature_template SET ScriptName = "npc_shadowfiend" WHERE entry = 19668 ;
 */
class npc_shadowfiend : public CreatureScript {
public :
    /// Constructor of the CreatureScript
    npc_shadowfiend() : CreatureScript("npc_shadowfiend") { }

    /// Script
    class npc_shadowfiend_AI : public ScriptedAI {
    public :
        /// Script's constructor
        npc_shadowfiend_AI(Creature* creature) : ScriptedAI(creature) {
            // We are a temp summon ; get the owner
            if(TempSummon* summon = creature->ToTempSummon()) {
                if(Unit* owner = summon->GetOwner()) {
                    if(Player* player = owner->ToPlayer()) {
                        // Attack the masters victim if any
                        if(Unit* victim = player->GetSelectedUnit()) {
                            AttackStart(victim);
                        }
                        // Master's don't have a selected target ; attack the victim
                        else if(Unit* victim = player->getVictim()) {
                            AttackStart(victim);
                        }
                        // Master's doesn't have a victim : find one !
                        else {
                            // ThreatList is empty, find the nearest unit and attack it
                            if(player->getThreatManager().getThreatList().empty()) {
                                CellCoord coords(Trinity::ComputeCellCoord(player->GetPositionX(), player->GetPositionY()));
                                Cell cell(coords);
                                cell.SetNoCreate();

                                std::list<Unit*> targets ;

                                Trinity::AnyUnfriendlyUnitInObjectRangeCheck check(player, player, 84.0f); // 7 yards / s, so in 12 seconds = 84 yards
                                Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(player, targets, check);

                                TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck>, GridTypeMapContainer> visitor(searcher);
                                TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck>, WorldTypeMapContainer> visitor2(searcher);
								
								Map* map = creature->GetMap(); 
                                cell.Visit(coords, visitor, *map, *creature, 84.0f);
                                cell.Visit(coords, visitor2, *map, *creature, 84.0f);

                                if(!targets.empty()) {
                                    targets.sort(Trinity::DistanceCompareOrderPred(player, true));
                                    Unit* target = targets.front();
                                    AttackStart(target);
                                }
                                else
                                    creature->DisappearAndDie();
                            } else {
                                // Threat List not empty, pick the closest unit and attack it
                                std::list<HostileReference*> threatList = player->getThreatManager().getThreatList() ;
								std::list<Unit*> asUnit ;
								for(std::list<HostileReference*>::const_iterator iter = threatList.begin() ; iter != threatList.end() ; ++iter) {
									asUnit.push_back((*iter)->getTarget()) ;
								}
                                asUnit.sort(Trinity::DistanceCompareOrderPred(player, true));
                                Unit* target = asUnit.front();
                                AttackStart(target);
                            }
                        }
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff) {
            // Return since we have no target
            if(!UpdateVictim())
                return ;

            DoRegenerateMana();
        }

        /// Replaces DoMeleeAttackIfReady()
        /// There is no need to check unit state casting, since we never cast a spell
        /// Everything is just a copy of DoMeleeAttackIfReady(), but we need it to handle regen
        void DoRegenerateMana() {
            bool canRegen = false ; // If set to false after every check, we are not ready to do a melee attack

            Unit* victim = me->getVictim(); // Get the victim
            if(!victim)
                return ;

            /// Checks
            if (me->isAttackReady() && me->IsWithinMeleeRange(victim)) {
                me->AttackerStateUpdate(victim);
                me->resetAttackTimer();
                canRegen = true ; // Okay, we area ready, so we can regen
            }

            if (me->haveOffhandWeapon() && me->isAttackReady(OFF_ATTACK) && me->IsWithinMeleeRange(victim)) {
                me->AttackerStateUpdate(victim, OFF_ATTACK);
                me->resetAttackTimer(OFF_ATTACK);
                canRegen = true ;
            }

            if(canRegen) {
                if(TempSummon* summon = me->ToTempSummon())
                    if(Unit* owner = summon->GetOwner()) // Get owner
                        DoCast(owner, 34650, true); // We are happy : there is spell, so cast it
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const {
        return new npc_shadowfiend_AI(creature);
    }
};


void AddSC_priest_spell_scripts()
{
    new spell_pri_divine_aegis();
    new spell_pri_glyph_of_prayer_of_healing();
    new spell_pri_guardian_spirit();
    new spell_pri_leap_of_faith_effect_trigger();
    new spell_pri_lightwell_renew();
    new spell_pri_mana_burn();
    new spell_pri_mana_leech();
    new spell_pri_mind_sear();
    new spell_pri_pain_and_suffering_proc();
    new spell_pri_penance();
    new spell_pri_power_word_shield();
    new spell_pri_prayer_of_mending_heal();
    new spell_pri_reflective_shield_trigger();
    new spell_pri_renew();
    new spell_pri_shadow_word_death();
    new spell_pri_shadowform();
    new spell_pri_vampiric_touch();
	new spell_pri_chakra_serenity_proc();
	new spell_pri_void_shift();
	new spell_pri_mind_spike();
	new npc_shadowfiend();
}
