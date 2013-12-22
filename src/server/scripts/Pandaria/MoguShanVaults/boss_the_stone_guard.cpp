/* # Script de Sungis : The Stone Guard # */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mogushan_vaults.h"

enum Spells
{
	/* Amethyst Guardian */
    SPELL_AMETHYST_OVERLOAD				= 115844,
    SPELL_AMETHYST_PETRIFICATION		= 116057,
    SPELL_AMETHYST_PETRIFICATION_BAR	= 131255,
    SPELL_AMETHYST_TRUE_FORM			= 115829,
    SPELL_AMETHYST_POOL					= 116235,

	/* Cobalt Guardian */
    SPELL_COBALT_OVERLOAD				= 115840,
    SPELL_COBALT_PETRIFICATION			= 115852,
    SPELL_COBALT_PETRIFICATION_BAR		= 131268,
    SEPLL_COBALT_TRUE_FORM				= 115771,
    SPELL_COBALT_MINE					= 129460,

	/* Jade Guardian */
    SPELL_JADE_OVERLOAD                 = 115842,
    SPELL_JADE_PETRIFICATION            = 116006,
    SPELL_JADE_PETRIFICATION_BAR        = 131269,
    SEPLL_JADE_TRUE_FORM                = 115827,
    SPELL_JADE_SHARDS                   = 116223,

	/* Jasper Guardian */
    SPELL_JASPER_OVERLOAD				= 115843,
    SPELL_JASPER_PETRIFICATION			= 116036,
    SPELL_JASPER_PETRIFICATION_BAR		= 131270,
    SEPLL_JASPER_TRUE_FORM				= 115828,
    SPELL_JASPER_CHAINS					= 130395,
    SPELL_JASPER_CHAINS_VISUAL			= 130403,
    SPELL_JASPER_CHAINS_DAMAGE			= 130404,

    /* Shared Guardian Spells */
    SPELL_SOLID_STONE					= 115745,
	SPELL_STONE_VISUAL					= 123947,
    SPELL_REND_FLESH					= 125206,
    SPELL_ANIM_SIT						= 128886,
    SPELL_ZERO_ENERGY					= 72242,
    SPELL_TOTALY_PETRIFIED				= 115877,
	SPELL_BERSERK						= 26662,
	
};

enum Events
{

};

enum Texts
{

};

enum Phases
{

};

enum Npcs
{

};

class boss_amethyst_guardian : public CreatureScript
{
    public:
        boss_amethyst_guardian() : CreatureScript("boss_amethyst_guardian") { }

        struct boss_amethyst_guardianAI : public BossAI
        {
            boss_amethyst_guardianAI(Creature* creature) : BossAI(creature, DATA_THE_STONE_GUARD)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Creature* cobalt;
			Creature* jade;
			Creature* jasper;

            void Reset()
            {
				_Reset();

				events.Reset();

				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT);
            }

			void EnterEvadeMode()
			{
				_EnterEvadeMode();
			}

            void EnterCombat(Unit* /*who*/)
            {
				_EnterCombat();

				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				me->RemoveAurasDueToSpell(SPELL_ANIM_SIT, me->GetGUID());
				
				if (!me->isInCombat())
					me->SetInCombatWithZone();
				if (cobalt = me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 99999.0f, true))
					if (!cobalt->isInCombat())
						cobalt->SetInCombatWithZone();
				if (jade = me->FindNearestCreature(BOSS_JADE_GUARDIAN, 99999.0f, true))
					if (!jade->isInCombat())
						jade->SetInCombatWithZone();
				if (jasper = me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 99999.0f, true))
					if (!jasper->isInCombat())
						jasper->SetInCombatWithZone();
            }

            void JustDied(Unit* /*killer*/)
            {
				_JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_amethyst_guardianAI(creature);
        }
};

class boss_cobalt_guardian : public CreatureScript
{
    public:
        boss_cobalt_guardian() : CreatureScript("boss_cobalt_guardian") { }

        struct boss_cobalt_guardianAI : public BossAI
        {
            boss_cobalt_guardianAI(Creature* creature) : BossAI(creature, DATA_THE_STONE_GUARD)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Creature* amethyst;
			Creature* jade;
			Creature* jasper;

            void Reset()
            {
				_Reset();

				events.Reset();

				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT);
            }

			void EnterEvadeMode()
			{
				_EnterEvadeMode();
			}

            void EnterCombat(Unit* /*who*/)
            {
				_EnterCombat();

				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				me->RemoveAurasDueToSpell(SPELL_ANIM_SIT, me->GetGUID());
				
				if (!me->isInCombat())
					me->SetInCombatWithZone();
				if (amethyst = me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 99999.0f, true))
					if (!amethyst->isInCombat())
						amethyst->SetInCombatWithZone();
				if (jade = me->FindNearestCreature(BOSS_JADE_GUARDIAN, 99999.0f, true))
					if (!jade->isInCombat())
						jade->SetInCombatWithZone();
				if (jasper = me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 99999.0f, true))
					if (!jasper->isInCombat())
						jasper->SetInCombatWithZone();
            }

            void JustDied(Unit* /*killer*/)
            {
				_JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_cobalt_guardianAI(creature);
        }
};

class boss_jade_guardian : public CreatureScript
{
    public:
        boss_jade_guardian() : CreatureScript("boss_jade_guardian") { }

        struct boss_jade_guardianAI : public BossAI
        {
            boss_jade_guardianAI(Creature* creature) : BossAI(creature, DATA_THE_STONE_GUARD)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Creature* amethyst;
			Creature* cobalt;
			Creature* jasper;

            void Reset()
            {
				_Reset();

				events.Reset();

				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT);
            }

			void EnterEvadeMode()
			{
				_EnterEvadeMode();
			}

            void EnterCombat(Unit* /*who*/)
            {
				_EnterCombat();

				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				me->RemoveAurasDueToSpell(SPELL_ANIM_SIT, me->GetGUID());
				
				if (!me->isInCombat())
					me->SetInCombatWithZone();
				if (amethyst = me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 99999.0f, true))
					if (!amethyst->isInCombat())
						amethyst->SetInCombatWithZone();
				if (cobalt = me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 99999.0f, true))
					if (!cobalt->isInCombat())
						cobalt->SetInCombatWithZone();
				if (jasper = me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 99999.0f, true))
					if (!jasper->isInCombat())
						jasper->SetInCombatWithZone();
            }

            void JustDied(Unit* /*killer*/)
            {
				_JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jade_guardianAI(creature);
        }
};

class boss_jasper_guardian : public CreatureScript
{
    public:
        boss_jasper_guardian() : CreatureScript("boss_jasper_guardian") { }

        struct boss_jasper_guardianAI : public BossAI
        {
            boss_jasper_guardianAI(Creature* creature) : BossAI(creature, DATA_THE_STONE_GUARD)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Creature* amethyst;
			Creature* cobalt;
			Creature* jade;

            void Reset()
            {
				_Reset();

				events.Reset();

				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT);
            }

			void EnterEvadeMode()
			{
				_EnterEvadeMode();
			}

            void EnterCombat(Unit* /*who*/)
            {
				_EnterCombat();

				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				me->RemoveAurasDueToSpell(SPELL_ANIM_SIT, me->GetGUID());
				
				if (!me->isInCombat())
					me->SetInCombatWithZone();
				if (amethyst = me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 99999.0f, true))
					if (!amethyst->isInCombat())
						amethyst->SetInCombatWithZone();
				if (cobalt = me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 99999.0f, true))
					if (!cobalt->isInCombat())
						cobalt->SetInCombatWithZone();
				if (jade = me->FindNearestCreature(BOSS_JADE_GUARDIAN, 99999.0f, true))
					if (!jade->isInCombat())
						jade->SetInCombatWithZone();
            }

            void JustDied(Unit* /*killer*/)
            {
				_JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jasper_guardianAI(creature);
        }
};

void AddSC_boss_the_stone_guard()
{
    new boss_amethyst_guardian();
	new boss_cobalt_guardian();
	new boss_jade_guardian();
	new boss_jasper_guardian();
}