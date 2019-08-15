#pragma once

#include "stm32f4xx_hal.h"

class Lock
{
public:
    Lock(volatile uint32_t *lock);
    ~Lock();
public:
    volatile uint32_t *m_lock;
};