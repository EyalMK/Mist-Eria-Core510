/* # Script de Sungis : The Stone Guard # */

/* Notes : What is missing ? :	- Guardians personnal spells (Amethyst & Jasper & Jade)
								- 
*/

#include "ScriptPCH.h"
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
    SPELL_COBALT_TRUE_FORM				= 115771,
    SPELL_COBALT_MINE					= 129460,

	/* Jade Guardian */
    SPELL_JADE_OVERLOAD                 = 115842,
    SPELL_JADE_PETRIFICATION            = 116006,
    SPELL_JADE_PETRIFICATION_BAR        = 131269,
    SPELL_JADE_TRUE_FORM                = 115827,
    SPELL_JADE_SHARDS                   = 116223,

	/* Jasper Guardian */
    SPELL_JASPER_OVERLOAD				= 115843,
    SPELL_JASPER_PETRIFICATION			= 116036,
    SPELL_JASPER_PETRIFICATION_BAR		= 131270,
    SPELL_JASPER_TRUE_FORM				= 115828,
    SPELL_JASPER_CHAINS					= 130395,
    SPELL_JASPER_CHAINS_VISUAL			= 130403,
    SPELL_JASPER_CHAINS_DAMAGE			= 130404,

    /* Shared Guardian Spells */
    SPELL_SOLID_STONE					= 115745,
	SPELL_STONE_VISUAL					= 123947,
	SPELL_ANIM_SIT						= 128886,
    SPELL_REND_FLESH					= 125206,
    SPELL_ZERO_POWER					= 72242,
    SPELL_TOTALY_PETRIFIED				= 115877,
	SPELL_BERSERK						= 26662,

	/* Cobalt Mine */
	SPELL_COBALT_MINE_EXPLOSION			= 116281,
	SPELL_COBALT_MINE_VISUAL			= 129455,
};

enum Events
{
	/* Shared Guardian Events */
	EVENT_REND_FLESH				= 1,
	EVENT_PETRIFICATION_SET			= 2,
	EVENT_PETRIFICATION_INCREASE_1	= 3,
	EVENT_PETRIFICATION_INCREASE_2	= 4,
	EVENT_PETRIFICATION_INCREASE_3	= 5,
	EVENT_INCREASE_POWER_1			= 6,
	EVENT_INCREASE_POWER_2			= 7,
	EVENT_BERSERK					= 8,

	/* Amethyst Guardian */
	EVENT_AMETHYST_POOL				= 9,

	/* Cobalt Guardian */
	EVENT_COBALT_MINE				= 9,

	/* Jade Guardian */
	EVENT_JADE_SHARDS				= 9,

	/* Jasper Guardian */


	/* The Stone Guard Tracker */
	EVENT_CHOOSE_PETRIFICATION		= 1,

	/* Cobalt mine */
	EVENT_COBALT_MINE_ACTIVATION	= 1,
};

enum Actions
{
	ACTION_CHOOSE_PETRIFICATION,
	ACTION_PETRIFICATION_BAR,
	ACTION_TRACKER_RESET,
};

enum Npcs
{
	NPC_THE_STONE_GUARD_TRACKER		= 400463,
};

enum Guardians
{
	AMETHYST	= 1,
	COBALT		= 2,
	JADE		= 3,
	JASPER		= 4
};

class boss_amethyst_guardian : public CreatureScript
{
    public:
        boss_amethyst_guardian() : CreatureScript("boss_amethyst_guardian") { }

		CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_amethyst_guardianAI(creature);
        }

        struct boss_amethyst_guardianAI : public ScriptedAI
        {
            boss_amethyst_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Map* map;
			bool solidStone;

            void Reset()
            {
				events.Reset();
				solidStone = false;
				map = me->GetMap();

				me->setPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
				me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT, true);
				me->CastSpell(me, SPELL_ZERO_POWER);
            }

			void JustReachedHome()
            {
				me->CastSpell(me, SPELL_ANIM_SIT, true);
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_PETRIFICATION_BAR:
						events.ScheduleEvent(EVENT_PETRIFICATION_SET, 0);
						break;
				}
			}

			void EnterEvadeMode()
			{
				ScriptedAI::EnterEvadeMode();

				if (map)
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
							{
								player->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_TOTALY_PETRIFIED);
								player->SetPower(POWER_ALTERNATE_POWER, 0);
							}
				}

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_TRACKER_RESET);
			}

			void DamageTaken(Unit* who, uint32& damage)
			{
				if (instance)
				{
					if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
						if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
							if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
							{
								if (cobalt->isAlive())
									if (cobalt->GetHealth() >= damage)
										cobalt->SetHealth(cobalt->GetHealth() - damage);
									else cobalt->Kill(jasper);

								if (jade->isAlive())
									if (jade->GetHealth() >= damage)
										jade->SetHealth(jade->GetHealth() - damage);
									else jade->Kill(jasper);

								if (jasper->isAlive())
									if (jasper->GetHealth() >= damage)
										jasper->SetHealth(jasper->GetHealth() - damage);
									else jasper->Kill(jasper);
							}
				}
			}
			
            void EnterCombat(Unit* /*who*/)
            {
				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				
				events.ScheduleEvent(EVENT_REND_FLESH, 5*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_1, 3*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_2, 3475);
				events.ScheduleEvent(EVENT_BERSERK, 5*MINUTE*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_AMETHYST_POOL, 9*IN_MILLISECONDS);

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

				if (instance)
				{
					if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
						cobalt->SetInCombatWithZone();
					if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
						jade->SetInCombatWithZone();
					if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
						jasper->SetInCombatWithZone();

					if (!me->isInCombat())
						me->SetInCombatWithZone();
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
				if (instance)
				{
					if (!solidStone)
						if (!me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_AMETHYST_TRUE_FORM))
								me->RemoveAurasDueToSpell(SPELL_AMETHYST_TRUE_FORM, me->GetGUID());

							me->CastSpell(me, SPELL_SOLID_STONE);
							solidStone = true;
						}
				
					if (solidStone)
						if (me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_SOLID_STONE))
								me->RemoveAurasDueToSpell(SPELL_SOLID_STONE, me->GetGUID());

							me->CastSpell(me, SPELL_AMETHYST_TRUE_FORM, true);
							solidStone = false;
						}

					if (map)
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();

						if (!PlayerList.isEmpty())
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (player->GetPower(POWER_ALTERNATE_POWER) == 100 && !player->HasAura(SPELL_TOTALY_PETRIFIED))
										player->CastSpell(player, SPELL_TOTALY_PETRIFIED);
					}
					
					if (me->GetPower(POWER_ENERGY) == 100)
					{
						me->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);

						if (map)
						{
							Map::PlayerList const &PlayerList = map->GetPlayers();

							if (!PlayerList.isEmpty())
								for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
									if (Player* player = i->getSource())
									{
										player->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION_BAR);
										player->SetPower(POWER_ALTERNATE_POWER, 0);
									}
						}

						if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
							tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

						me->CastSpell(me, SPELL_AMETHYST_OVERLOAD);
						me->SetPower(POWER_ENERGY, 0);
					}

					while (uint32 eventId = events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_REND_FLESH:
								me->CastSpell(me->getVictim(), SPELL_REND_FLESH);

								events.ScheduleEvent(EVENT_REND_FLESH, 6*IN_MILLISECONDS);
								break;

							case EVENT_PETRIFICATION_SET:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
											{
												player->CastSpell(player, SPELL_AMETHYST_PETRIFICATION_BAR);
												player->SetMaxPower(POWER_ALTERNATE_POWER, 100);
												player->SetPower(POWER_ALTERNATE_POWER, 0);
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
											}
								}

								me->CastSpell(me, SPELL_AMETHYST_PETRIFICATION, true);

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_SET);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_1:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_2, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_2:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_3, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_3:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);
								break;
							}
							
							case EVENT_INCREASE_POWER_1:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 1);

								events.ScheduleEvent(EVENT_INCREASE_POWER_1, 1250);
								break;

							case EVENT_INCREASE_POWER_2:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 2);

								events.ScheduleEvent(EVENT_INCREASE_POWER_2, 1725);
								break;

							case EVENT_BERSERK:
								me->CastSpell(me, SPELL_BERSERK, true);

								events.CancelEvent(EVENT_BERSERK);
								break;
								
							case EVENT_AMETHYST_POOL:
								if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
									me->CastSpell(target, SPELL_AMETHYST_POOL, true);

								events.ScheduleEvent(EVENT_AMETHYST_POOL, 9*IN_MILLISECONDS);
								break;

							default:
								break;
						}
					}
				}

                DoMeleeAttackIfReady();
            }
        };
};

class boss_cobalt_guardian : public CreatureScript
{
    public:
        boss_cobalt_guardian() : CreatureScript("boss_cobalt_guardian") { }

		CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_cobalt_guardianAI(creature);
        }

        struct boss_cobalt_guardianAI : public ScriptedAI
        {
            boss_cobalt_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Map* map;
			bool solidStone;

            void Reset()
            {
				events.Reset();
				solidStone = false;
				map = me->GetMap();
				
				me->setPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
				me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT, true);
				me->CastSpell(me, SPELL_ZERO_POWER);
            }
			
			void JustReachedHome()
            {
				me->CastSpell(me, SPELL_ANIM_SIT, true);
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_PETRIFICATION_BAR:
						events.ScheduleEvent(EVENT_PETRIFICATION_SET, 0);
						break;
				}
			}

			void EnterEvadeMode()
			{
				ScriptedAI::EnterEvadeMode();

				if (map)
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
							{
								player->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_TOTALY_PETRIFIED);
								player->SetPower(POWER_ALTERNATE_POWER, 0);
							}
				}

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_TRACKER_RESET);
			}

			void DamageTaken(Unit* who, uint32& damage)
			{
				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
							if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
							{
								if (amethyst->isAlive())
									if (amethyst->GetHealth() >= damage)
										amethyst->SetHealth(amethyst->GetHealth() - damage);
									else amethyst->Kill(amethyst);

								if (jade->isAlive())
									if (jade->GetHealth() >= damage)
										jade->SetHealth(jade->GetHealth() - damage);
									else jade->Kill(jade);

								if (jasper->isAlive())
									if (jasper->GetHealth() >= damage)
										jasper->SetHealth(jasper->GetHealth() - damage);
									else jasper->Kill(jasper);
							}
				}
			}
			
            void EnterCombat(Unit* /*who*/)
            {
				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				
				events.ScheduleEvent(EVENT_REND_FLESH, 5*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_1, 3*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_2, 3475);
				events.ScheduleEvent(EVENT_BERSERK, 5*MINUTE*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_COBALT_MINE, 9*IN_MILLISECONDS);

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						amethyst->SetInCombatWithZone();
					if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
						jade->SetInCombatWithZone();
					if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
						jasper->SetInCombatWithZone();

					if (!me->isInCombat())
						me->SetInCombatWithZone();
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
				if (instance)
				{
					if (!solidStone)
						if (!me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_COBALT_TRUE_FORM))
								me->RemoveAurasDueToSpell(SPELL_COBALT_TRUE_FORM, me->GetGUID());

							me->CastSpell(me, SPELL_SOLID_STONE);
							solidStone = true;
						}
				
					if (solidStone)
						if (me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_SOLID_STONE))
								me->RemoveAurasDueToSpell(SPELL_SOLID_STONE, me->GetGUID());

							me->CastSpell(me, SPELL_COBALT_TRUE_FORM, true);
							solidStone = false;
						}

					if (map)
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();

						if (!PlayerList.isEmpty())
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (player->GetPower(POWER_ALTERNATE_POWER) == 100 && !player->HasAura(SPELL_TOTALY_PETRIFIED))
										player->CastSpell(player, SPELL_TOTALY_PETRIFIED);
					}
					
					if (me->GetPower(POWER_ENERGY) == 100)
					{
						me->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);
						
						if (map)
						{
							Map::PlayerList const &PlayerList = map->GetPlayers();

							if (!PlayerList.isEmpty())
								for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
									if (Player* player = i->getSource())
									{
										player->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION_BAR);
										player->SetPower(POWER_ALTERNATE_POWER, 0);
									}
						}
						
						if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
							tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

						me->CastSpell(me, SPELL_COBALT_OVERLOAD);
						me->SetPower(POWER_ENERGY, 0);
					}

					while (uint32 eventId = events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_REND_FLESH:
								me->CastSpell(me->getVictim(), SPELL_REND_FLESH);

								events.ScheduleEvent(EVENT_REND_FLESH, 6*IN_MILLISECONDS);
								break;

							case EVENT_PETRIFICATION_SET:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
											{
												player->CastSpell(player, SPELL_COBALT_PETRIFICATION_BAR);
												player->SetMaxPower(POWER_ALTERNATE_POWER, 100);
												player->SetPower(POWER_ALTERNATE_POWER, 0);
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
											}
								}

								me->CastSpell(me, SPELL_COBALT_PETRIFICATION, true);

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_SET);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_1:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_2, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_2:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_3, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_3:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);
								break;
							}
							
							case EVENT_INCREASE_POWER_1:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 1);

								events.ScheduleEvent(EVENT_INCREASE_POWER_1, 1250);
								break;

							case EVENT_INCREASE_POWER_2:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 2);

								events.ScheduleEvent(EVENT_INCREASE_POWER_2, 1725);
								break;
								
							case EVENT_BERSERK:
								me->CastSpell(me, SPELL_BERSERK, true);

								events.CancelEvent(EVENT_BERSERK);
								break;

							case EVENT_COBALT_MINE:
								if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
									me->CastSpell(target, SPELL_COBALT_MINE, true);

								events.ScheduleEvent(EVENT_COBALT_MINE, 9*IN_MILLISECONDS);
								break;

							default:
								break;
						}
					}
				}

                DoMeleeAttackIfReady();
            }
        };
};

class boss_jade_guardian : public CreatureScript
{
    public:
        boss_jade_guardian() : CreatureScript("boss_jade_guardian") { }

		CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jade_guardianAI(creature);
        }

        struct boss_jade_guardianAI : public ScriptedAI
        {
            boss_jade_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Map* map;
			bool solidStone;

            void Reset()
            {
				events.Reset();
				solidStone = false;
				map = me->GetMap();

				me->setPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
				me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT, true);
				me->CastSpell(me, SPELL_ZERO_POWER);
            }
			
			void JustReachedHome()
            {
				me->CastSpell(me, SPELL_ANIM_SIT, true);
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_PETRIFICATION_BAR:
						events.ScheduleEvent(EVENT_PETRIFICATION_SET, 0);
						break;
				}
			}

			void EnterEvadeMode()
			{
				ScriptedAI::EnterEvadeMode();

				if (map)
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
							{
								player->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_TOTALY_PETRIFIED);
								player->SetPower(POWER_ALTERNATE_POWER, 0);
							}
				}

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_TRACKER_RESET);
			}

			void DamageTaken(Unit* who, uint32& damage)
			{
				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
							if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
							{
								if (amethyst->isAlive())
									if (amethyst->GetHealth() >= damage)
										amethyst->SetHealth(amethyst->GetHealth() - damage);
									else amethyst->Kill(jasper);

								if (cobalt->isAlive())
									if (cobalt->GetHealth() >= damage)
										cobalt->SetHealth(cobalt->GetHealth() - damage);
									else cobalt->Kill(jasper);

								if (jasper->isAlive())
									if (jasper->GetHealth() >= damage)
										jasper->SetHealth(jasper->GetHealth() - damage);
									else jasper->Kill(jasper);
							}
				}
			}
			
            void EnterCombat(Unit* /*who*/)
            {
				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				
				events.ScheduleEvent(EVENT_REND_FLESH, 5*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_1, 3*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_2, 3475);
				events.ScheduleEvent(EVENT_BERSERK, 5*MINUTE*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_JADE_SHARDS, 9*IN_MILLISECONDS);

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						amethyst->SetInCombatWithZone();
					if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
						cobalt->SetInCombatWithZone();
					if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
						jasper->SetInCombatWithZone();

					if (!me->isInCombat())
						me->SetInCombatWithZone();
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
				if (instance)
				{
					if (!solidStone)
						if (!me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_JADE_TRUE_FORM))
								me->RemoveAurasDueToSpell(SPELL_JADE_TRUE_FORM, me->GetGUID());

							me->CastSpell(me, SPELL_SOLID_STONE);
							solidStone = true;
						}
				
					if (solidStone)
						if (me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JASPER_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_SOLID_STONE))
								me->RemoveAurasDueToSpell(SPELL_SOLID_STONE, me->GetGUID());

							me->CastSpell(me, SPELL_JADE_TRUE_FORM, true);
							solidStone = false;
						}

					if (map)
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();

						if (!PlayerList.isEmpty())
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (player->GetPower(POWER_ALTERNATE_POWER) == 100 && !player->HasAura(SPELL_TOTALY_PETRIFIED))
										player->CastSpell(player, SPELL_TOTALY_PETRIFIED);
					}
					
					if (me->GetPower(POWER_ENERGY) == 100)
					{
						me->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);

						if (map)
						{
							Map::PlayerList const &PlayerList = map->GetPlayers();

							if (!PlayerList.isEmpty())
								for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
									if (Player* player = i->getSource())
									{
										player->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION_BAR);
										player->SetPower(POWER_ALTERNATE_POWER, 0);
									}
						}
						
						if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
							tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

						me->CastSpell(me, SPELL_JADE_OVERLOAD);
						me->SetPower(POWER_ENERGY, 0);
					}

					while (uint32 eventId = events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_REND_FLESH:
								me->CastSpell(me->getVictim(), SPELL_REND_FLESH);

								events.ScheduleEvent(EVENT_REND_FLESH, 6*IN_MILLISECONDS);
								break;

							case EVENT_PETRIFICATION_SET:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
											{
												player->CastSpell(player, SPELL_JADE_PETRIFICATION_BAR);
												player->SetMaxPower(POWER_ALTERNATE_POWER, 100);
												player->SetPower(POWER_ALTERNATE_POWER, 0);
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
											}
								}

								me->CastSpell(me, SPELL_JADE_PETRIFICATION, true);

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_SET);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_1:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_2, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_2:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_3, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_3:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);
								break;
							}
							
							case EVENT_INCREASE_POWER_1:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 1);

								events.ScheduleEvent(EVENT_INCREASE_POWER_1, 1250);
								break;

							case EVENT_INCREASE_POWER_2:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 2);

								events.ScheduleEvent(EVENT_INCREASE_POWER_2, 1725);
								break;
								
							case EVENT_BERSERK:
								me->CastSpell(me, SPELL_BERSERK, true);

								events.CancelEvent(EVENT_BERSERK);
								break;

							case EVENT_JADE_SHARDS:
								me->CastSpell(me, SPELL_JADE_SHARDS, true);

								events.ScheduleEvent(EVENT_JADE_SHARDS, 9*IN_MILLISECONDS);
								break;

							default:
								break;
						}
					}
				}

                DoMeleeAttackIfReady();
            }
        };
};

class boss_jasper_guardian : public CreatureScript
{
    public:
        boss_jasper_guardian() : CreatureScript("boss_jasper_guardian") { }

		CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jasper_guardianAI(creature);
        }

        struct boss_jasper_guardianAI : public ScriptedAI
        {
            boss_jasper_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			EventMap events;
			Map* map;
			bool solidStone;

            void Reset()
            {
				events.Reset();
				solidStone = false;
				map = me->GetMap();

				me->setPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
				me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
				me->CastSpell(me, SPELL_STONE_VISUAL);
				me->CastSpell(me, SPELL_ANIM_SIT, true);
				me->CastSpell(me, SPELL_ZERO_POWER);
            }
			
			void JustReachedHome()
            {
				me->CastSpell(me, SPELL_ANIM_SIT, true);
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_PETRIFICATION_BAR:
						events.ScheduleEvent(EVENT_PETRIFICATION_SET, 0);
						break;
				}
			}

			void EnterEvadeMode()
			{
				ScriptedAI::EnterEvadeMode();

				if (map)
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
							{
								player->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION_BAR);
								player->RemoveAurasDueToSpell(SPELL_TOTALY_PETRIFIED);
								player->SetPower(POWER_ALTERNATE_POWER, 0);
							}
				}

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_TRACKER_RESET);
			}

			void DamageTaken(Unit* who, uint32& damage)
			{
				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
							if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
							{
								if (amethyst->isAlive())
									if (amethyst->GetHealth() >= damage)
										amethyst->SetHealth(amethyst->GetHealth() - damage);
									else amethyst->Kill(amethyst);

								if (cobalt->isAlive())
									if (cobalt->GetHealth() >= damage)
										cobalt->SetHealth(cobalt->GetHealth() - damage);
									else cobalt->Kill(cobalt);

								if (jade->isAlive())
									if (jade->GetHealth() >= damage)
										jade->SetHealth(jade->GetHealth() - damage);
									else jade->Kill(jade);
							}
				}
			}

			void EnterCombat(Unit* /*who*/)
            {
				me->RemoveAurasDueToSpell(SPELL_STONE_VISUAL, me->GetGUID());
				
				events.ScheduleEvent(EVENT_REND_FLESH, 5*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_1, 3*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_INCREASE_POWER_2, 3475);
				events.ScheduleEvent(EVENT_BERSERK, 5*MINUTE*IN_MILLISECONDS);

				if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
					tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

				if (instance)
				{
					if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
						amethyst->SetInCombatWithZone();
					if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
						cobalt->SetInCombatWithZone();
					if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
						jade->SetInCombatWithZone();

					if (!me->isInCombat())
						me->SetInCombatWithZone();
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
                
				if (instance)
				{
					if (!solidStone)
						if (!me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) &&
							!me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_JASPER_TRUE_FORM))
								me->RemoveAurasDueToSpell(SPELL_JASPER_TRUE_FORM, me->GetGUID());

							me->CastSpell(me, SPELL_SOLID_STONE);
							solidStone = true;
						}
				
					if (solidStone)
						if (me->FindNearestCreature(BOSS_AMETHYST_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_COBALT_GUARDIAN, 10.0f, true) ||
							me->FindNearestCreature(BOSS_JADE_GUARDIAN, 10.0f, true))
						{
							if (me->HasAura(SPELL_SOLID_STONE))
								me->RemoveAurasDueToSpell(SPELL_SOLID_STONE, me->GetGUID());

							me->CastSpell(me, SPELL_JASPER_TRUE_FORM, true);
							solidStone = false;
						}

					if (map)
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();

						if (!PlayerList.isEmpty())
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (player->GetPower(POWER_ALTERNATE_POWER) == 100 && !player->HasAura(SPELL_TOTALY_PETRIFIED))
										player->CastSpell(player, SPELL_TOTALY_PETRIFIED);
					}

					if (me->GetPower(POWER_ENERGY) == 100)
					{
						me->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
						events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);

						if (map)
						{
							Map::PlayerList const &PlayerList = map->GetPlayers();

							if (!PlayerList.isEmpty())
								for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
									if (Player* player = i->getSource())
									{
										player->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION_BAR);
										player->SetPower(POWER_ALTERNATE_POWER, 0);
									}
						}
						
						if (Creature* tracker = me->FindNearestCreature(NPC_THE_STONE_GUARD_TRACKER, 99999.0f, true))
							tracker->AI()->DoAction(ACTION_CHOOSE_PETRIFICATION);

						me->CastSpell(me, SPELL_JASPER_OVERLOAD);
						me->SetPower(POWER_ENERGY, 0);
					}

					while (uint32 eventId = events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_REND_FLESH:
								me->CastSpell(me->getVictim(), SPELL_REND_FLESH);

								events.ScheduleEvent(EVENT_REND_FLESH, 6*IN_MILLISECONDS);
								break;

							case EVENT_PETRIFICATION_SET:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
											{
												player->CastSpell(player, SPELL_JASPER_PETRIFICATION_BAR);
												player->SetMaxPower(POWER_ALTERNATE_POWER, 100);
												player->SetPower(POWER_ALTERNATE_POWER, 0);
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
											}
								}

								me->CastSpell(me, SPELL_JASPER_PETRIFICATION, true);

								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_SET);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_1:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_2, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_1);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_2:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												player->ModifyPower(POWER_ALTERNATE_POWER, 5);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_3, 3*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_2);
								break;
							}

							case EVENT_PETRIFICATION_INCREASE_3:
							{
								if (map)
								{
									Map::PlayerList const &PlayerList = map->GetPlayers();

									if (!PlayerList.isEmpty())
										for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
											if (Player* player = i->getSource())
												me->ModifyPower(POWER_ENERGY, 2);
								}
								
								events.ScheduleEvent(EVENT_PETRIFICATION_INCREASE_1, 7*IN_MILLISECONDS);
								events.CancelEvent(EVENT_PETRIFICATION_INCREASE_3);
								break;
							}
							
							case EVENT_INCREASE_POWER_1:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 1);

								events.ScheduleEvent(EVENT_INCREASE_POWER_1, 1350);
								break;

							case EVENT_INCREASE_POWER_2:
								if (!solidStone)
									me->ModifyPower(POWER_ENERGY, 2);

								events.ScheduleEvent(EVENT_INCREASE_POWER_2, 1750);
								break;
								
							case EVENT_BERSERK:
								me->CastSpell(me, SPELL_BERSERK, true);

								events.CancelEvent(EVENT_BERSERK);
								break;

							default:
								break;
						}
					}
				}

                DoMeleeAttackIfReady();
            }
        };
};

class npc_the_stone_guard_tracker : public CreatureScript
{
public:
	npc_the_stone_guard_tracker() : CreatureScript("npc_the_stone_guard_tracker") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_the_stone_guard_trackerAI(creature);
	}

	struct npc_the_stone_guard_trackerAI : public ScriptedAI
	{
		npc_the_stone_guard_trackerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		uint8 lastGuardian;
		bool choiceDone;
		
		void Reset()
        {
			events.Reset();
			lastGuardian = 0;
			choiceDone = false;
        }

		void DoAction(const int32 action)
        {
            switch (action)
            {
				case ACTION_CHOOSE_PETRIFICATION:
				{
					if (!choiceDone)
					{
						if (IsAmethystEligible() && IsCobaltEligible() && IsJadeEligible() && IsJasperEligible())
							lastGuardian = RAND(AMETHYST, COBALT, JADE, JASPER);

						else if (!IsAmethystEligible() && !IsCobaltEligible() && !IsJadeEligible() && IsJasperEligible())
							lastGuardian = JASPER;

						else if (!IsAmethystEligible() && !IsCobaltEligible() && IsJadeEligible() && !IsJasperEligible())
							lastGuardian = JADE;

						else if (!IsAmethystEligible() && IsCobaltEligible() && !IsJadeEligible() && !IsJasperEligible())
							lastGuardian = COBALT;

						else if (IsAmethystEligible() && !IsCobaltEligible() && !IsJadeEligible() && !IsJasperEligible())
							lastGuardian = AMETHYST;

						else if (!IsAmethystEligible() && !IsCobaltEligible() && IsJadeEligible() && IsJasperEligible())
							lastGuardian = RAND(JADE, JASPER);

						else if (!IsAmethystEligible() && IsCobaltEligible() && IsJadeEligible() && !IsJasperEligible())
							lastGuardian = RAND(COBALT, JADE);

						else if (IsAmethystEligible() && IsCobaltEligible() && !IsJadeEligible() && !IsJasperEligible())
							lastGuardian = RAND(AMETHYST, COBALT);

						else if (!IsAmethystEligible() && IsCobaltEligible() && IsJadeEligible() && IsJasperEligible())
							lastGuardian = RAND(COBALT, JADE, JASPER);

						else if (IsAmethystEligible() && !IsCobaltEligible() && IsJadeEligible() && IsJasperEligible())
							lastGuardian = RAND(AMETHYST, JADE, JASPER);

						else if (IsAmethystEligible() && IsCobaltEligible() && !IsJadeEligible() && IsJasperEligible())
							lastGuardian =RAND(AMETHYST, COBALT, JASPER);

						else if (IsAmethystEligible() && IsCobaltEligible() && IsJadeEligible() && !IsJasperEligible())
							lastGuardian = RAND(AMETHYST, COBALT, JADE);

						events.ScheduleEvent(EVENT_CHOOSE_PETRIFICATION, 6*IN_MILLISECONDS);
						choiceDone = true;
					}
					break;
				}

				case ACTION_TRACKER_RESET:
				{
					events.Reset();
					lastGuardian = 0;
					choiceDone = false;
					break;
				}
			}
		}

		bool IsAmethystEligible()
		{
			if (instance)
			{
				if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
				{
					if (!amethyst->isAlive() || lastGuardian == AMETHYST)
						return false;
				}
				else return false;
			}
			else return false;

			return true;
		}

		bool IsCobaltEligible()
		{
			if (instance)
			{
				if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
				{
					if (!cobalt->isAlive() || lastGuardian == COBALT)
						return false;
				}
				else return false;
			}
			else return false;

			return true;
		}

		bool IsJadeEligible()
		{
			if (instance)
			{
				if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
				{
					if (!jade->isAlive() || lastGuardian == JADE)
						return false;
				}
				else return false;
			}
			else return false;

			return true;
		}

		bool IsJasperEligible()
		{
			if (instance)
			{
				if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
				{
					if (!jasper->isAlive() || lastGuardian == JASPER)
						return false;
				}
				else return false;
			}
			else return false;

			return true;
		}

		void UpdateAI(const uint32 diff)
		{
			events.Update(diff);

			if (instance)
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_CHOOSE_PETRIFICATION:
						{
							if (lastGuardian == AMETHYST)
								if (Creature* amethyst = me->GetCreature(*me, instance->GetData64(DATA_AMETHYST_GUARDIAN)))
									amethyst->AI()->DoAction(ACTION_PETRIFICATION_BAR);

							if (lastGuardian == COBALT)
								if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
									cobalt->AI()->DoAction(ACTION_PETRIFICATION_BAR);

							if (lastGuardian == JADE)
								if (Creature* jade = me->GetCreature(*me, instance->GetData64(DATA_JADE_GUARDIAN)))
									jade->AI()->DoAction(ACTION_PETRIFICATION_BAR);

							if (lastGuardian == JASPER)
								if (Creature* jasper = me->GetCreature(*me, instance->GetData64(DATA_JASPER_GUARDIAN)))
									jasper->AI()->DoAction(ACTION_PETRIFICATION_BAR);

							choiceDone = false;
							events.CancelEvent(EVENT_CHOOSE_PETRIFICATION);
							break;
						}

						default:
							break;
					}
				}
			}
		}
	};
};

class npc_cobalt_mine : public CreatureScript
{
public:
	npc_cobalt_mine() : CreatureScript("npc_cobalt_mine") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_cobalt_mineAI(creature);
	}

	struct npc_cobalt_mineAI : public ScriptedAI
	{
		npc_cobalt_mineAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		bool canExplode;

		void Reset()
        {
			events.Reset();
			canExplode = false;

			me->setFaction(14);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

			me->CastSpell(me, SPELL_COBALT_MINE_VISUAL, true);
			me->DespawnOrUnsummon(90*IN_MILLISECONDS); // 10 cobalt mines not more
			events.ScheduleEvent(EVENT_COBALT_MINE_ACTIVATION, 3*IN_MILLISECONDS);
        }

		void UpdateAI(const uint32 diff)
		{
			events.Update(diff);

			if (Creature* cobalt = me->GetCreature(*me, instance->GetData64(DATA_COBALT_GUARDIAN)))
				if (!cobalt->isInCombat())
					me->DespawnOrUnsummon();

			if (canExplode)
			{
				if (Map* map = me->GetMap())
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
								if (player->GetExactDist2d(me->GetPositionX(), me->GetPositionY()) < 7.0f)
								{
									me->CastSpell(me, SPELL_COBALT_MINE_EXPLOSION, true);
									me->DespawnOrUnsummon();
								}
				}
			}

			if (instance)
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_COBALT_MINE_ACTIVATION:
							canExplode = true;

							events.CancelEvent(EVENT_COBALT_MINE_ACTIVATION);
							break;

						default:
							break;
					}
				}
			}
		}
	};
};

void AddSC_boss_the_stone_guard()
{
    new boss_amethyst_guardian();
	new boss_cobalt_guardian();
	new boss_jade_guardian();
	new boss_jasper_guardian();
	new npc_the_stone_guard_tracker();
	new npc_cobalt_mine();
}