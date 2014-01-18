#include "stormstout_brewery.h"
#include "InstanceScript.h"

class instance_stormstout_brewery : public InstanceMapScript
{
public :
    instance_stormstout_brewery() : InstanceMapScript("instance_stormstout_brewery", 961)
    {

    }

    class instance_stormstout_brewery_InstanceScript : public InstanceScript
    {
    public :
        instance_stormstout_brewery_InstanceScript(Map* map) : InstanceScript(map)
        {
        }

        
    InstanceScript* GetInstanceScript(InstanceMap *instance) const
    {
        return new instance_stormstout_brewery_InstanceScript(instance);
    }
};

void AddSC_instance_stormstout_brewery()
{
    new instance_stormstout_brewery();
}
