#include "stormstout_brewery.h"

/**
 * Bon je vais la faire en français, ce sera plus simple a comprendre :
 * Le spell Summon Suds :
 * Le principe en lui-meme est simple :
 * 1. On prend un cote de la salle aleatoirement
 * 2. On prend les coordonees de chaque extremite
 * 3. On calcule l'equation de la droite formee par les deux extremitees
 * 4. On pop des npcs a intervalles reguliers sur cette droite
 * 5. On calcule l'equation de la parallele a l'autre bout de la salle en prenant les coords de chaque extremite
 * 6. Chaque npc se deplace jusqu'au point correspondant
 *
 * Ce qui nous fait donc :
 * Quatre positions : sommet de depart, bas de depart, sommet d'arrivee, bas d'arrivee
 */

// A gauche les points de summon debut et fin
// A droite les points d'arrivee debut et fin
static const Position summonHorizontal[2][2] =
{
    {{0.0f, 0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f, 0.0f}}
};

static const Position summonVertical[2][2] =
{
    {{0.0f, 0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f, 0.0f}}
};

#define MAX_SUMMON_FIZZY_BUBBLES_POSITION 12
static const Position summonFizzyBubblePositions[MAX_SUMMON_FIZZY_BUBBLES_POSITION] =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}
};

#define FLOOR_Z 250.0f // This is horrible, but as the height of a player is higher when it jumps... this is the only check we can do... ten seconds of loop

enum Spells
{
    /// Yan'Zhu

    // Generic
    Spell_YanZhu_BrewBolt = 114548,

    // Wheat
    Spell_YanZhu_Bloat = 106546, // WHAT THE FUCK ? HOW DO YOU HANDLE THE TARGETS ? DONT WORK
    Spell_YanZhu_Bloated = 106549, // The triggerer of the crazy spell DONT WORK
    Spell_YanZhu_BloatedPeriodic = 106560, // OH MY FUCKING GOD DONT WORK
    Spell_YanZhu_BlackoutBrew = 106851, // Watch carefully WORKS
    Spell_YanZhu_BlackoutDrunk = 106857, // When Blackout Brew reaches 10 stacks TO SCRIPT

    // Ale
    Spell_YanZhu_BubbleShield = 106563, // Npcs ? Can stack ? WORKS
    // Here comes the yeasty brew alamental

    // Stout
    Spell_YanZhu_Carbonation = 115003, // WORKS
    Spell_YanZhu_CarbonationPeriodic = 114386, // Must handle target select ; also there is a Carbonation that summons npcs... but WORKS
    Spell_YanZhu_WallOfSuds = 114467,
    Spell_YanZhu_WallOfSudsPeriodic = 114466,
    Spell_YanZhu_Sudsy = 114468,
    Spell_YanZhu_SudsyTriggered = 114470, // Scriptable ? I doubt >.<

    Spell_YeastyBrewAlamental_BrewBolt = 114548,
    Spell_YeastyBrewAlamental_Ferment = 106859,
    Spell_YeastyBrewAlamental_FermentPeriodic = 114451
};

enum Events
{
    Event_YanZhu_BrewBolt = 1,
    Event_YanZhu_AleAbility = 2,
    Event_YanZhu_StoutAbility = 3,
    Event_YanZhu_WheatAbility = 4,

    Event_YeastyBrewAlamental_BrewBolt = 1,
    Event_YeastyBrewAlamental_Ferment = 2
};

#define NUMBER_OF_SUMMONED_SUDS 10U

class boss_yan_zhu_the_uncasked : public CreatureScript
{
public :
    boss_yan_zhu_the_uncasked() : CreatureScript("boss_yan_zhu_the_uncasked")
    {

    }

    struct boss_yan_zhu_the_uncasked_AIScript : public ScriptedAI
    {
    public :
        boss_yan_zhu_the_uncasked_AIScript(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            if(instance)
                instance->SetData(Data_YanZhuEventProgress, NOT_STARTED);

            m_uiWheatAbility = RAND(Spell_YanZhu_Bloat, Spell_YanZhu_BlackoutBrew);
            m_uiAleAbility = RAND(0, Spell_YanZhu_BubbleShield);
            m_uiStoutAbility = RAND(Spell_YanZhu_Carbonation, Spell_YanZhu_WallOfSuds);

            m_uiBrewBoltTimer = 500 ;

            events.Reset();
        }

        void EnterEvadeMode()
        {
            ScriptedAI::EnterEvadeMode();
            if(instance)
                instance->SetData(Data_YanZhuEventProgress, FAIL);
        }

        void EnterCombat(Unit *who)
        {
            if(instance)
                instance->SetData(Data_YanZhuEventProgress, IN_PROGRESS);

            DoZoneInCombat();
            events.ScheduleEvent(Event_YanZhu_AleAbility, IsHeroic() ? 25000 : 30000);
            events.ScheduleEvent(Event_YanZhu_StoutAbility, IsHeroic() ? 35000 : 40000);
            events.ScheduleEvent(Event_YanZhu_WheatAbility, IsHeroic() ? 30000 : 35000);
        }

        void JustDied(Unit *killer)
        {
            if(instance)
                instance->SetData(Data_YanZhuEventProgress, DONE);

            ClearZone();
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;

            if(!DoCheckPlayers())
            {
                if(m_uiBrewBoltTimer <= diff)
                {
                    if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        DoCast(target, Spell_YanZhu_BrewBolt);
                    m_uiBrewBoltTimer = 500 ;
                }
                else
                    m_uiBrewBoltTimer -= diff ;
            }

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case Event_YanZhu_AleAbility :
                    if(m_uiAleAbility)
                        SummonBubbleShield();
                    else
                        SummonYeastyBrewAlamental();

                    events.ScheduleEvent(Event_YanZhu_AleAbility, IsHeroic() ? 25000 : 30000);
                    break ;

                case Event_YanZhu_StoutAbility :
                    if(m_uiStoutAbility == Spell_YanZhu_Carbonation)
                    {
                        DoCast(m_uiStoutAbility);
                        SummonFizzyBubbles();
                    }
                    else
                        CastWallOfSuds();

                    events.ScheduleEvent(Event_YanZhu_StoutAbility, IsHeroic() ? 35000 : 40000);
                    break ;

                case Event_YanZhu_WheatAbility :
                    if(m_uiWheatAbility == Spell_YanZhu_BlackoutBrew)
                        DoCast(m_uiWheatAbility);
                    else
                    {
                        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(target, m_uiWheatAbility, true);
                    }

                    events.ScheduleEvent(Event_YanZhu_WheatAbility, IsHeroic() ? 30000 : 35000);
                    break ;

                default :
                    break ;
                }
            }
        }

        void SummonBubbleShield()
        {
            const float rayon = 2.0f ;
            for(float i = 0 ; i < 2 * M_PI ; i += 2 * M_PI / 8)
            {
                float x = me->GetPositionX() + rayon * cos(i);
                float y = me->GetPositionY() + rayon * sin(i);
                float z = me->GetPositionZ() + 3.0f ;

                if(Creature* bubbleShield = me->SummonCreature(Mob_BubbleShield, x, y, z))
                    bubbleShield->CastSpell(me, Spell_YanZhu_BubbleShield, true);
            }
        }

        void SummonYeastyBrewAlamental()
        {
            for(uint8 i = 0 ; i < DoCountCasters() ; ++i)
            {
                float x = me->GetPositionX() + rand() % 4 ;
                float y = me->GetPositionY() + rand() % 4 ;
                float z = me->GetPositionZ() ;

                me->SummonCreature(Mob_YeastyBrewAlamental, x, y, z);
            }
        }

        void SummonFizzyBubbles()
        {
            for(uint8 i = 0 ; i < MAX_SUMMON_FIZZY_BUBBLES_POSITION ; ++i)
            {
                Position pos ;
                pos.Relocate(summonFizzyBubblePositions[i]);
                me->SummonCreature(Npc_FizzyBubble, pos, TEMPSUMMON_TIMED_DESPAWN, 20000);
            }
        }

        void CastWallOfSuds()
        {
            SummonSuds();
            SummonSuds(false);
        }

        bool DoCheckPlayers()
        {
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const & playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
                        if(Player* p = iter->getSource())
                        {
                            if(p->GetExactDist2d(me->GetPositionX(), me->GetPositionY()) <= 1.5f)
                                return true ;
                            else
                                continue ;
                        }
                    }
                }
            }

            return false ;
        }

        uint32 DoCountCasters()
        {
            uint32 count = 0 ;
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const & playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
                        if(Player *p = iter->getSource())
                        {
                            if(p->GetPower(POWER_MANA))
                                ++count ;
                        }
                    }
                }
            }

            return count ;
        }

        void DespawnMinions(uint32 entry)
        {
            std::list<Creature*> minions ;
            GetCreatureListWithEntryInGrid(minions, me, entry, 50000.0f);

            if(!minions.empty())
            {
                for(std::list<Creature*>::const_iterator iter = minions.begin() ; iter != minions.end() ; ++iter)
                {
                    if(Creature* minion = *iter)
                        minion->DisappearAndDie();
                }
            }
        }

        inline void ClearZone()
        {
            DespawnMinions(Mob_BubbleShield);
            DespawnMinions(Mob_YeastyBrewAlamental);
            DespawnMinions(Npc_Suds);
        }

        /** Wall of Suds system */
        void SummonSuds(bool horizontal = true)
        {
            Position debutSummon, finSummon, debutArrivee, finArrivee; // The four positions we will need
            RelocatePositions(&debutSummon, &finSummon, &debutArrivee, &finArrivee, horizontal); // Relocate them (generic)

            float coeffSummon, ordoSummon, progressSummon, coeffEnd, ordoEnd, progressEnd ; // Enough to get two equations
            CalculateEquation(&debutSummon, &finSummon, coeffSummon, ordoSummon, progressSummon); // Calculate them
            CalculateEquation(&debutArrivee, &finArrivee, coeffEnd, ordoEnd, progressEnd);

            // Start summoning
            for(uint32 i = 0 ; i < NUMBER_OF_SUMMONED_SUDS ; ++i)
            {
                float summon_x, summon_y, summon_z, move_x, move_y, move_z ; // Six points to build two positions
                RelocatePoints(&debutSummon, i, progressSummon, coeffSummon, ordoSummon, summon_x, summon_y, summon_z); // Relocate the summon
                RelocatePoints(&debutArrivee, i, progressEnd, coeffEnd, ordoEnd, move_x, move_y, move_z); // Relocate the move point

                // Compute all
                if(Creature* summon = me->SummonCreature(Npc_Suds, summon_x, sumon_y, summon_z))
                {
                    summon->CastSpell(summon, Spell_YanZhu_WallOfSuds, true);
                    summon->GetMotionMaster()->MovePoint(0, move_x, move_y, move_z);
                }
            }
        }

        /** Generic function that relocate the four positions passed as arguments with the arrays containing the summoning positions */
        /** Param horizontal determines which array we will use */
        void RelocatePositions(Position* startSummon, Position* endSummon, Position* moveStart, Position* moveEnd, bool const& horizontal = true)
        {
            if(horizontal)
            {
                startSummon->Relocate(summonHorizontal[0][0]);
                endSummon->Relocate(summonHorizontal[1][0]);

                moveStart->Relocate(summonHorizontal[0][1]);
                moveEnd->Relocate(summonHorizontal[1][1]);
            }
            else
            {
                startSummon->Relocate(summonVertical[0][0]);
                endSummon->Relocate(summonVertical[1][0]);

                moveStart->Relocate(summonVertical[0][1]);
                moveEnd->Relocate(summonVertical[1][1]);
            }
        }

        // y = ax + b
        // b = y - ax
        /** Generic function that calculates the equation formed by two points (Position passed as args) */
        /** Also it determines the distance between the x position of two summons, knowing the total number of summon (defined NUMBER_OF_SUMMONED_SUDS) */
        void CalculateEquation(Position* start, Position* end, float& coeff, float& ordo, float& progress)
        {
            float x = start->GetPositionX();
            coeff = (end->GetPositionY() - start->GetPositionY()) / (end->GetPositionX() - start->GetPositionX()); // a

            float y = start->GetPositionY() ;
            ordo = y - coeff * x ;

            float delta = fabs(end->GetPositionX() - start->GetPositionX());
            progress = delta / NUMBER_OF_SUMMONED_SUDS ;
        }

        /** Generic function that calculates x, y, and z coordinates knowing the component of the equation */
        void RelocatePoints(Position const* start, uint32 i,
                            float const& progress, float const& coeff, float const& ordo,
                            float& x, float& y, float& z)
        {
            x = start->GetPositionX() + i * progress ;
            y = x * coeff + ordo ;
            z = start->GetPositionZ() ;
        }

    private :
        EventMap events ;
        InstanceScript* instance ;

        uint32 m_uiAleAbility ;
        uint32 m_uiWheatAbility ;
        uint32 m_uiStoutAbility ;

        uint32 m_uiBrewBoltTimer ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new boss_yan_zhu_the_uncasked_AIScript(creature);
    }
};

class mob_yeasty_brew_alamental : public CreatureScript
{
public :
    mob_yeasty_brew_alamental() : CreatureScript("mob_yeasty_brew_alamental")
    {

    }

    struct mob_yeasty_brew_alamental_AIScript : public ScriptedAI
    {

    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_yeasty_brew_alamental_AIScript(creature);
    }
};

class npc_fizzy_bubble : public CreatureScript
{
public :
    npc_fizzy_bubble() : CreatureScript("npc_fizzy_bubble")
    {

    }

    bool OnGossipHello(Player *player, Creature *creature)
    {
        if(!p || !creature)
            return false ;

        if(player->HasAura(114458))
            return false ;

        player->CastSpell(player, 114458, true);
        player->PlayerTalkClass->SendCloseGossip();
        creature->DisappearAndDie();
        return true ;
    }
};

class npc_sudsy_stalker : public CreatureScript
{
public :
    npc_sudsy_stalker() : CreatureScript("npc_sudsy_stalker")
    {

    }

    struct npc_sudsy_stalker_AIScript : public ScriptedAI
    {

    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_sudsy_stalker_AIScript(creature);
    }
};

class spell_yan_zhu_blackout_brew : public SpellScriptLoader
{
public :
    spell_yan_zhu_blackout_brew() : SpellScriptLoader("spell_yan_zhu_blackout_brew")
    {

    }

    class spell_yan_zhu_blackout_brew_AuraScript : public AuraScript
    {

    };

    AuraScript* GetAuraScript() const
    {
        return new spell_yan_zhu_blackout_brew_AuraScript();
    }
};

void AddSC_boss_yan_zhu_the_uncasked()
{
    new boss_yan_zhu_the_uncasked();
}
