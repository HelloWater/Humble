
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "Atomic.h"
#include "CirQueue.h"

H_BNAMSP

//��Ϣͨ��
class CChan
{
public:
    CChan(class CTaskWorker *pTask, const int iCapacity);
    ~CChan(void);

    bool Send(void *pszVal);
    void *Recv(void);

    class CTaskWorker *getTask(void);

    size_t getSize(void);
    size_t getCapacity(void);
    CAtomic *getLock(void)
    {
        return &m_objQuLck;
    };

private:
    CChan(void);
    H_DISALLOWCOPY(CChan);

private:    
    class CTaskWorker *m_pTask;
    CAtomic m_objQuLck;
    CCirQueue m_quData;
};

H_ENAMSP

#endif//H_CHAN_H_
