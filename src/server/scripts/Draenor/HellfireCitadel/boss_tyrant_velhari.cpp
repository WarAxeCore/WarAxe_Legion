/*
░██╗░░░░░░░██╗░█████╗░██████╗░░█████╗░██╗░░██╗███████╗░█████╗░░█████╗░██████╗░███████╗
░██║░░██╗░░██║██╔══██╗██╔══██╗██╔══██╗╚██╗██╔╝██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝
░╚██╗████╗██╔╝███████║██████╔╝███████║░╚███╔╝░█████╗░░██║░░╚═╝██║░░██║██████╔╝█████╗░░
░░████╔═████║░██╔══██║██╔══██╗██╔══██║░██╔██╗░██╔══╝░░██║░░██╗██║░░██║██╔══██╗██╔══╝░░
░░╚██╔╝░╚██╔╝░██║░░██║██║░░██║██║░░██║██╔╝╚██╗███████╗╚█████╔╝╚█████╔╝██║░░██║███████╗
░░░╚═╝░░░╚═╝░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝░╚════╝░░╚════╝░╚═╝░░╚═╝╚══════╝

(c) Copyright WarAxeCore_Legion 2020
*/
//Raid Modes: LFR, Normal, Heroic

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "hellfire_citadel.h"
#include "SpellScript.h"

//Every player over 10 raises boss hp by roughly 4 million

enum Tyrant_Spells
{
    SPELL_TOUCH_OF_HARM = 180166, // Verified Working (LFR, Normal, Heroic)
    SPELL_AURA_OF_OPPRESSION = 181718, // Verified Working (LFR, Normal, Heroic)
    SPELL_AURA_OF_CONTEMPT = 179986,
    SPELL_AURA_OF_MALICE = 179991,
    SPELL_ANNIHILATING_STRIKE = 180260, // Verified Working (LFR, Normal, Heroic)
    SPELL_Edict_of_Condemnation = 182459,
    SPELL_INFERNAL_TEMPEST = 180300,
    SPELL_FONT_OF_CORRUPTION = 180526,
    SPELL_SEAL_OF_DECAY = 180000, // Verified Working (LFR, Normal, Heroic)
    SPELL_TAINTED_SHADOWS = 180533,
    SPELL_BULWARK_OF_TYRANT = 180600,
    SPELL_GAVEL_OF_TYRANT = 180608,
    SPELL_AURA_DARK_POWER = 180054,
    SPELL_AURA_GIVE_DARK_POWER = 180238,
    SPELL_AURA_REMOVE_DARK_POWER = 187484,
    SPELL_OPPRESSION_DMG = 184990,
    SPELL_OPPRESSION_AURA_DEBUFF = 181683,
    SPELL_TAINED_SHADOWS_2 = 180569,
};

enum Tyrant_Misc
{
    NPC_ANCIENT_ENFORCER = 90270,
    NPC_ANCIENT_HARBINGER = 90271,
    NPC_ANCIENT_SOVEREIGN = 90272,
};

enum Tyrant_Texts
{
    SAY_AGGRO = 1,
    SAY_90_TEXT,
    SAY_INFERNAL_TEMPEST,
    SAY_70_TEXT,
    SAY_60_TEXT,
    SAY_FONT_OF_CORRUPTION_1,
    SAY_FONT_OF_CORRUPTION_2,
    SAY_FONT_OF_CORRUPTION_3,
    SAY_FONT_OF_CORRUPTION_4,
    SAY_40_TEXT,
    SAY_30_TEXT,
    SAY_GAVEL_OF_TYRANT,
    SAY_KILL_PLAYER_1,
    SAY_KILL_PLAYER_2,
    SAY_KILL_PLAYER_3,
    SAY_KILL_PLAYER_4,
    SAY_WIPE,
    SAY_DEATH,
};

//Font of Corruption (3 targets < 11 players, 4 targets 12 to 15, 5 targets > 16)

class boss_tyrant_velhari : public CreatureScript
{
public:
    boss_tyrant_velhari() : CreatureScript("boss_tyrant_velhari") { }

    struct boss_tyrant_velhariAI : public BossAI
    {
        boss_tyrant_velhariAI(Creature* creature) : BossAI(creature, DATA_VELHARI) {}

        void Reset()
        {
            events.Reset();
            _Reset();

            me->RemoveAura(SPELL_AURA_OF_OPPRESSION);
            me->RemoveAura(SPELL_AURA_OF_MALICE);
            me->RemoveAura(SPELL_AURA_OF_CONTEMPT);

            me->CastSpell(me, SPELL_AURA_DARK_POWER, true);

            RespawnAncients();

                switch (instance->instance->GetDifficultyID())
                {
                case DIFFICULTY_10_N:
                case DIFFICULTY_25_N:
                case DIFFICULTY_NORMAL_RAID:
                    me->SetScaledHp(137819920);
                    break;
                case DIFFICULTY_10_HC:
                case DIFFICULTY_25_HC:
                case DIFFICULTY_HEROIC_RAID:
                    me->SetScaledHp(267165264);
                    break;
                case DIFFICULTY_LFR:
                    me->SetScaledHp(157080896);
                    break;
                }

            me->_oppressionStacks = 0;
            _addPhase = 0;
            playerCount = 0;
        }

        void RespawnAncients()
        {
            //Check if any previous ancients were spawned then delete
            if (Creature* pTemp = Unit::GetCreature(*me, AE_GUID))
            {
                if (pTemp)
                    pTemp->DisappearAndDie();
            }
            if (Creature* pTemp = Unit::GetCreature(*me, AH_GUID))
            {
                if (pTemp)
                    pTemp->DisappearAndDie();
            }
            if (Creature* pTemp = Unit::GetCreature(*me, AS_GUID))
            {
                if (pTemp)
                    pTemp->DisappearAndDie();
            }

            //Now reset all GUIDs to zero.
            AE_GUID = ObjectGuid::Empty;
            AH_GUID = ObjectGuid::Empty;
            AS_GUID = ObjectGuid::Empty;

            // Finally resummon and reassign the objectguids.
            Unit* AE = me->SummonCreature(NPC_ANCIENT_ENFORCER, 3831.416f, 2538.141f, 213.57f);
            if (AE)
            {
                AE->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                AE_GUID = AE->GetGUID();
            }
            Unit* AH = me->SummonCreature(NPC_ANCIENT_HARBINGER, 3829.40f, 2524.93f, 216.67f);
            if (AH)
            {
                AH->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                AH_GUID = AH->GetGUID();
            }
            Unit* AS = me->SummonCreature(NPC_ANCIENT_SOVEREIGN, 3828.70f, 2550.00f, 216.67f);
            if (AS)
            {
                AS->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                AS_GUID = AS->GetGUID();
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            me->BossYell("The Eredar are no strangers to war. Are you so eager to meet your end?", 50585);
            me->CastSpell(me, SPELL_AURA_OF_OPPRESSION, true);

            Map::PlayerList const &plist = me->GetMap()->GetPlayers();
            if (!plist.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                    {
                        pPlayer->CastSpell(pPlayer, SPELL_OPPRESSION_AURA_DEBUFF, true);
                        pPlayer->MaxHealedHealthPct = 80;
                    }
                }
            }

            events.RescheduleEvent(EVENT_1, urand(8000, 10000));
            events.RescheduleEvent(EVENT_2, urand(14000, 15000));
            events.RescheduleEvent(EVENT_3, 1000);
            events.RescheduleEvent(EVENT_4, 65000);
            events.RescheduleEvent(EVENT_5, urand(2000, 4000));
            events.RescheduleEvent(EVENT_6, urand(8000, 10000));
            events.RescheduleEvent(EVENT_7, urand(8000, 10000));
            events.RescheduleEvent(EVENT_28, 13000); // -5% player max health
            events.RescheduleEvent(EVENT_29, 5000);
            events.RescheduleEvent(EVENT_30, 500); // Check player movement
        }

        void KilledUnit(Unit* victim) override
        {
            if (!victim->IsPlayer())
                return;

            switch (urand(0, 3))
            {
            case 0:
                me->BossYell("Not. Even. Worthy.", 50593);
                break;
            case 1:
                me->BossYell("Know your place, weakling!", 50594);
                break;
            case 2:
                me->BossYell("Oh, is that one finished?", 50595);
                break;
            case 3:
                me->BossYell("Does it hurt, being so powerless?", 50596);
                break;
            }
        }

        void EnterEvadeMode()
        {
            //Talk(SAY_EVADE);
            BossAI::EnterEvadeMode();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->BossYell("What a... tragedy...", 50586);
            _JustDied();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType dmgType)
        {
            if (me->HasAura(SPELL_AURA_OF_OPPRESSION) && me->HealthBelowPctDamaged(70, damage))
            {
                me->BossYell("You are getting so very frail and weak.", 50591);
                me->RemoveAura(SPELL_AURA_OF_OPPRESSION);
                me->CastSpell(me, SPELL_AURA_OF_CONTEMPT, true);
            }

            if (me->HasAura(SPELL_AURA_OF_CONTEMPT) && me->HealthBelowPctDamaged(40, damage))
            {
                me->BossYell("Enough! This is where you die!", 50592);
                me->RemoveAura(SPELL_AURA_OF_CONTEMPT);
                me->CastSpell(me, SPELL_AURA_OF_MALICE, true);
            }

            if (_addPhase == 0 && me->HealthBelowPctDamaged(90, damage))
            {
                me->BossYell("Enforcer, enact my will!", 50590);
                Creature* AncientEnforcer = me->GetMap()->GetCreature(AE_GUID);
                if (AncientEnforcer)
                {
                    AncientEnforcer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientEnforcer->setFaction(14);
                    AncientEnforcer->GetMotionMaster()->MoveJump(3798.55f, 2537.91f, 204.61f, 5.0f, 15.0f);
                    AncientEnforcer->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }
                else
                {
                    AncientEnforcer= me->SummonCreature(NPC_ANCIENT_ENFORCER, 3798.55f, 2537.91f, 204.61f);
                    AncientEnforcer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientEnforcer->setFaction(14);
                    AncientEnforcer->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }

                ++_addPhase;
            }
            if (_addPhase == 1 && me->HealthBelowPctDamaged(60, damage))
            {
                me->BossYell("Harbinger, restore my wounds!", 50588);
                Creature* AncientHarbinger = me->GetMap()->GetCreature(AH_GUID);
                if (AncientHarbinger)
                {
                    AncientHarbinger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientHarbinger->setFaction(14);
                    AncientHarbinger->GetMotionMaster()->MoveJump(3798.55f, 2537.91f, 204.61f, 5.0f, 15.0f);
                    AncientHarbinger->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }
                else
                {
                    AncientHarbinger = me->SummonCreature(NPC_ANCIENT_HARBINGER, 3798.55f, 2537.91f, 204.61f);
                    AncientHarbinger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientHarbinger->setFaction(14);
                    AncientHarbinger->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }

                ++_addPhase;
            }
            if (_addPhase == 2 && me->HealthBelowPctDamaged(30, damage))
            {
                me->BossYell("The Sovereign watches over me!", 50589);
                Creature* AncientSovereign = me->GetMap()->GetCreature(AS_GUID);
                if (AncientSovereign)
                {
                    AncientSovereign->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientSovereign->setFaction(14);
                    AncientSovereign->GetMotionMaster()->MoveJump(3798.55f, 2537.91f, 204.61f, 5.0f, 15.0f);
                    AncientSovereign->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }
                else
                {
                    AncientSovereign = me->SummonCreature(NPC_ANCIENT_SOVEREIGN, 3798.55f, 2537.91f, 204.61f);
                    AncientSovereign->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    AncientSovereign->setFaction(14);
                    AncientSovereign->AI()->AttackStart(me->SelectNearestTarget(500.0f));
                }

                ++_addPhase;
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                    // Tracks a player, slashing out in a cone for 13145 Fire damage. All players struck create a Searing Blaze at their location.
                    if (me->HasAura(SPELL_AURA_OF_OPPRESSION))
                    {
                        me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_ANNIHILATING_STRIKE);
                    }
                    events.RescheduleEvent(EVENT_1, urand(9000, 10000));
                    break;
                case EVENT_2:
                    // Places a shell of shadow magic around a player that absorbs 24485 healing.If Touch of Harm is dispelled, the remaining absorb jumps to another random player.
                    me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_TOUCH_OF_HARM);
                    events.RescheduleEvent(EVENT_2, 60000);
                    break;
                case EVENT_3: // Corrupt Power Abilities
                    if (me->GetPower(POWER_ALTERNATE) == 3)
                    {
                        if (me->HasAura(SPELL_AURA_OF_OPPRESSION))
                        {
                            me->BossYell("A tempest of righteous fire!", 50602);
                            me->CastSpell(me, SPELL_INFERNAL_TEMPEST, false);
                            events.RescheduleEvent(EVENT_3, 10000);
                        }
                        if (me->HasAura(SPELL_AURA_OF_CONTEMPT))
                        {
                            switch (urand(0, 3))
                            {
                            case 0:
                                me->BossYell("Your existence, without purpose.", 50598);
                                break;
                            case 1:
                                me->BossYell("Your failures, absolute.", 50599);
                                break;
                            case 2:
                                me->BossYell("Your attempts, meaningless.", 50597);
                                break;
                            case 3:
                                me->BossYell("Your thoughts, insipid.", 50600);
                                break;
                            }
                            UnitList targets;
                            SelectTargetList(targets, 3, SELECT_TARGET_RANDOM, 0.0f, true);
                            if (!targets.empty())
                                for (UnitList::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    DoCast((*itr), SPELL_FONT_OF_CORRUPTION, true);
                                break;
                            events.RescheduleEvent(EVENT_3, 10000);
                        }
                        if (me->HasAura(SPELL_AURA_OF_MALICE))
                        {
                            me->BossYell("Insects deserve to be crushed!", 50601);
                            me->CastSpell(me, SPELL_GAVEL_OF_TYRANT, false);
                            events.RescheduleEvent(EVENT_3, 10000);
                        }
                    }
                    else
                    {
                        events.RescheduleEvent(EVENT_3, 1000);
                    }
                    break;
                case EVENT_4:
                    // Edict of Condemnation inflicts 552 Shadow damage, split among all players struck.This effect strikes every 3 sec. for 9 sec.
                    me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_Edict_of_Condemnation);
                    events.RescheduleEvent(EVENT_2, urand(53000, 55000));
                    break;
                case EVENT_5:
                    // Melee attacks occasionally curse the target with shadow magic, reducing healing and absorbs received by 10 % for 18 sec.This effect stacks.
                    if (me->getVictim())
                    me->CastSpell(me->getVictim(), SPELL_SEAL_OF_DECAY);
                    events.RescheduleEvent(EVENT_5, urand(5000, 9000));
                    break;
                case EVENT_6:
                    // Crushes a player with dark magic, inflicting 4712 Shadow damage and triggering a Corrupted Blast.
                    if (me->HasAura(SPELL_AURA_OF_CONTEMPT))
                    {
                        if (me->getVictim())
                        {
                            me->CastSpell(me->getVictim(), SPELL_TAINTED_SHADOWS, false);
                            Map::PlayerList const &plist = me->GetMap()->GetPlayers();
                            if (!plist.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
                                {
                                    if (Player* pPlayer = itr->getSource())
                                    {
                                        if (pPlayer->HasAura(SPELL_FONT_OF_CORRUPTION)) //Players affected by Font of Corruption will take  Tainted Shadows damage each time  Tainted Shadows damage is dealt
                                        {
                                            me->CastSpell(pPlayer, SPELL_TAINED_SHADOWS_2, true);
                                        }
                                    }
                                }
                            }                          
                        }
                    }
                    events.RescheduleEvent(EVENT_6, urand(9000, 10000));
                    break;
                case EVENT_7:
                    // Inflicts 3614 Shadow damage to the current target as well as additional random players, leaving behind Despoiled Ground at their locations.
                    if (me->HasAura(SPELL_AURA_OF_MALICE))
                    {
                        if (me->getVictim())
                        {
                            me->CastSpell(me->getVictim(), SPELL_BULWARK_OF_TYRANT, false); // Current Target
                            me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_BULWARK_OF_TYRANT, true); // Random player
                            me->CastSpell(me, SPELL_AURA_GIVE_DARK_POWER, true);
                        }
                    }
                    events.RescheduleEvent(EVENT_1, urand(9000, 10000));
                    break;
                case EVENT_28:
                    // Initially, the Aura caps players' health at 80%, but every 13 seconds the cap reduces by 5%
                    if (me->HasAura(SPELL_AURA_OF_CONTEMPT))
                    {
                        Map::PlayerList const &plist = me->GetMap()->GetPlayers();
                        if (!plist.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
                            {
                                if (Player* pPlayer = itr->getSource())
                                {
                                    pPlayer->MaxHealedHealthPct = pPlayer->MaxHealedHealthPct - 5;
                                }
                            }
                        }
                    }
                        break;
                case EVENT_29:
                    // Every 5 seconds a 10% dmg increase is added when Aura of Oppression is up
                    if (me->HasAura(SPELL_AURA_OF_OPPRESSION))
                    {
                        me->_oppressionStacks += 0.10;
                        events.RescheduleEvent(EVENT_29, 5000);
                    }
                    break;
                case EVENT_30:
                    // Aura of Opression movement check
                    if (me->HasAura(SPELL_AURA_OF_OPPRESSION))
                    {
                        Map::PlayerList const &plist = me->GetMap()->GetPlayers();
                        if (!plist.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
                            {
                                if (Player* pPlayer = itr->getSource())
                                {
                                    if (pPlayer->isMoving()) //Players affected by Font of Corruption will take  Tainted Shadows damage each time  Tainted Shadows damage is dealt
                                    {
                                        pPlayer->CastSpell(pPlayer, SPELL_OPPRESSION_DMG, true);
                                    }
                                }
                            }
                        }
                    }
                    if (me->HasAura(SPELL_AURA_OF_CONTEMPT))
                    {
                        Map::PlayerList const &plist = me->GetMap()->GetPlayers();
                        if (!plist.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
                            {
                                if (Player* pPlayer = itr->getSource())
                                {
                                    if (pPlayer->GetHealthPct() >= pPlayer->MaxHealedHealthPct) //Players affected by Font of Corruption will take  Tainted Shadows damage each time  Tainted Shadows damage is dealt
                                    {
                                        pPlayer->SetHealth(CalculatePct(pPlayer->GetMaxHealth(), pPlayer->MaxHealedHealthPct));
                                    }
                                }
                            }
                        }
                    }
                    events.RescheduleEvent(EVENT_30, 500);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        int8 _oppressionStack;
        int8 _addPhase;
        int8 playerCount;
        ObjectGuid AE_GUID;
        ObjectGuid AH_GUID;
        ObjectGuid AS_GUID;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_tyrant_velhariAI(creature);
    }
};

class spell_taintedshadows_tyrant : public SpellScript
{
    PrepareSpellScript(spell_taintedshadows_tyrant);

    //Crushes a player with dark magic, inflicting $s1 Shadow damage and triggering a Corrupted Blast.

    void DealDamage()
    {
        if (!GetCaster() || !GetHitUnit())
            return;

        switch (GetCaster()->GetMap()->GetDifficultyID())
        {
        case DIFFICULTY_LFR_RAID:
        case DIFFICULTY_LFR:
            SetHitDamage(urand(24499, 27000));
            break;
        case DIFFICULTY_10_N:
        case DIFFICULTY_25_N:
        case DIFFICULTY_NORMAL_RAID:
            SetHitDamage(urand(48899, 54000));
            break;
        case DIFFICULTY_10_HC:
        case DIFFICULTY_25_HC:
        case DIFFICULTY_HEROIC_RAID:
            SetHitDamage(urand(68499, 75600));
            break;
        case DIFFICULTY_MYTHIC_RAID:
            SetHitDamage(urand(92499, 102000));
            break;
        default:
            SetHitDamage(48899);
            break;
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_taintedshadows_tyrant::DealDamage);
    }
};

//184990
class spell_oppression_tyrant : public SpellScript
{
    PrepareSpellScript(spell_oppression_tyrant);

    //This damage starts out light, but increases by 10% every 5 seconds, so the longer players stay in this phase, the more dangerous movement will become. NYI

    void DealDamage()
    {
        if (!GetCaster() || !GetHitUnit())
            return;

        Unit* tyrantVelhari = GetCaster()->FindNearestCreature(93439, 300.0f);
        if (!tyrantVelhari)
            return;

        switch (GetCaster()->GetMap()->GetDifficultyID())
        {
        case DIFFICULTY_LFR_RAID:
        case DIFFICULTY_LFR:
            SetHitDamage(5292 + (5292 * tyrantVelhari->_oppressionStacks)); //Value: 5292 on LFR
            break;
        case DIFFICULTY_10_N:
        case DIFFICULTY_25_N:
        case DIFFICULTY_NORMAL_RAID:
            SetHitDamage(10585 + (10585 * tyrantVelhari->_oppressionStacks)); //Value: 10585 on Normal
            break;
        case DIFFICULTY_10_HC:
        case DIFFICULTY_25_HC:
        case DIFFICULTY_HEROIC_RAID:
            SetHitDamage(14820 + (14820 * tyrantVelhari->_oppressionStacks)); //Value: 14820 on Heroic
            break;
        case DIFFICULTY_MYTHIC_RAID:
            SetHitDamage(20007 + (20007 * tyrantVelhari->_oppressionStacks)); //Value: 20007 on Mythic
            break;
        default:
            SetHitDamage(10585 + (10585 * tyrantVelhari->_oppressionStacks)); //Value: 10585 on Normal
            break;
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_oppression_tyrant::DealDamage);
    }
};

class spell_corrupt_power_give : public SpellScriptLoader
{
public:
    spell_corrupt_power_give() : SpellScriptLoader("spell_corrupt_power_give") { }

    class spell_corrupt_power_give_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corrupt_power_give_SpellScript);

        void OnAfterCast()
        {
            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->CastSpell(GetCaster()->ToCreature(), SPELL_AURA_GIVE_DARK_POWER, true);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_corrupt_power_give_SpellScript::OnAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corrupt_power_give_SpellScript();
    }
};

class spell_corrupt_power_remove : public SpellScriptLoader
{
public:
    spell_corrupt_power_remove() : SpellScriptLoader("spell_corrupt_power_remove") { }

    class spell_corrupt_power_remove_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corrupt_power_remove_SpellScript);

        void OnAfterCast()
        {
            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->CastSpell(GetCaster()->ToCreature(), SPELL_AURA_REMOVE_DARK_POWER, true);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_corrupt_power_remove_SpellScript::OnAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corrupt_power_remove_SpellScript();
    }
};

void AddSC_boss_tyrant_velhari()
{
    new boss_tyrant_velhari();
    RegisterSpellScript(spell_oppression_tyrant);
    RegisterSpellScript(spell_taintedshadows_tyrant);
    new spell_corrupt_power_give();
    new spell_corrupt_power_remove();
}