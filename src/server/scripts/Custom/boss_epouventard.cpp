/*
 * Non mais pour qui il c'est pris Morgan de foutre un copyright ici !!!
 * ÃA C'EST Ã ERYLDOR, DONC Ã MOI, MOI ET MOI SEUL !!!!
 */

#include "ScriptPCH.h"
#include "Map.h"
#include "Group.h"
#include "GameObjectAI.h"

#define PI 3.1415926f
#define PHASE_DESTINATION 2 //On met tout le groupe en phase 2

enum Spells
{
    //Phase La Belle
    //Phase Naz'jar
    //Phase Trognepus
};

enum Phase
{
    PHASE_INTRO = 0,
    PHASE_LA_BELLE = 1,
    PHASE_NAZJAR = 2,
    PHASE_TROGNEPUS = 3,
    PHASE_VENTS = 4
};

enum Event
{
    EVENT_HELLO = 1,
    EVENT_START_COMBAT = 2
};

std::vector<uint64> playerDephase;

#define DISPLAYID_LA_BELLE 30148
#define DISPLAYID_NAZJAR 34342
#define DISPLAYID_TROGNEPUS 31005

#define GOB_BLEU 9000029
#define GOB_VERT 9000030

#define SPELL_VERT 22577
#define SPELL_BLEU 22576
#define SPELL_MASSACRE 82848

class boss_epouventard : public CreatureScript
{
    public:
    boss_epouventard() : CreatureScript("boss_epouventard") {}

    bool OnGossipHello(Player *pPlayer, Creature *pCreature)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ton reigne de terreur s'arrÃŠte aujourd'hui ! Tu vas payer !", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->PlayerTalkClass->SendGossipMenu(397001, pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player *pPlayer, Creature *pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if(uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {            

            pCreature->AI()->DoAction(PHASE_INTRO);
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    struct boss_epouventardAI : public ScriptedAI
    {
        boss_epouventardAI(Creature *c) : ScriptedAI(c){}

        bool introDone;
        bool addSpawn;

        uint32 m_phase;

        //Timer
        //Phase intro
        uint32 m_phaseHello;
        uint32 m_phaseAnnounceLaBelle;
        uint32 m_phaseSwitchToUn;        

        //Phase 1
        uint32 m_phase1LavaDrool;
        uint32 m_phase1BerserkCharge;
        uint32 m_phase1Roar;
        uint32 m_phase1Circle;
        uint32 m_phase1Massacre;
        bool add1Done;

        //Phase 2
        uint32 m_phase2SwitchToNazjar;
        uint32 m_phase2Geyser;
        uint32 m_phase2SummonAdds;
        uint32 m_phase2ZoneOmbre;
        uint32 m_phase2MaleFatigue;
        bool ventDone;
        uint8 m_ventCounter;

        //Phase 3
        uint32 m_phase3SwitchToTrognepus;
        uint32 m_phase3Englue;
        uint32 m_phase3Fracasser;
        uint32 m_phase3Meteore;
        uint64 kifouette, bijou;
        uint32 m_kResurectTimer; //Timer resurrection Kifouette
        uint32 m_bResurectTimer; //Timer resurrection Bijou
        bool addDead;

        std::vector<uint64> vent;
        uint32 m_ventTimer;

        uint64 gob_bleu;
        uint64 gob_vert;
        uint64 gob_bon;

        bool m_isHeroic;

        void Reset()
        {
            me->setFaction(35);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetControlled(false, UNIT_STATE_ROOT);
            introDone = false;
            addSpawn = false;
            addDead = false; //Phase trognepus
            m_phase = PHASE_LA_BELLE;
            me->DeMorph();
            for(int i = 0 ; i < vent.size() ; i++)
            {
                Creature *cVent = ObjectAccessor::GetCreature(*me, vent[i]);
                if(cVent)
                    cVent->DisappearAndDie();
            }


            playerDephase.clear();

            Creature *cBijou = ObjectAccessor::GetCreature(*me, bijou);
            if(cBijou)
                cBijou->DisappearAndDie();
            Creature *cKifouette = ObjectAccessor::GetCreature(*me, kifouette);
            if(cKifouette)
                cKifouette->DisappearAndDie();

            GameObject *gbleu = ObjectAccessor::GetGameObject(*me, gob_bleu);
            if(gbleu)
                gbleu->Delete();
            GameObject *gvert = ObjectAccessor::GetGameObject(*me, gob_vert);
            if(gvert)
                gvert->Delete();
        }

        void JustDied(Unit */*who*/)
        {
            Talk(-1397008);
            for(int i = 0 ; i < vent.size() ; i++)
            {
                Creature *cVent = ObjectAccessor::GetCreature(*me, vent[i]);
                if(cVent)
                    cVent->DisappearAndDie();
            }

            Creature *cBijou = ObjectAccessor::GetCreature(*me, bijou);
            if(cBijou)
                cBijou->DisappearAndDie();
            Creature *cKifouette = ObjectAccessor::GetCreature(*me, kifouette);
            if(cKifouette)
                cKifouette->DisappearAndDie();

            Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    Player *p = i->getSource();
                    if (p)
                    {
                        if (Is25ManRaid())
                            p->CompleteQuest(700131);
                        else
                            p->CompleteQuest(700013);
                    }
                }
            }
        }

        void DamageTaken(Unit *who, uint32 &damage)
        {
            if(me->GetHealth() < damage)
                me->DeMorph();

            if(who)
            {
                if(Player *plr = who->ToPlayer())
                {
                    if(Group *grp = plr->GetGroup())
                    {
                        m_isHeroic = (grp->GetMembersCount() > 10);
                    }
                }
            }
        }

        void DoAction(const int32 param)
        {
            if(param == PHASE_INTRO)
            {
                StartBoss();
            }
        }

        void StartBoss()
        {
            me->setFaction(14);
            if(!introDone)
            {                
                m_phase = PHASE_INTRO;
                m_phaseHello = 1000;
                m_phaseAnnounceLaBelle = 20000;
                m_phaseSwitchToUn = 25000;
                introDone = true;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(m_phase == PHASE_VENTS)
            {
                if(m_ventTimer <= uiDiff)
                {
                    if(m_ventCounter > 5)
                    {
                        m_phase = PHASE_NAZJAR;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        return;
                    }

                    uint32 index = urand(0, vent.size()-1);
                    Creature *cVent = ObjectAccessor::GetCreature(*me, vent[index]);
                    if(cVent)
                    {                        
                        cVent->CastSpell(cVent, 93059, true); //Bouclier des tempete
                        m_ventTimer = m_isHeroic ? 8000 : 12000;
                        m_ventCounter++;
                    }
                }
                else
                    m_ventTimer -= uiDiff;
            }

            if(m_phase == PHASE_INTRO)
            {
                if(m_phaseHello <= uiDiff)
                {
                    Talk(-1397001);
                    m_phaseHello = 200000;
                }
                else
                    m_phaseHello -= uiDiff;

                if(m_phaseAnnounceLaBelle <= uiDiff)
                {
                    Talk(-1397002);
                    m_phaseAnnounceLaBelle = 20000;
                }
                else
                    m_phaseAnnounceLaBelle -= uiDiff;

                if(m_phaseSwitchToUn <= uiDiff)
                {
                    me->SetDisplayId(DISPLAYID_LA_BELLE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->Attack(SelectTarget(SELECT_TARGET_RANDOM), true);
                    m_phase = PHASE_LA_BELLE;

                    Talk(-1397009);

                    m_phase1LavaDrool = 5000;
                    m_phase1BerserkCharge = 15000;
                    m_phase1Roar = 25000;
                    m_phase1Circle = 45000;
                    m_phase1Massacre = 51000;
                    add1Done = false;
                }
                else
                    m_phaseSwitchToUn -= uiDiff;
            }

            if(!UpdateVictim() || !introDone)
                return;

            if(m_phase == PHASE_LA_BELLE)
            {
                if(m_phase1LavaDrool <= uiDiff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_RANDOM), 93666);
                    m_phase1LavaDrool = 5000;
                }
                else
                    m_phase1LavaDrool -= uiDiff;

                if(m_phase1BerserkCharge <= uiDiff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_FARTHEST), 76030);
                    m_phase1BerserkCharge = 25000;
                }
                else
                    m_phase1BerserkCharge -= uiDiff;

                if(m_phase1Roar <= uiDiff)
                {
                    DoCastAOE(93586);
                    m_phase1Roar = 30000;
                }
                else
                    m_phase1Roar -= uiDiff;

                if(m_phase1Circle <= uiDiff)
                {
                    GameObject *gbleu = me->SummonGameObject(GOB_BLEU, me->GetPositionX(), me->GetPositionY() - 20, me->GetPositionZ()+5, 0, 0, 0, 0, 0, 0);
                    if(gbleu)
                        gob_bleu = gbleu->GetGUID();
                    GameObject *gvert = me->SummonGameObject(GOB_VERT, me->GetPositionX(), me->GetPositionY() + 20, me->GetPositionZ()+5, 0, 0, 0, 0, 0, 0);
                    if(gvert)
                        gob_vert = gvert->GetGUID();

                    if(gbleu && gvert)
                    {
                        uint32 couleur = urand(0, 1);
                        if(couleur)
                        {
                            me->AddAura(SPELL_VERT, me);
                            Talk(-1397010);
                            gbleu->AI()->DoAction(1);
                        }
                        else
                        {
                            gvert->AI()->DoAction(1);
                            Talk(-1397011);
                            me->AddAura(SPELL_BLEU, me);
                        }
                    }

                    DoStopAttack();
                    me->SetControlled(true, UNIT_STATE_ROOT);

                    m_phase1Circle = 30000;
                }
                else
                    m_phase1Circle -= uiDiff;

                if(m_phase1Massacre <= uiDiff)
                {
                    DoCastAOE(SPELL_MASSACRE);
                    me->SetControlled(false, UNIT_STATE_ROOT);
                    me->RemoveAura(SPELL_BLEU);
                    me->RemoveAura(SPELL_VERT);
                    m_phase1Massacre = 30000;
                }
                else
                    m_phase1Massacre -= uiDiff;

                if(HealthBelowPct(66))
                {
                    m_phase = PHASE_NAZJAR;
                    m_phase2SwitchToNazjar = 5000;
                    Talk(-1397003);
                }
            }

            if(m_phase == PHASE_NAZJAR)
            {
                if(m_phase2SwitchToNazjar <= uiDiff && me->GetDisplayId() == DISPLAYID_LA_BELLE)
                {
                    me->SetDisplayId(DISPLAYID_NAZJAR);
                    me->SetControlled(false, UNIT_STATE_ROOT);
                    Talk(-1405860);
                    m_phase2Geyser = 5000;
                    m_phase2SummonAdds = 30000;
                    m_phase2ZoneOmbre = 10000;
                    m_phase2MaleFatigue = 7000;
                    ventDone = false;
                }
                else
                    m_phase2SwitchToNazjar -= uiDiff;

                if(me->GetDisplayId() == DISPLAYID_NAZJAR)
                {
                    if(m_phase2Geyser <= uiDiff)
                    {
                        DoCast(SelectTarget(SELECT_TARGET_RANDOM), 75722); //Geyser (Naz'salope)
                        m_phase2Geyser = 10000;
                    }
                    else
                        m_phase2Geyser -= uiDiff;

                    if(m_phase2SummonAdds <= uiDiff && !addSpawn)
                    {
                        Talk(-1405864);
                        me->SummonCreature(406330, me->GetPositionX() + 10, me->GetPositionY(), me->GetPositionZ()+1, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        me->SummonCreature(444040, me->GetPositionX() - 10, me->GetPositionY(), me->GetPositionZ()+1, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        addSpawn = true;
                    }
                    else
                        m_phase2SummonAdds -= uiDiff;

                    if(m_phase2ZoneOmbre <= uiDiff)
                    {
                        DoCastAOE(96311); //Fissure sombre
                        m_phase2ZoneOmbre = 15000;
                    }
                    else
                        m_phase2ZoneOmbre -= uiDiff;

                    if(m_phase2MaleFatigue <= uiDiff)
                    {
                        DoCastToAllHostilePlayers(76094); //Malediction (Ulthok)
                        m_phase2MaleFatigue = 30000;
                    }
                    else
                        m_phase2MaleFatigue -= uiDiff;

                    if(HealthBelowPct(50) && !ventDone)
                    {
                        ventDone = true;
                        me->SetReactState(REACT_PASSIVE);
                        DoStopAttack();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        vent = SpawnInCircle(10.0f, 5, 51672, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        for(int i = 0 ; i < vent.size() ; i++)
                        {
                            Creature *c = ObjectAccessor::GetCreature(*me, vent[i]);
                            if(c)
                            {                                
                                c->SetReactState(REACT_PASSIVE);
                                c->Attack(SelectTarget(SELECT_TARGET_RANDOM), false);
                                c->SetControlled(true, UNIT_STATE_ROOT);
                            }
                        }

                        m_phase = PHASE_VENTS;
                        m_ventTimer = m_isHeroic ? 8000 : 12000;
                        m_ventCounter = 0;
                    }

                    if(HealthBelowPct(33))
                    {
                        m_phase = PHASE_TROGNEPUS;
                        m_phase3SwitchToTrognepus = 5000;
                        Talk(-1397004);
                    }
                }

            }

            if(m_phase == PHASE_TROGNEPUS)
            {
                if(m_phase3SwitchToTrognepus <= uiDiff && me->GetDisplayId() == DISPLAYID_NAZJAR)
                {
                    me->SetDisplayId(DISPLAYID_TROGNEPUS);
                    me->SetControlled(false, UNIT_STATE_ROOT);
                    Talk(-1397005);
                    m_phase3Fracasser = 20000;
                    m_phase3Englue = 26000;
                    m_phase3Meteore = m_isHeroic ? urand(11000, 13000) : urand(27000, 29000);
                    Unit *cKifouette = me->SummonCreature(397002, me->GetPositionX()+15, me->GetPositionY(), me->GetPositionZ()+1, 0, TEMPSUMMON_MANUAL_DESPAWN);
                    Unit *cBijou = me->SummonCreature(397003, me->GetPositionX()-15, me->GetPositionY(), me->GetPositionZ()+1, 0, TEMPSUMMON_MANUAL_DESPAWN);
                    m_bResurectTimer = 10000;
                    m_kResurectTimer = 10000;
                    if(cBijou && cKifouette)
                    {
                        bijou = cBijou->GetGUID();
                        kifouette = cKifouette->GetGUID();
                        addDead = false;
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    else
                        addDead = true;
                }
                else
                    m_phase3SwitchToTrognepus -= uiDiff;

                if(!addDead && me->GetDisplayId() == DISPLAYID_TROGNEPUS)
                {
                    Creature *cBijou = ObjectAccessor::GetCreature(*me, bijou);
                    Creature *cKifouette = ObjectAccessor::GetCreature(*me, kifouette);

                    if(!cBijou || !cKifouette)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        addDead = true;
                    }

                    if(cBijou->isDead() && cKifouette->isAlive()) //J'ai inverse les noms de pointeurs
                    {
                        if(m_bResurectTimer <= uiDiff)
                        {
                            int32 bp = 100;
                            me->MonsterYell("Relčve toi, Kifouette !", 0, 0);
                            me->CastCustomSpell(cBijou, 50769, &bp, 0, 0, true);
                            cBijou->Respawn(true);
                            m_bResurectTimer = 10000;
                        }
                        else
                            m_bResurectTimer -= uiDiff;
                    }
                    else if(cBijou->isAlive() && cKifouette->isDead())
                    {
                        if(m_kResurectTimer <= uiDiff)
                        {
                            int32 bp = 100;
                            me->MonsterYell("Reviens ā la vie, Bijou !", 0, 0);
                            me->CastCustomSpell(cKifouette, 50769, &bp, 0, 0, true);
                            cBijou->Respawn(true);
                            m_kResurectTimer = 10000;
                        }
                        else
                            m_kResurectTimer -= uiDiff;
                    }
                    else if(cBijou->isDead() && cKifouette->isDead())
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        addDead = true;
                    }
                }

                if(me->GetDisplayId() == DISPLAYID_TROGNEPUS)
                {
                    if(m_phase3Fracasser <= uiDiff)
                    {
                        Talk(-1397007);
                        m_phase3Fracasser = 20000;
                        DoCastAOE(92662);
                    }
                    else
                        m_phase3Fracasser -= uiDiff;

                    if(m_phase3Englue <= uiDiff)
                    {
                        Talk(-1397006);
                        m_phase3Englue = 20000;
                    }
                    else
                        m_phase3Englue -= uiDiff;

                    if(m_phase3Meteore <= uiDiff)
                    {
                        int32 degat = 0;
                        if(m_isHeroic)
                        {
                            degat = 2375000;
                            if(!addDead)
                                degat = 1500000;
                        }
                        else
                        {
                            degat = 950000;
                            if(!addDead)
                                degat = 600000;
                        }
                        me->CastCustomSpell(me->getVictim(), 28884, &degat, 0, 0, false);
                        m_phase3Meteore = 20000;
                    }
                    else
                        m_phase3Meteore -= uiDiff;
                }
            }

            DoMeleeAttackIfReady();
        }

        std::vector<uint64> SpawnInCircle(float rayon, uint8 number, uint32 entry, TempSummonType summon = TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, uint32 time = 30000)
        {
            float teta = (2*PI) / number;
            std::vector <uint64> result;
            for(uint8 i = 0 ; i < number ; i++)
            {
                float x = cos(i*teta) * rayon + me->GetPositionX();
                float y = sin(teta*i) * rayon + me->GetPositionY();

                Unit *tmp = me->SummonCreature(entry, x, y, me->GetPositionZ()+2.0f, 0, summon, time);
                result.push_back(tmp->GetGUID());
            }

            return result;
        }
    };

    CreatureAI *GetAI(Creature *c) const
    {
        return new boss_epouventardAI(c);
    }
};

class gob_circle : public GameObjectScript
{
public:
    gob_circle() : GameObjectScript("gob_circle"){}

    struct gob_circleAI : public GameObjectAI
    {
        gob_circleAI(GameObject *gob) : GameObjectAI(gob)
        {

        }

        bool isBouclier;

        uint32 m_removeBouclierTimer;
        uint32 m_checkTimer;

        void Reset()
        {
            isBouclier = false;
            m_removeBouclierTimer = 11000;
            m_checkTimer = 100;
        }

        void DoAction(const int32 ac)
        {
            isBouclier = (ac == 1);

            if(ac == 2)
            {
                Map *map = go->GetMap();
                if(!map)
                    return;

                 Map::PlayerList const &plist = map->GetPlayers();

                for(Map::PlayerList::const_iterator i = plist.begin() ; i != plist.end() ; ++i)
                {
                    //sLog->outString("Debut boucle 2");
                    Player *plr = i->getSource();
                    //sLog->outString("Apres getSource 2");
                    if(!plr)
                        continue;
                    if(plr->isGameMaster())
                        continue;
                    if(plr->HasAura(75563))
                        plr->RemoveAura(75563);

                    //sLog->outString("Fin boucle 2");
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(m_removeBouclierTimer <= uiDiff)
            {
                DoAction(2);
                go->Delete();
            }
            else
                m_removeBouclierTimer -= uiDiff;

            if(!isBouclier)
                return;

            if(m_checkTimer <= uiDiff)
            {
                Map *map = go->GetMap();
                if(!map)
                    return;

                 Map::PlayerList const &plist = map->GetPlayers();

                for(Map::PlayerList::const_iterator i = plist.begin() ; i != plist.end() ; ++i)
                {
                    //sLog->outString("Boucle 1");
                    Player *plr = i->getSource();
                    //sLog->outString("Apres getSource()");
                    if(!plr)
                        continue;
                    if(plr->isGameMaster())
                        continue;

                    //sLog->outString("Mileiur boucle 1");

                    Position plrPos;
                    plr->GetPosition(&plrPos);

                    if(go->GetExactDist2d(&plrPos) <= 10.0f && !plr->HasAura(75563))
                        plr->CastSpell(plr,75563, true);
                    else if(go->GetExactDist2d(&plrPos) > 10.0f && plr->HasAura(75563))
                        plr->RemoveAura(75563);
                    //sLog->outString("Fin boucle 1");
                }
                m_checkTimer = 100;
            }
            else
                m_checkTimer -= uiDiff;
        }
    };

    GameObjectAI *GetAI(GameObject *gob) const
    {
        return new gob_circleAI(gob);
    }
};

void AddSC_boss_epouventard()
{
    new boss_epouventard;
    new gob_circle;
}
