#ifndef ACTION_H
#define ACTION_H

#include <stdint-gcc.h>
#include "subcontroller.h"
#include "data_subcontroller.h"

class Action
{
public:
    static Action* GetInstance() { return m_UniqueInstance; }

    void action(uint32_t acts, uint8_t asMany_N_acts, useconds_t delay_steps);

private:
    static Action* m_UniqueInstance;

    uint8_t lastStatus;
};

#endif // ACTION_H
