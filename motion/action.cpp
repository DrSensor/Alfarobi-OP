#include "action.h"

Action* Action::m_UniqueInstance = new Action();

Action::Action()
{
}

void Action::action(uint32_t acts, uint8_t asMany_N_acts, useconds_t delay_steps)
{
    lastStatus = acts;
    for(uint8_t i=0;i<asMany_N_acts;i++) {
        port_write(acts);
        usleep(delay_steps);
    }
}
