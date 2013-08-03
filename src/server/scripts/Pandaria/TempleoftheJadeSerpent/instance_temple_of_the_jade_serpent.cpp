#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "temple_of_the_jade_serpent.h"


class instance_temple_of_the_jade_serpent : public InstanceMapScript
{
    public:
        instance_temple_of_the_jade_serpent() : InstanceMapScript("instance_temple_of_the_jade_serpent", 859) { }

        struct instance_temple_of_the_jade_serpent_InstanceMapScript : public InstanceScript
        {
            instance_temple_of_the_jade_serpent_InstanceMapScript(Map* map) : InstanceScript(map)
			{}
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
