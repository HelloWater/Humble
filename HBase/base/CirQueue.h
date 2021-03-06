
#ifndef H_CIRQUEUE_H_
#define H_CIRQUEUE_H_

#include "HMutex.h"

H_BNAMSP

//����
class CCirQueue : public CObject
{
public:
    CCirQueue(void) : m_uiSize(H_INIT_NUMBER), m_uiNext(H_INIT_NUMBER), m_uiCapacity(H_ONEK)
    {
        m_pData = new(std::nothrow) void*[m_uiCapacity];
        H_ASSERT(NULL != m_pData, H_ERR_MEMORY);
    };
    explicit CCirQueue(const size_t &uiCapacity) : m_uiSize(H_INIT_NUMBER), m_uiNext(H_INIT_NUMBER),
        m_uiCapacity(uiCapacity)
    {
        m_pData = new(std::nothrow) void*[m_uiCapacity];
        H_ASSERT(NULL != m_pData, H_ERR_MEMORY);
    };
    ~CCirQueue(void)
    {
        H_SafeDelArray(m_pData);
    };

    H_INLINE void setCapacity(const size_t &uiCapacity)
    {
        if (uiCapacity <= m_uiCapacity)
        {
            return;
        }

        void **pNewData = new(std::nothrow) void*[uiCapacity];
        H_ASSERT(NULL != pNewData, H_ERR_MEMORY);

        memcpy(pNewData + m_uiNext, m_pData + m_uiNext, m_uiSize * sizeof(void*));
        H_SafeDelArray(m_pData);

        m_uiCapacity = uiCapacity;
        m_pData = pNewData;
    };
    H_INLINE void Push(void *pValue)
    {
        if (m_uiSize >= m_uiCapacity)
        {
            setCapacity(m_uiCapacity * 2);
        }

        size_t iPos(m_uiNext + m_uiSize);
        if (iPos >= m_uiCapacity)
        {
            iPos -= m_uiCapacity;
        }

        m_pData[iPos] = pValue;
        ++m_uiSize;
    };
    H_INLINE void *Pop(void)
    {
        void *pValue(NULL);

        if (m_uiSize > H_INIT_NUMBER)
        {
            pValue = m_pData[m_uiNext];
            ++m_uiNext;
            --m_uiSize;
            if (m_uiNext >= m_uiCapacity)
            {
                m_uiNext -= m_uiCapacity;
            }
        }

        return pValue;
    };
    H_INLINE void* Peek(const size_t &uiIndex)
    {
        void *pValue(NULL);

        if (m_uiSize > H_INIT_NUMBER
            && uiIndex < m_uiSize)
        {
            pValue = m_pData[m_uiNext + uiIndex];
        }

        return pValue;
    };
    H_INLINE const size_t &Size(void)
    {
        return m_uiSize;
    };

private:
    size_t m_uiSize;
    size_t m_uiNext;
    size_t m_uiCapacity;
    void **m_pData;
};

class CSafeQueue : public CObject
{
public:
    CSafeQueue(void)
    {};
    explicit CSafeQueue(const size_t uiCapacity) : m_objQu(uiCapacity)
    {};
    ~CSafeQueue(void)
    {};

    void Push(void *pValue)
    {
        m_objLck.Lock();
        m_objQu.Push(pValue);
        m_objLck.unLock();
    };
    void *Pop(void)
    {
        m_objLck.Lock();
        void *pValue(m_objQu.Pop());
        m_objLck.unLock();

        return pValue;
    };
    void* Peek(const int &iIndex)
    {
        m_objLck.Lock();
        void *pValue(m_objQu.Peek(iIndex));
        m_objLck.unLock();

        return pValue;
    };
    size_t Size(void)
    {
        m_objLck.Lock();
        size_t iSize(m_objQu.Size());
        m_objLck.unLock();

        return iSize;
    };

private:
    CCirQueue m_objQu;
    CMutex m_objLck;
};

H_ENAMSP

#endif//H_CIRQUEUE_H_
