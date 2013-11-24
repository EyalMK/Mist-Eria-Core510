/* # Script de Tydrheal & Sungis : Temple of the Jade Serpent # */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "temple_of_the_jade_serpent.h"

/* Notes : What is missing ? : - ...
*/


class instance_temple_of_the_jade_serpent : public InstanceMapScript
{
	public:
		instance_temple_of_the_jade_serpent() : InstanceMapScript("instance_temple_of_the_jade_serpent", 960) { }

		struct instance_temple_of_the_jade_serpent_InstanceMapScript : public InstanceScript
		{
			instance_temple_of_the_jade_serpent_InstanceMapScript(Map* map) : InstanceScript(map)
			{
				BossWiseMariGUID			= 0;
				WiseMariGateGUID			= 0;
				BossLorewalkerStonestepGUID	= 0;
				NpcZaoSunseekerGUID			= 0;
				NpcCorruptedScrollGUID		= 0;
				NpcSun1GUID					= 0;
				NpcSun2GUID					= 0;
				NpcSun3GUID					= 0;
				NpcSun4GUID					= 0;
				NpcSun5GUID					= 0;
				BossLiuFlameheartGUID		= 0;
				BossShaOfDoubtGUID			= 0;
			}

			void OnCreatureCreate(Creature* creature)
			{
				switch (creature->GetEntry())
				{
					case BOSS_WISE_MARI:
						BossWiseMariGUID = creature->GetGUID();
						break;

					case BOSS_LOREWALKER_STONESTEP:
						BossLorewalkerStonestepGUID = creature->GetGUID();
						break;

					case NPC_ZAO_SUNSEEKER:
						NpcZaoSunseekerGUID = creature->GetGUID();
						break;

					case NPC_CORRUPTED_SCROLL:
						NpcCorruptedScrollGUID = creature->GetGUID();
						
					case BOSS_LIU_FLAMEHEART:
						BossLiuFlameheartGUID = creature->GetGUID();
						break;
						
					case BOSS_SHA_OF_DOUBT:
						BossShaOfDoubtGUID = creature->GetGUID();
						break;
		
					default:
						break;
				}

				switch (creature->GetGUID())
				{
					case NPC_SUN_1:
						NpcSun1GUID = creature->GetGUID();
						break;

					case NPC_SUN_2:
						NpcSun2GUID = creature->GetGUID();
						break;

					case NPC_SUN_3:
						NpcSun3GUID = creature->GetGUID();
						break;

					case NPC_SUN_4:
						NpcSun4GUID = creature->GetGUID();
						break;

					case NPC_SUN_5:
						NpcSun5GUID = creature->GetGUID();
						break;
		
					default:
						break;
				}
			}
			
			void OnGameObjectCreate(GameObject* go) 
            {
                switch (go->GetEntry())
                {
					case GO_WISE_MARI_GATE:
						WiseMariGateGUID = go->GetGUID();
						break;
				}
			}
			
			void OnGameObjectRemove(GameObject* go) 
            {
                switch (go->GetEntry())
                {
				}
			}

			uint64 GetData64(uint32 id) const 
			{
				switch (id)
				{
					case DATA_BOSS_WISE_MARI:
						return BossWiseMariGUID;
					
					case DATA_WISE_MARI_GATE:
						return WiseMariGateGUID;

					case DATA_BOSS_LOREWALKER_STONESTEP:
						return BossLorewalkerStonestepGUID;

					case DATA_NPC_ZAO_SUNSEEKER:
						return NpcZaoSunseekerGUID;

					case DATA_NPC_CORRUPTED_SCROLL:
						return NpcCorruptedScrollGUID;

					case DATA_NPC_SUN_1:
						return NpcSun1GUID;

					case DATA_NPC_SUN_2:
						return NpcSun2GUID;

					case DATA_NPC_SUN_3:
						return NpcSun3GUID;

					case DATA_NPC_SUN_4:
						return NpcSun4GUID;

					case DATA_NPC_SUN_5:
						return NpcSun5GUID;
					
					case DATA_BOSS_LIU_FLAMEHEART:
						return BossLiuFlameheartGUID;
						
					case DATA_BOSS_SHA_OF_DOUBT:
						return BossShaOfDoubtGUID;
						
					default:
						break;
				}

				return 0;
			}

			protected:
				uint64 BossWiseMariGUID;
				uint64 WiseMariGateGUID;
				uint64 BossLorewalkerStonestepGUID;
				uint64 NpcZaoSunseekerGUID;
				uint64 NpcCorruptedScrollGUID;
				uint64 NpcSun1GUID;
				uint64 NpcSun2GUID;
				uint64 NpcSun3GUID;
				uint64 NpcSun4GUID;
				uint64 NpcSun5GUID;
				uint64 BossLiuFlameheartGUID;
				uint64 BossShaOfDoubtGUID;

		};

		InstanceScript* GetInstanceScript(InstanceMap* map) const 
		{
			return new instance_temple_of_the_jade_serpent_InstanceMapScript(map);
		}
};


void AddSC_instance_temple_of_the_jade_serpent()
{
   new instance_temple_of_the_jade_serpent();
}

