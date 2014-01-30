/* # Script de Sungis : Mogu'shan Vaults # */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "mogushan_vaults.h"

class instance_mogushan_vaults : public InstanceMapScript
{
	public:
		instance_mogushan_vaults() : InstanceMapScript("instance_mogushan_vaults", 1008) { }

		struct instance_mogushan_vaults_InstanceMapScript : public InstanceScript
		{
			instance_mogushan_vaults_InstanceMapScript(Map* map) : InstanceScript(map)
			{
				BossAmethystGuardianGUID		= 0;
				BossCobaltGuardianGUID			= 0;
				BossJadeGuardianGUID			= 0;
				BossJasperGuardianGUID			= 0;
				BossFengTheAccursedGUID			= 0;
				BossGarajalTheSpiritBinderGUID	= 0;
				BossZianGUID					= 0;
				BossMengGUID					= 0;
				BossQiangGUID					= 0;
				BossSubetaiGUID					= 0;
				BossElegonGUID					= 0;
				BossJanXiGUID					= 0;
				BossQinXiGUID					= 0;
			}

			void OnCreatureCreate(Creature* creature)
			{
				switch (creature->GetEntry())
				{
					case BOSS_AMETHYST_GUARDIAN:
						BossAmethystGuardianGUID = creature->GetGUID();
						break;
					case BOSS_COBALT_GUARDIAN:
						BossCobaltGuardianGUID = creature->GetGUID();
						break;
					case BOSS_JADE_GUARDIAN:
						BossJadeGuardianGUID = creature->GetGUID();
						break;
					case BOSS_JASPER_GUARDIAN:
						BossJasperGuardianGUID = creature->GetGUID();
						break;
					case BOSS_FENG_THE_ACCURSED:
						BossFengTheAccursedGUID = creature->GetGUID();
						break;
					case BOSS_GARAJAL_THE_SPIRITBINDER:
						BossGarajalTheSpiritBinderGUID = creature->GetGUID();
						break;
					case BOSS_ZIAN:
						BossZianGUID = creature->GetGUID();
						break;
					case BOSS_MENG:
						BossMengGUID = creature->GetGUID();
						break;
					case BOSS_QIANG:
						BossQiangGUID = creature->GetGUID();
						break;
					case BOSS_SUBETAI:
						BossSubetaiGUID = creature->GetGUID();
						break;
					case BOSS_ELEGON:
						BossElegonGUID = creature->GetGUID();
						break;
					case BOSS_JAN_XI:
						BossJanXiGUID = creature->GetGUID();
						break;
					case BOSS_QIN_XI:
						BossQinXiGUID = creature->GetGUID();
						break;
					default:
						break;
				}
			}

			uint64 GetData64(uint32 id) const
			{
				switch (id)
				{
					case DATA_AMETHYST_GUARDIAN:
						return BossAmethystGuardianGUID;
					case DATA_COBALT_GUARDIAN:
						return BossCobaltGuardianGUID;
					case DATA_JADE_GUARDIAN:
						return BossJadeGuardianGUID;
					case DATA_JASPER_GUARDIAN:
						return BossJasperGuardianGUID;
					case DATA_FENG_THE_ACCURSED:
						return BossFengTheAccursedGUID;
					case DATA_GARAJAL_THE_SPIRITBINDER:
						return BossGarajalTheSpiritBinderGUID;
					case DATA_ZIAN:
						return BossZianGUID;
					case DATA_MENG:
						return BossMengGUID;
					case DATA_QIANG:
						return BossQiangGUID;
					case DATA_SUBETAI:
						return BossSubetaiGUID;
					case DATA_ELEGON:
						return BossElegonGUID;
					case DATA_JAN_XI:
						return BossJanXiGUID;
					case DATA_QIN_XI:
						return BossQinXiGUID;
					default:
						break;
				}

				return 0;
			}

			protected:
				uint64 BossAmethystGuardianGUID;
				uint64 BossCobaltGuardianGUID;
				uint64 BossJadeGuardianGUID;
				uint64 BossJasperGuardianGUID;
				uint64 BossFengTheAccursedGUID;
				uint64 BossGarajalTheSpiritBinderGUID;
				uint64 BossZianGUID;
				uint64 BossMengGUID;
				uint64 BossQiangGUID;
				uint64 BossSubetaiGUID;
				uint64 BossElegonGUID;
				uint64 BossJanXiGUID;
				uint64 BossQinXiGUID;
		};

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_mogushan_vaults_InstanceMapScript(map);
        }
};

void AddSC_instance_mogushan_vaults()
{
    new instance_mogushan_vaults();
}
