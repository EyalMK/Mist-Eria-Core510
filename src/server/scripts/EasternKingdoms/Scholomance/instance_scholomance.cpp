#include "scholomance.h"

/** Global C++ file for everything related to the script of the instance itself **/

/// Loader of the InstanceScript
class instance_scholomance : public InstanceMapScript {
public :
	/// Default and only constructor, based on InstanceMapScript one
	instance_scholomance() : InstanceMapScript("instance_scholomance", 1007) {
	
	}
	
	/// Script for the instance
	class instance_scholomance_InstanceScript : public InstanceScript {
	public :
		/// Default and only constructor, use InstanceScript constructor
		instance_scholomance_InstanceScript(Map* map) : InstanceScript(map) {
		
		}
		
		/// Override InstanceScript::Initialize
		/// Called only during Initialization phase, not on Load
		/// Initialize every datas here
		void Initialize() {
		
		}
		
		/// Set the given @a data to the givent @a value
		/// @param data : an arbitrary value serving as an index
		/// @param value : the value given to the data
		/// @overload : InstanceScript::SetData
		void SetData(uint32 data, uint32 value) {
		
		}
		
		/// Set the givent @a data data to the given @value
		/// @sa SetData
		void SetData64(uint32 data, uint64 value) {
		
		}
		
		/// Returns the value of the given @a data
		uint32 GetData(uint32 data) {
		
		}
		
		/// Returns the value of the givent @a data
		uint64 GetData64(uint32 data) {
		
		}
		
		/// Read the given @a data to load the InstanceScript
		/// Use a std::istringsteam to proceed
		/// Do not confuse with Initialize
		void Load(const char* data) {
		
		}
		
		/// Export the datas wanted to be saved as a std::string
		/// Use a std::ostringstream to proceed
		std::string GetSaveData() {
		
		}
		
	private :
		
	};
	
	/// Returns a full valid pointer to the InstanceScript above
	InstanceScript* GetInstanceScript(InstanceMap* instance) const {
		return new instance_scholomance_InstanceScript(instance);
	}
};

void AddSC_instance_scholomance() {
	new instance_scholomance();
}