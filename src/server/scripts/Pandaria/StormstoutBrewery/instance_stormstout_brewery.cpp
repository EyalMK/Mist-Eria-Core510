#include "stormstout_brewery.h"
#include "InstanceScript.h"

// When we initialize (Initialize() )the instance, we randomize here ;
// After that, when OnCreatureCreate() is called, we update the entries if needed
// Also, we must set the datas properly, so YanZhu will have the good spells

// First entry is the basic entry (arbitrary chosen), the second entry is the eventually updated entry
static const uint32 AlamentalsCombination[3][2] =
{
    {Mob_FizzyBrewAlamental, Mob_SudsyBrewAlamental}, // Stout ability
    {Mob_BubblingBrewAlamental, Mob_YeasyBrewAlamental}, // Ale ability
    {Mob_BloatedBrewAlamental, Mob_StoutBrewAlamental} // Wheat ability
};

class instance_stormstout_brewery : public InstanceMapScript
{
public :
    instance_stormstout_brewery() : InstanceMapScript("instance_stormstout_brewery", 908)
    {

    }

    class instance_stormstout_brewery_InstanceScript : public InstanceScript
    {
    public :
        // Initialization
        instance_stormstout_brewery_InstanceScript(Map* map) : InstanceScript(map)
        {
            // Nowadays, everybody prefers the constructor instead of Initialize() ;
            // but, since we still call Initialize(), it would be dumb not to use it
        }

        void Initialize()
        {
            memset(m_auiEncounter, NOT_STARTED, sizeof(m_auiEncounter)) ; // Initialize the events states
            memset(m_auiAlamental, 0, sizeof(m_auiAlamental)); // Initialize the npcs

            m_ullHoptallusGuid = m_ullOokOokGuid = m_ullYanZhuGuid = m_ucKilledHozenCounter = 0 ; // Initialize counters

            for(uint8 i = 0 ; i < MAX_ALAMENTAL ; ++i)
                m_auiAlamental[i] = AlamentalsCombination[RAND(0,1)][i];
        }

        /// @note : Tweex said that if the server crashes while a player is in the instance, doors may be closed again, since it is their original status
        /// The idea is to check the array containing the boss status and to ReSet the doors status
        void OnPlayerEnter(Player *pPlayerEntered)
        {
            if(!pPlayerEntered)
                return ;

            if(GetData(Data_BossOokOok) == NOT_STARTED && GetData(Data_OokOokPartyEventProgress) != DONE)
            {
                if(!pPlayerEntered->HasAura(Spell_EnableBananaBar))
                {
                    pPlayerEntered->CastSpell(pPlayerEntered, Spell_EnableBananaBar, true);
                    pPlayerEntered->SetPower(POWER_ALTERNATE_POWER, int32(GetData64(Data_OokOokPartyCounter)));
                }
            }
        }

        // Setting datas
        void SetData(uint32 uiDataIndex, uint32 uiDataValue)
        {
            switch(uiDataIndex)
            {
            case Data_OokOokPartyEventProgress :
                if(uiDataValue == DONE)
                    EndOokOokEvent();
                break ;

            case Data_OokOokEventProgress :
                HandleDoors(uiDataValue, GetData64(GameObject_OokOokEntranceDoor), GetData64(GameObject_OokOokExitDoor));
                if(uiDataValue == DONE)
                    StartHoptallusPreEvent();
                break ;

            case Data_HoptallusEventProgress :
                HandleDoors(uiDataValue, GetData64(GameObject_HoptallusEntranceDoor), GetData64(GameObject_HoptallusExitDoor));
                break ;

            case Data_YanZhuEventProgress :
                HandleDoors(uiDataValue, GetData64(GameObject_YanZhuEntranceDoor), 0);
                break ;

            default :
                break ;
            }

            if(uiDataIndex < MAX_ENCOUNTER)
                m_auiEncounter[uiDataIndex] = uiDataValue;
        }

        void SetData64(uint32 uiDataIndex, uint64 ullDataValue)
        {
            // Normally there is nothing to do here, except in special cases
            switch(uiDataIndex)
            {
            case Data_OokOokPartyCounter :
                if(GetData(Data_OokOokPartyEventProgress) == DONE)
                    return ;
                m_ucKilledHozenCounter += ullDataValue ;
                HandlePower(true, int32(ullDataValue));
                if(m_ucKilledHozenCounter >= 40)
                {
                    SetData(Data_OokOokPartyEventProgress, DONE);
                    HandlePower(false, 0);
                }
                break ;

            default :
                break ;
            }
        }

        // Getting Datas
        uint32 GetData(uint32 uiDataIndex) const
        {
            switch(uiDataIndex)
            {
            case Data_OokOokPartyEventProgress :
            case Data_OokOokEventProgress :
            case Data_HoptallusEventProgress :
            case Data_YanZhuEventProgress :
                return m_auiEncounter[uiDataIndex];

            case Data_AleAlamental :
            case Data_WheatAlamental :
            case Data_StoutAlamental :
                return m_auiAlamental[uiDataIndex - 4];
            default :
                return 0 ;
            }

            // Normally we should have already returned ; be if we forgot or we dit not :
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : GetData() did not return something during the switch ; returning 0 !");
            return 0 ;
        }

        uint64 GetData64(uint32 uiDataIndex) const
        {
            switch(uiDataIndex)
            {
            case Data_OokOokPartyCounter :
                return m_ucKilledHozenCounter ;

            case Data_BossOokOok :
                return m_ullOokOokGuid ;

            case Data_BossHoptallus :
                return m_ullHoptallusGuid ;

            case Data_BossYanZhu :
                return m_ullYanZhuGuid ;

            default :
                return 0 ;
            }

            // Normally we should have already returned ; be if we forgot or we dit not :
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : GetData64() did not return something during the switch ; returning 0 !");
            return 0 ;
        }

        // Create system
        void OnCreatureCreate(Creature *pCreatureCreated)
        {
            if(!pCreatureCreated)
                return ;

            uint64 guid = pCreatureCreated->GetGUID();
            if(!guid)
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : OnCreatureCreate() received a creature (entry %u) with a guid equal to zero",
                               pCreatureCreated->GetEntry());
            }

            switch(pCreatureCreated->GetEntry())
            {
            // Bosses
            case Boss_OokOok :
                m_ullOokOokGuid = guid ;
                break;

            case Boss_Hoptallus :
                m_ullHoptallusGuid = guid ;
                break ;

            case Boss_YanZhutheUncasked :
                m_ullYanZhuGuid = guid ;
                break ;

            default :
                break ;
            }
        }

        void OnGameObjectCreate(GameObject *pGameObjectCreated)
        {
            if(!pGameObjectCreated)
                return ;

            uint64 guid = pGameObjectCreated->GetGUID();
            if(!guid)
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : OnGameObjectCreate received GameObject (entry %u) with guid equal zero !",
                               pGameObjectCreated->GetEntry());

            switch(pGameObjectCreated->GetEntry())
            {
            case GameObject_OokOokEntranceDoor :
                m_ullOokOokEntranceDoorGuid = guid ;
                HandleGameObject(0, true, pGameObjectCreated);
                break ;

            case GameObject_OokOokExitDoor :
                m_ullOokOokExitDoorGuid = guid ;
                if(GetData(Data_OokOokEventProgress) == DONE)
                    HandleGameObject(0, true, pGameObjectCreated);
                else
                    HandleGameObject(0, false, pGameObjectCreated);
                break ;

            case GameObject_HoptallusEntranceDoor :
                m_ullHoptallusEntranceDoorGuid = guid ;
                HandleGameObject(0, true, pGameObjectCreated);
                break ;

            case GameObject_HoptallusExitDoor :
                m_ullHoptallusExitDoorGuid = guid ;
                if(GetData(Data_HoptallusEventProgress) == DONE)
                    HandleGameObject(0, true, pGameObjectCreated);
                else
                    HandleGameObject(0, false, pGameObjectCreated);
                break ;

            case GameObject_YanZhuEntranceDoor :
                m_ullYanZhuEntranceDoorGuid = guid ;
                HandleGameObject(0, true, pGameObjectCreated);
                break ;
            }
        }

        // Remove system
        void OnCreatureRemove(Creature *pCreatureRemoved)
        {
            if(!pCreatureRemoved)
                return ;

            for(uint8 i = 0 ; i < MAX_OOKOOK_ENTOURAGE ; ++i)
            {
                if(pCreatureRemoved->GetEntry() == OokOokEntourage[i])
                {
                    SetData64(Data_OokOokPartyCounter, 1);
                }
            }
        }

        void OnGameObjectRemove(GameObject *pGameObjectRemoved)
        {
            if(!pGameObjectRemoved)
                return ;
        }

        // Load and save
        void Load(const char* savedData)
        {
            if(!savedData)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return ;
            }

            OUT_LOAD_INST_DATA(savedData);

            std::istringstream loadISS(savedData);

            char dataHead1, dataHead2;
            loadISS >> dataHead1 >> dataHead2 ;

            if(dataHead1 != 'S' || dataHead2 != 'B')
                return ;

            for(uint8 i = 0 ; i < MAX_ENCOUNTER ; ++i)
            {
                loadISS >> m_auiEncounter[i];
                if(m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED ;
            }

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveOSS;
            saveOSS << "S B " << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2] << ' ' << m_auiEncounter[3] ;

            OUT_SAVE_INST_DATA_COMPLETE;

            return saveOSS.str();
        }

        // Bananar Bar System
        void UpdatePowerOnPlayer(Player* player, int32 amount)
        {
            if(player)
            {
                // If the player has not the aura, something anormal happened during OnPlayerEnter ; I will not handle the aura here
                if(!player->HasAura(Spell_EnableBananaBar))
                    return ;

                player->ModifyPower(POWER_ALTERNATE_POWER, amount);
            }
        }

        void RemovePowerOnPlayer(Player* player)
        {
            if(player)
            {
                player->RemoveAurasDueToSpell(Spell_EnableBananaBar);
            }
        }

        void HandlePower(bool apply, int32 amount)
        {
            if(apply && !amount)
                return ;

            Map::PlayerList const& playerList = instance->GetPlayers();
            if(!playerList.isEmpty())
            {
                for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                {
                    apply ? UpdatePowerOnPlayer(iter->getSource(), amount) : RemovePowerOnPlayer(iter->getSource());
                }
            }
        }

        // Ook Ook Event
        void EndOokOokEvent()
        {
            Creature* ookOok = instance->GetCreature(m_ullOokOokGuid);
            if(!ookOok)
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : Unable to find OokOok in EndOokOokEvent()");
            }
            std::list<Creature*> ookOokEntourage ;

            // Note : Be carefull with that, perhaps an array of lists would be better
            for(uint8 i = 0 ; i < MAX_OOKOOK_ENTOURAGE ; ++i)
                GetCreatureListWithEntryInGrid(ookOokEntourage, ookOok, OokOokEntourage[i], 50000.0f);

            for(std::list<Creature*>::iterator iter = ookOokEntourage.begin() ; iter != ookOokEntourage.end() ; ++iter)
            {
                if(Creature* c = (*iter))
                {
                    if(c->AI())
                    {
                        c->AI()->DoAction(0);
                    }
                }
            }

            ookOok->AI()->DoAction(0); // Start the ook ook boss event
        }

        // Hoptallus pre-event
        void StartHoptallusPreEvent()
        {
            if(Creature* hoptallus = instance->GetCreature(m_ullHoptallusGuid))
                hoptallus->AI()->DoAction(0);
        }

        // General door handling
        void HandleDoors(uint32 state, uint64 ullFirstGuid, uint64 ullSecondGuid)
        {
            if(state == IN_PROGRESS)
                HandleGameObject(GetData64(ullFirstGuid), false);
            else if(state == FAIL)
                HandleGameObject(GetData64(ullFirstGuid), true);
            else if(state == DONE)
            {
                if(ullSecondGuid)
                    HandleGameObject(GetData64(ullSecondGuid), true);
                HandleGameObject(GetData64(ullFirstGuid), true);
            }
        }

    private :
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        uint32 m_auiAlamental[MAX_ALAMENTAL];

        uint8 m_ucKilledHozenCounter ;
        uint64 m_ullOokOokGuid ;
        uint64 m_ullHoptallusGuid ;
        uint64 m_ullYanZhuGuid ; // Why not in an array ?

        uint64 m_ullOokOokEntranceDoorGuid ;
        uint64 m_ullOokOokExitDoorGuid ;
        uint64 m_ullHoptallusEntranceDoorGuid ;
        uint64 m_ullHoptallusExitDoorGuid ;
        uint64 m_ullYanZhuEntranceDoorGuid ;
    };

    InstanceScript* GetInstanceScript(InstanceMap *instance) const
    {
        return new instance_stormstout_brewery_InstanceScript(instance);
    }
};

void AddSC_instance_stormstout_brewery()
{
    new instance_stormstout_brewery();
}
