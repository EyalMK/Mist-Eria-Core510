#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "siege_of_niuzao_temple.h"

class instance_siege_of_niuzao_temple : public InstanceMapScript{
public :
    instance_siege_of_niuzao_temple() : InstanceMapScript("instance_siege_of_niuzao_template", 1011){
        
    }
    
    class instance_siege_of_niuzao_temple_InstanceScript : public InstanceScript{
    public :
        instance_siege_of_niuzao_temple_InstanceScript(Map * map) : InstanceScript(map){
            
        }
        
        void Initialize(){
            
        }
        
    private :
        
    };
    
    InstanceScript* GetInstanceScript(InstanceMap *instance) const{
        return new instance_siege_of_niuzao_temple_InstanceScript(instance);
    }
};

void AddSC_instance_siege_of_niuzao_temple(){
    new instance_siege_of_niuzao_temple();
}
