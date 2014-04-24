#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "PassiveAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "Pet.h"
#include "SpellScript.h"
#include "Spell.h"
#include "BrawlersGuildMgr.h"

/************************/
/****** BASTONNEUR ******/
/************************/

/* Rang 1*/

// Bruce
class npc_bruce : public CreatureScript
{
public:
    npc_bruce() : CreatureScript("npc_bruce") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bruceAI(creature);
    }

    struct npc_bruceAI : public ScriptedAI
    {
        npc_bruceAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 powerfulBiteTimer;
        uint32 chompTimer;

        void Reset()
        {
            powerfulBiteTimer = 3000;
            chompTimer = 6000;
        }

        void KilledUnit(Unit* unit)
        {
            if(Player* player = unit->toPlayer())
                sBrawlersGuildMgr->BossReport(player, false);
        }

        void JustDied(Unit* killer)
        {
            if(Player* player = killer->toPlayer())
                sBrawlersGuildMgr->BossReport(player, true);
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            if (powerfulBiteTimer <= diff)
            {
                me->CastSpell(me->getVictim(), 135340, false);
                powerfulBiteTimer = 8000;
            }
            else powerfulBiteTimer -= diff;

            if (chompTimer <= diff)
            {
                me->CastSpell(me, 135342, false);
                chompTimer = 8000;
            }
            else chompTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*class spell_chomp: public SpellScriptLoader
{
    public:
        spell_chomp() : SpellScriptLoader("spell_chomp") { }

        class spell_chomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chomp_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                //targets.remove_if(InArcCheckPredicate(GetCaster(), 120.0f * M_PI / 180.0f));
                targets.remove_if(WorldObjectSpellConeTargetCheck(M_PI, 10.00f, GetCaster(), GetSpellInfo(), TARGET_CHECK_ENEMY, NULL));
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_chomp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_chomp_SpellScript();
        }
};*/

/************************/


// Vian
class npc_vian_the_volatile : public CreatureScript
{
public:
    npc_vian_the_volatile() : CreatureScript("npc_vian_the_volatile") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vian_the_volatileAI(creature);
    }

    struct npc_vian_the_volatileAI : public ScriptedAI
    {
        npc_vian_the_volatileAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 volatileflamesTimer;
        uint32 lavaburstTimer;
        uint32 firelineTimer;

        void Reset()
        {
            volatileflamesTimer = 1000;
            lavaburstTimer = 4000;
            firelineTimer = 10000;
        }

        void KilledUnit(Unit* unit)
        {
            if(Player* player = unit->ToPlayer())
                sBrawlersGuildMgr->BossReport(player, false);
        }

        void JustDied(Unit* killer)
        {
            if(Player* player = killer->ToPlayer())
                sBrawlersGuildMgr->BossReport(player, true);
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            if (volatileflamesTimer <= diff)
            {
                me->CastSpell(me, 133430, false);
                volatileflamesTimer = 15000;
            }
            else volatileflamesTimer -= diff;

            if (lavaburstTimer <= diff)
            {
                me->CastSpell(me->getVictim(), 133293, false);
                lavaburstTimer = 15000;
            }
            else lavaburstTimer -= diff;

            if (firelineTimer <= diff)
            {
                me->CastSpell(me, 133607, false);
                firelineTimer = 30000;
            }
            else firelineTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_the_brawlers_guild_boss()
{
    new npc_bruce();
    new npc_vian_the_volatile();
}
