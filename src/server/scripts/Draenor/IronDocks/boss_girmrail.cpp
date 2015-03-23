/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "iron_docks.h"
#include "Vehicle.h"

enum Yells
{
    // Noxx
    SAY_INTRO                   = 50, // Is there killing to be done? Let me have 'em! (46128)
    SAY_SLAY                    = 3, // Tick.. tick.. tick.. BOOM! (46129)
    SAY_DEATH                   = 4, // I'm hit... (46127)
    SAY_SPELL01                 = 1, // Now you ain't so tall no more! (46130)
    SAY_SPELL02 =                 2, // You better step back. I'm calling in the bomb squad! (46131)
    SAY_SPELL03 =                 5, // Time is money friend, and it looks like you out of both! (46132)
    //Makogg
    SAY_INTRO_MAKOGG                   = 60, //  Hah! (45800)
    SAY_DEATH_MAKOGG                  = 61, //  This.. is.. far, from over.. (45799)
    SAY_SPELL01_MAKOGG                 = 62, //  You will burn! (45801)
};
enum Spells
{
    // Makogg
    SPELL_FLAMING_SLASH_DUMMY = 163665,
    SPELL_FLAMING_SLASH_UNKNOWN_AURA = 163669,
    SPELL_FLAMING_SLASH_DAMAGE = 163668,
    SPELL_LAVA_SWEEP_VISUAL = 98873, // HACK
    SPELL_LAVA_SWEEP_DAMAGE = 165152,
    // Ahri'ok dugru
    SPELL_BLOOD_BOLT = 165122,
    SPELL_SANGUINE_SPHERE = 163689,
    SPELL_ABRUPT_RESTORATION = 163705,
    SPELL_TAINTED_BLOOD = 163740,
    // Noxx
    SPELL_GUT_SHOT = 167115,
    SPELL_OGRE_TRAP_THROWING_VISUAL = 163307,
    SPELL_OGRE_TRAP_OPEN_TEETH = 177391,
    SPELL_OGRE_TRAP_CLOSED_TEETH = 177396,
    SPELL_OGRE_TRAPS = 163390,
    SPELL_OGRE_TRAP_SUMMONING = 163277,
    SPELL_SHREDDING_TENDONS = 163276,
    SPELL_JUMPER_CABLES = 163376,
    // Bombsquad
    SPELL_BIG_BOOM = 163379,
};
enum Events
{
    EVENT_FLAMING_SLASH = 500,
    EVENT_LAVA_SWEEP = 501,
    // DURGRU
    EVENT_BLOOD_BOLT = 502,
    EVENT_SANGUINE_SPHERE = 5654,
    EVENT_TAINTED_BLOOD = 504,
    // Noxx
    EVENT_GUT_SHOT = 505,
    EVENT_OGRE_TRAP = 506,
    EVENT_JUMPER_CABLES = 507,
    EVENT_BIG_BOOM = 508,
    EVENT_BOMB_SQUAD = 509,
};
enum Creatures
{
    TRIGGER_LAVA_SWEEP = 95353,
    CREATURE_OGRE_TRAP = 88758,
    BOMB_SQUAD_TRIGGER = 80875,
};
enum Talks
{
    TALK_ZOGGOSH_05 = 14, // Sir.. he's out of ammunition. I'm reloading.. I'm reloading!! (44052)
    TALK_KORAMAR_05 = 27, // What is wrong with you?! keep firing you all! (46904)
    TALK_KORAMAR_06 = 28, // Arghh.. forget it, they'll never make it pass the tracks.. (unknown message) (46905)

    // event 3
    TALK_ZOGGOSH_06 = 15, // Sir.. half of our army has been taken out. Don't you think we should.. (44053)
    TALK_KORAMAR_07 = 29, // That was an insignificant loss... let them exhaust themselves playing against our weak.. against those who're unworthy of serving in Black Hands army. That's all what Orc understand(46906)
};

class beforegrimrail_event : public BasicEvent
{
public:
    explicit beforegrimrail_event(Unit* unit, int value) : obj(unit), modifier(value)
    {
    }

    bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
    {
        if (InstanceScript* instance = obj->GetInstanceScript())
        {
            if (Creature* Zoggosh = instance->instance->GetCreature(instance->GetData64(DATA_ZUGGOSH)))
            {
                if (Creature* Koramar = instance->instance->GetCreature(instance->GetData64(DATA_KORAMAR)))
                {
                    if (Creature* Skulloc = instance->instance->GetCreature(instance->GetData64(DATA_SKULLOC)))
                    {
                        switch (modifier)
                        {
                        case 0:
                            Koramar->AI()->Talk(TALK_KORAMAR_05);
                            Koramar->m_Events.AddEvent(new beforegrimrail_event(Koramar, 1), Koramar->m_Events.CalculateTime(8000));
                            break;
                        case 1:
                            Zoggosh->AI()->Talk(TALK_ZOGGOSH_05);
                            Koramar->m_Events.AddEvent(new beforegrimrail_event(Koramar, 2), Koramar->m_Events.CalculateTime(10000));
                            break;
                        case 2:
                            Koramar->AI()->Talk(TALK_KORAMAR_06);
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }
private:
    Creature* storm;
    Unit* obj;
    int modifier;
    int Event;
};
class aftergrimrail_event : public BasicEvent
{
public:
    explicit aftergrimrail_event(Unit* unit, int value) : obj(unit), modifier(value)
    {
    }

    bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
    {
        if (InstanceScript* instance = obj->GetInstanceScript())
        {
            if (Creature* Zoggosh = instance->instance->GetCreature(instance->GetData64(DATA_ZUGGOSH)))
            {
                if (Creature* Koramar = instance->instance->GetCreature(instance->GetData64(DATA_KORAMAR)))
                {
                    if (Creature* Skulloc = instance->instance->GetCreature(instance->GetData64(DATA_SKULLOC)))
                    {
                        switch (modifier)
                        {
                        case 0:
                            Zoggosh->AI()->Talk(TALK_ZOGGOSH_06);
                            Koramar->m_Events.AddEvent(new aftergrimrail_event(Koramar, 1), Koramar->m_Events.CalculateTime(8000));
                            break;
                        case 1:
                            Koramar->AI()->Talk(TALK_KORAMAR_07);
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }
private:
    Creature* storm;
    Unit* obj;
    int modifier;
    int Event;
};
#define flamingslashinterval urand(9000, 12000)
#define lavasweepinterval urand(14000, 17000)
#define jumpercablesinterval 15000
#define ogretrapsinterval urand(18000, 20000)
#define gutshotinterval 4000
#define bombsquadinterval 30000
#define sanguinesphereinterval 20000
#define bloodboltinterval 6000
#define taintedbloodinterval 14000
class boss_grimrail_makogg : public CreatureScript
{
    public:
        boss_grimrail_makogg() : CreatureScript("boss_grimrail_makogg") { }

        struct boss_grimrail_makoggAI : public BossAI
        {
            boss_grimrail_makoggAI(Creature* creature) : BossAI(creature, DATA_GRIMRAIL_MAKOGG)
            {
                pinstance = me->GetInstanceScript();
                intro = false;
            }

            InstanceScript* pinstance;
            bool intro;

            void Reset() override
            {
                _Reset();     
            }
            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
                events.ScheduleEvent(EVENT_LAVA_SWEEP, lavasweepinterval);
                events.ScheduleEvent(EVENT_FLAMING_SLASH, flamingslashinterval);
            }
            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH_MAKOGG);

                if (Creature* NOXX = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_NOXX)))
                {
                    if (Creature* DUGURU = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_DUGURU)))
                    {
                        if (DUGURU->isDead() && NOXX->isDead())
                            DUGURU->m_Events.AddEvent(new aftergrimrail_event(DUGURU, 0), DUGURU->m_Events.CalculateTime(8000));
                    }
                } 
            }
            void MoveInLineOfSight(Unit* who)
            {
                if (who && who->IsInWorld() && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 35.0f) && !intro)
                {
                    intro = true;
                    Talk(SAY_INTRO_MAKOGG);

                    if (Creature* skulloc = pinstance->instance->GetCreature(pinstance->GetData64(DATA_SKULLOC)))
                        if (Creature* turret = skulloc->GetVehicleKit()->GetPassenger(0)->ToCreature())
                        {
                            turret->m_Events.KillAllEvents(true); // stops bombardment
                        }

                    me->m_Events.AddEvent(new beforegrimrail_event(me, 0), me->m_Events.CalculateTime(8000));
                }
            }
            void JustReachedHome()
            {
                if (Creature* NOXX = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_NOXX)))
                {
                    if (Creature* DUGURU = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_DUGURU)))
                    {
                        NOXX->Respawn();
                        NOXX->GetAI()->Reset();

                        DUGURU->Respawn();
                        DUGURU->GetAI()->Reset();  
                        instance->SetBossState(DATA_GRIMRAIL_MAKOGG, FAIL);
                    }
                }
            }
            void UpdateAI(uint32 const diff)
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
                        case EVENT_FLAMING_SLASH:
                            me->CastSpell(me, SPELL_FLAMING_SLASH_DUMMY);                            
                            events.ScheduleEvent(EVENT_FLAMING_SLASH, flamingslashinterval);
                            break;
                        case EVENT_LAVA_SWEEP:
                            Talk(SAY_SPELL01_MAKOGG);              
                            me->SummonCreature(TRIGGER_LAVA_SWEEP, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 8000);
                            events.ScheduleEvent(EVENT_LAVA_SWEEP, lavasweepinterval);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        private:

        };
        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_grimrail_makoggAI(creature);
        }
};
class iron_docks_lava_sweep_trigger : public CreatureScript
{
public:
    iron_docks_lava_sweep_trigger() : CreatureScript("iron_docks_lava_sweep_trigger") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* creature) : ScriptedAI(creature)    
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->setFaction(16);
        }

        uint32 interval;
        void Reset()
        {
            me->AddAura(SPELL_LAVA_SWEEP_VISUAL, me);
            me->SetSpeed(MOVE_RUN, 0.6f, true);

            if (Player* target = me->FindNearestPlayer(200.0f, true))
            {
                me->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
            }
        }
        void UpdateAI(uint32 const diff)
        {
            if (interval <= diff)
            {
                std::list<Player*> pl_list;
                me->GetPlayerListInGrid(pl_list, 3.0f);

                for (auto itr : pl_list)
                {
                    me->CastSpell(itr, SPELL_LAVA_SWEEP_DAMAGE);

                    AuraPtr sweepPTR = itr->GetAura(SPELL_LAVA_SWEEP_DAMAGE);

                    if (sweepPTR)
                    {
                        sweepPTR->SetDuration(2);
                    }
                }            
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_iron_docksAI(creature);
    }
};
class spell_flaming_slash_damage_targets : public SpellScriptLoader
{
public:
    spell_flaming_slash_damage_targets() : SpellScriptLoader("spell_flaming_slash_damage_targets") { }

    class iron_docks_spells : public SpellScript
    {
        PrepareSpellScript(iron_docks_spells);

        /*
        void CorrectTargets(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
                return;

            if (!GetCaster())
                return;

            targets.clear();

            std::list<Player*> list_targets;
            list_targets.clear();

            GetCaster()->GetPlayerListInGrid(list_targets, 100.0f);

            for (auto itr : list_targets)
            {
                targets.push_back(itr->ToUnit());
            }         
        }
        */
        void HandleAfterCast()
        {
            if (GetCaster())
            {
                std::list<Player*> list_targets;
                list_targets.clear();

                GetCaster()->GetPlayerListInGrid(list_targets, 100.0f);

                for (auto itr : list_targets)
                {
                    GetCaster()->CastSpell(GetCaster(), SPELL_FLAMING_SLASH_DAMAGE);
                }

                GetCaster()->RemoveAura(163665);
                GetCaster()->RemoveAura(163669);
            }
        }
        void Register()
        {
            AfterCast += SpellCastFn(iron_docks_spells::HandleAfterCast);
            /*
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(iron_docks_spells::CorrectTargets, EFFECT_0, TARGET_UNIT_TARGET_ANY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(iron_docks_spells::CorrectTargets, EFFECT_1, TARGET_UNIT_TARGET_ANY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(iron_docks_spells::CorrectTargets, EFFECT_2, TARGET_UNIT_TARGET_ANY);
            */
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new iron_docks_spells();
    }
};
class spell_flaming_slash_damage_target_change : public SpellScriptLoader
{
public:
    spell_flaming_slash_damage_target_change() : SpellScriptLoader("spell_flaming_slash_damage_target_change") { }

    class iron_docks_spells : public SpellScript
    {
        PrepareSpellScript(iron_docks_spells);

        bool Load()
        {
            SpellInfo* spell = const_cast<SpellInfo*>(GetSpellInfo());
            spell->Effects[0].TargetA = 87;
            spell->Effects[0].TargetB = 0;
            return true;
        }
        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(iron_docks_spells::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnEffectHitTarget += SpellEffectFn(iron_docks_spells::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnEffectHitTarget += SpellEffectFn(iron_docks_spells::HandleDamage, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new iron_docks_spells();
    }
};
class boss_grimrail_noxx : public CreatureScript
{
public:
    boss_grimrail_noxx() : CreatureScript("boss_grimrail_noxx") { }

    struct boss_grimrail_noxxAI : public BossAI
    {
        boss_grimrail_noxxAI(Creature* creature) : BossAI(creature, DATA_GRIMRAIL_NOXX)
        {
            pinstance = me->GetInstanceScript();
        }

        InstanceScript* pinstance;
        bool intro;
        void Reset() override
        {
            _Reset();
            intro = false;
        }
        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->IsInWorld() && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 18.0f) && !intro)
            {
                intro = true;
                Talk(SAY_INTRO);
            }
        }
        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
    
            events.ScheduleEvent(EVENT_OGRE_TRAP, ogretrapsinterval);
            events.ScheduleEvent(EVENT_JUMPER_CABLES, jumpercablesinterval);
            events.ScheduleEvent(EVENT_GUT_SHOT, gutshotinterval);
            events.ScheduleEvent(EVENT_BOMB_SQUAD, bombsquadinterval);
        }
        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_SLAY);
        }
        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);

                if (Creature* Makog = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_MAKOGG)))
                {
                    if (Creature* DUGURU = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_DUGURU)))
                    {
                        if (DUGURU->isDead() && Makog->isDead())
                            DUGURU->m_Events.AddEvent(new aftergrimrail_event(DUGURU, 0), DUGURU->m_Events.CalculateTime(8000));
                    }
                }
        }
        void JustReachedHome()
        {
            if (Creature* makogg = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_MAKOGG)))
            {
                if (Creature* DUGURU = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_DUGURU)))
                {
                    makogg->Respawn();
                    makogg->GetAI()->Reset();

                    DUGURU->Respawn();
                    DUGURU->GetAI()->Reset();

                    instance->SetBossState(DATA_GRIMRAIL_MAKOGG, FAIL);
                }
            }
        }
        void UpdateAI(uint32 const diff)
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
                case EVENT_OGRE_TRAP:           
                    Talk(SAY_SPELL03);

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            Creature* trap = me->SummonCreature(CREATURE_OGRE_TRAP, target->GetPositionX() + urand(i, i + urand(0.30f, 0.45f)), target->GetPositionY() + urand(i, i + 2.0f), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            //trap->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 10.0f, 10.0f, 10.0f);
                        }
                    }
                    events.ScheduleEvent(EVENT_OGRE_TRAP, ogretrapsinterval);
                    break;
                case EVENT_JUMPER_CABLES:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0F, true))
                        me->CastSpell(target, SPELL_JUMPER_CABLES);

                    Talk(SAY_SPELL01);
                    events.ScheduleEvent(EVENT_JUMPER_CABLES, jumpercablesinterval);
                    break;
                case EVENT_GUT_SHOT:
                    me->CastSpell(me->getVictim(), SPELL_GUT_SHOT);
                    events.ScheduleEvent(EVENT_GUT_SHOT, gutshotinterval);
                    break;
                case EVENT_BOMB_SQUAD:
                    Position pos;
                    me->GetRandomNearPosition(pos, 30.0f);

                    Talk(SAY_SPELL02);
                    me->SummonCreature(BOMB_SQUAD_TRIGGER, pos, TEMPSUMMON_MANUAL_DESPAWN);
                    events.ScheduleEvent(EVENT_BOMB_SQUAD, bombsquadinterval);
                    break;
                }
            }
        }
    private:

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grimrail_noxxAI(creature);
    }
};
class iron_docks_ogre_trap : public CreatureScript
{
public:
    iron_docks_ogre_trap() : CreatureScript("iron_docks_ogre_trap") { }

    struct mob_iron_docksAI : public Scripted_NoMovementAI
    {
        mob_iron_docksAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

        int interval;
        bool activated;
        uint32 despawn;
        void Reset()
        {
            activated = false;
            me->CastSpell(me, SPELL_OGRE_TRAP_OPEN_TEETH);
     

            despawn = 12000;
        }
        void UpdateAI(uint32 const diff)
        {
            if (!activated)
            {
                std::list<Player*> pl_list;
                me->GetPlayerListInGrid(pl_list, 1.0f);

                if (pl_list.empty())
                    return;

                for (auto itr : pl_list)
                {  
                    activated = true;
                    me->RemoveAllAuras();
                    me->AddAura(SPELL_OGRE_TRAP_CLOSED_TEETH, me);
                    me->AddAura(SPELL_SHREDDING_TENDONS, itr);
                    me->DespawnOrUnsummon(6000);
                }
            }
            if (despawn <= diff)
            {
                despawn = 50000;
                me->DespawnOrUnsummon(6000);
            }
            else
                despawn -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_iron_docksAI(creature);
    }
};
class iron_docks_bomb_trap : public CreatureScript
{
public:
    iron_docks_bomb_trap() : CreatureScript("iron_docks_bomb_trap") { }

    struct mob_iron_docksAI : public Scripted_NoMovementAI
    {
        mob_iron_docksAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
        }

        uint32 explosion;
        void Reset()
        {
            me->CastSpell(me, SPELL_BIG_BOOM, false);
            /*
            damage is bugged, the src is badly damaged. alot of stuff needs to be hardcoded
            */
        }
        void UpdateAI(uint32 const diff)
        {/*
            if (explosion <= diff)
            {
                std::list<Player*> pl_list;

                me->GetPlayerListInGrid(pl_list, 5.0f);



                for (auto itr : pl_list)
                {
                    
                    damage is bugged, the src is badly damaged. alot of stuff needs to be hardcoded
                    
                    itr->ToPlayer()->EnvironmentalDamage(DAMAGE_FIRE, urand(15000, 21000));
                }
            }
            else
            {
                explosion -= diff;
            }
            */
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_iron_docksAI(creature);
    }
};
// 98928 
class iron_docks_flame_wave_restriction : public SpellScriptLoader
{
public:
    iron_docks_flame_wave_restriction() : SpellScriptLoader("iron_docks_flame_wave_restriction") { }

    class iron_docks_spells : public SpellScript
    {
        PrepareSpellScript(iron_docks_spells);

        bool Load()
        {
            if (GetCaster() && GetCaster()->GetMapId() == 1195)
            {
                SpellInfo* spell = const_cast<SpellInfo*>(GetSpellInfo());
                spell->Effects[0].TargetA = 0;
                spell->Effects[0].TargetB = 0;
                spell->Effects[2].TargetA = 0;
                spell->Effects[2].TargetB = 0;
                return true;
            }
        }
        SpellCastResult CheckTarget()
        {
            if (GetCaster() && GetCaster()->GetMapId() == 1195)
            {
                return SPELL_FAILED_DONT_REPORT;
            }
            else
                return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(iron_docks_spells::CheckTarget);
        }
    };
};

class boss_grimrail_duguru : public CreatureScript
{
public:
    boss_grimrail_duguru() : CreatureScript("boss_grimrail_duguru") { }

    struct boss_grimrail_duguruAI : public BossAI
    {
        boss_grimrail_duguruAI(Creature* creature) : BossAI(creature, DATA_GRIMRAIL_DUGURU)
        {
            pinstance = me->GetInstanceScript();
        }

        InstanceScript* pinstance;
        void Reset() override
        {
            _Reset();
        }
        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            events.ScheduleEvent(EVENT_BLOOD_BOLT, bloodboltinterval);
            events.ScheduleEvent(EVENT_SANGUINE_SPHERE, sanguinesphereinterval);
            events.ScheduleEvent(EVENT_TAINTED_BLOOD, taintedbloodinterval);
        }
        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_SLAY);
        }
        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);

            if (Creature* Makog = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_MAKOGG)))
            {
                if (Creature* noxx = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_NOXX)))
                {
                    if (noxx->isDead() && Makog->isDead())
                        noxx->m_Events.AddEvent(new aftergrimrail_event(noxx, 0), noxx->m_Events.CalculateTime(8000));
                }
            }
        }
        void JustReachedHome()
        {
            if (Creature* makogg = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_MAKOGG)))
            {
                if (Creature* noxx = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_NOXX)))
                {
                    makogg->Respawn();
                    makogg->GetAI()->Reset();

                    noxx->Respawn();
                    noxx->GetAI()->Reset();

                    instance->SetBossState(DATA_GRIMRAIL_MAKOGG, FAIL);
                }
            }
        }
        void UpdateAI(uint32 const diff)
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
                case EVENT_BLOOD_BOLT:
                    me->CastSpell(me->getVictim(), SPELL_BLOOD_BOLT);
                    events.ScheduleEvent(EVENT_BLOOD_BOLT, bloodboltinterval + 2500);
                    break;
                case EVENT_SANGUINE_SPHERE:
                    if (Creature* makogg = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_MAKOGG)))
                    {
                        if (Creature* noxx = pinstance->instance->GetCreature(pinstance->GetData64(DATA_GRIMRAIL_NOXX)))
                        {
                            if (makogg->GetHealthPct() > noxx->GetHealthPct())
                            {
                                me->AddAura(SPELL_SANGUINE_SPHERE, makogg);
                            }
                            else
                            {
                                me->AddAura(SPELL_SANGUINE_SPHERE, noxx);
                            }
                        }
                    }
                    events.ScheduleEvent(EVENT_SANGUINE_SPHERE, sanguinesphereinterval);
                    break;
                case EVENT_TAINTED_BLOOD:
                    if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                        me->CastSpell(target, SPELL_TAINTED_BLOOD);
                    events.ScheduleEvent(EVENT_TAINTED_BLOOD, taintedbloodinterval);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grimrail_duguruAI(creature);
    }
};
void AddSC_boss_grimrail()
{
    // bosses
    new boss_grimrail_noxx();
    new boss_grimrail_makogg();
    new boss_grimrail_duguru();

    // triggers
    new iron_docks_lava_sweep_trigger();
    new iron_docks_ogre_trap();
    new iron_docks_bomb_trap();

    // spells
    new spell_flaming_slash_damage_targets();
    //new iron_docks_ogre_trap_throw();
    new iron_docks_flame_wave_restriction();
    new spell_flaming_slash_damage_target_change();
}