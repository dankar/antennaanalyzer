#include "lock.h"

Lock::Lock(volatile uint32_t *lock)
{
    int status = 0;
    m_lock = lock;
    do {
        while (__LDREXW(m_lock) != 0);

        status = __STREXW(1, m_lock);
    
    } while (status!=0); //retry until lock successfully
    __DMB();
}
Lock::~Lock()
{
    __DMB(); // Ensure memory operations completed before
    // releasing lock
    *m_lock = 0;
}
