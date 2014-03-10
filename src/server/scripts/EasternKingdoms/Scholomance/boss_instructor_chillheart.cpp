#include "scholomance.h"

/** Global C++ file for everything directly related to the boss Instructor Chillheart **/

/// Gloabl namespace to prevent objects lying everywhere
/// Use @code using namespace InstructorChillheart ; @endcode if you don't want to type InstructorChillheart::Object each time
namespace InstructorChillheart {
	enum Creatures {
	
	};
	
	enum GameObjects {
	
	};
	
	enum Spells {
	
	};
	
	enum Events {
	
	};
	
	enum Actions {
	
	};
	
	// Eventually, positions and maybe some predicates
} // namespace InstructorChillheart

class boss_instructor_chillheart : public CreatureScript {
public :
	boss_instructor_chillheart() : CreatureScript("boss_instructor_chillheart") {
	
	}
	
	class boss_instructor_chillheart_AI : public ScriptedAI {
	public :
		boss_instructor_chillheart_AI(Creature* creature) : ScriptedAI(creature) {
			_instance = creature->GetInstanceScript();
		}
		
		void Reset() {
			if(_instance)
				_instance->SetData(DATA_INSTRUCTOR_CHILLHEART, NOT_STARTED);
		
			_events.Reset();
		}
		
		void EnterCombat(Unit* aggro) {
			if(_instance)
				_instance->SetData(DATA_INSTRUCTOR_CHILLHEART, IN_PROGRESS);
		}
		
		void EnterEvadeMode() {
			if(_instance)
				_instance->SetData(DATA_INSTRUCTOR_CHILLHEART, FAIL);
				
			ScriptedAI::EnterEvadeMode();
		}
		
		void JustDied(Unit* killer) {
			if(_instance)
				_instance->SetData(DATA_INSTRUCTOR_CHILLHEART, DONE);
		}
		
		void KilledUnit(Unit* killed) {
		
		}
		
		void UpdateAI(const uint32 diff) {
			if(!UpdateVictim())
				return ;
				
			_events.Update(diff);
			
			if(me->HasUnitState(UNIT_STATE_CASTING))
				return ;
			
			while(uint32 eventId = _events.ExecuteEvent()) {
				switch(eventId) {
				
				default :
					break ;
				}
			}
			
			DoMeleeAttackIfReady();
		}
		
	private :
		InstanceScript* _instance ;
		EventMap _events ;
	};
	
	CreatureAI* GetAI(Creature* creature) const {
		return new boss_instructor_chillheart_AI(creature);
	}
};

void AddSC_boss_instructor_chillheart() {
	new boss_instructor_chillheart();
}