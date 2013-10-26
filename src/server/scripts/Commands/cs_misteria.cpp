#include <ScriptPCH.h>
#include <ReputationMgr.h>

extern uint32 realmID;

class cs_misteria : public CommandScript
{
public:
    cs_misteria() : CommandScript("cs_misteria"){}

    ChatCommand *GetCommands() const
    {
        static ChatCommand misteriaCommandTable[] =
        {
            {"recup",           SEC_PLAYER,            false, &HandleRecupCommand,               "", NULL },
            { NULL,             0,                     false, NULL,                              "", NULL }
        };

        return misteriaCommandTable;
    }

    static bool HandleRecupCommand(ChatHandler* handler, char const* args)
    {
        Player *pPlayer = handler->GetSession()->GetPlayer();

        if(!pPlayer)
            return false;

        uint32 guidLow = pPlayer->GetGUIDLow();

        QueryResult result = LoginDatabase.PQuery("SELECT id, level, stuff_wanted FROM recuperation WHERE idPerso=%u AND state=3 AND destination_realm=%u", guidLow, realmID);
        if(result->GetRowCount() == 0)
        {
            handler->PSendSysMessage("Vous ne disposez pas de recuperation pour ce personnage, ou elle n'a pas encore été validee par un maitre du jeu");
            return true;
        }

        Field *recup = result->Fetch();

        uint32 id = recup[0].GetUInt32();
        uint32 stuffId = recup[2].GetUInt32();
        uint8 level = recup[1].GetUInt8();

        QueryResult metier = LoginDatabase.PQuery("SELECT skill, value, max FROM recuperation_metier WHERE idRecup = %u", id);
        QueryResult reput = LoginDatabase.PQuery("SELECT reputId, value FROM recuperation_reput WHERE idRecup = %u", id);
        QueryResult stuff = LoginDatabase.PQuery("SELECT itemId FROM recuperation_stuff WHERE spe = %u", stuffId);


        uint32 sacId = 0;
        uint32 moneyToAdd = 0;
        if(level == 85)
        {
            sacId = 21841;
            moneyToAdd = 12000 * GOLD;
        }
        else
        {
            sacId = 30744;
            moneyToAdd = 10000 * GOLD;
        }

        pPlayer->ModifyMoney(moneyToAdd);

        for(uint32 i = INVENTORY_SLOT_BAG_START ; i < INVENTORY_SLOT_BAG_END ; i++)
        {
            if(Bag *pBag = (Bag*)pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                for(uint32 j = 0 ; j < pBag->GetBagSize(); j++)
                {
                    if(Item *pItem = pBag->GetItemByPos(j))
                        pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, j, false);
                }

                pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
            }

        }

        for(uint32 i = INVENTORY_SLOT_ITEM_START ; i < INVENTORY_SLOT_ITEM_END ; i++)
        {
            if(pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
        }

        for(uint32 i = 0 ; i < 4 ; i++)
        {
            pPlayer->EquipNewItem(INVENTORY_SLOT_BAG_START+i, sacId, true);
        }

        pPlayer->GiveLevel(level);
        pPlayer->InitTalentForLevel();
        pPlayer->SetUInt32Value(PLAYER_XP,0);

        if(stuff->GetRowCount() > 0)
        {
            do
            {
                Field * piece = stuff->Fetch();
                uint32 item = piece[0].GetUInt32();
                pPlayer->AddItem(item, 1);
            }
            while(stuff->NextRow());
        }

        if(reput->GetRowCount() > 0)
        {
            do
            {
                Field *rep = reput->Fetch();
                uint32 reputId = rep[0].GetUInt32();
                uint32 rank = rep[1].GetUInt32();

                FactionEntry const *fac = sFactionStore.LookupEntry(reputId);

                if(rank == 42000)
                    rank += 1000;

                pPlayer->GetReputationMgr().SetReputation(fac, rank);
            }
            while(reput->NextRow());
        }

        if(metier->GetRowCount() > 0)
        {
            do
            {
                Field *prof = metier->Fetch();

                uint32 skill = prof[0].GetUInt32();
                uint32 value = prof[1].GetUInt32();
                uint32 max = prof[2].GetUInt32();

                QueryResult profSpellQ = LoginDatabase.PQuery("SELECT competence FROM recuperation_metier_spell WHERE id = %u AND niveau = %u", skill, max);
                if(profSpellQ->GetRowCount() == 0)
                    continue;

                Field *profSpell = profSpellQ->Fetch();
                uint32 spell = profSpell->GetUInt32();

                if(!pPlayer->HasSpell(spell))
                    pPlayer->learnSpell(spell, true);

                pPlayer->SetSkill(skill, pPlayer->GetSkillStep(skill), value, max);
            }
            while(metier->NextRow());
        }

        LoginDatabase.PQuery("UPDATE recuperation SET state=5 WHERE id=%u", id);
        pPlayer->SaveToDB();

        return true;
    }
};

void AddSc_misteria_script()
{
    new cs_misteria();
}
