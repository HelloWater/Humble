
#ifndef H_RSA_H_
#define H_RSA_H_

#include "RSAKey.h"

H_BNAMSP

//RSA�ӽ���
class CRSA : public CObject
{
public:
    CRSA(void);
    ~CRSA(void);

    /*��Կ����,��ҪR_RANDOM_STRUCT�ṹ��*/
    void setKey(CRSAKey *pKey);

    /*��Կ����*/
    std::string pubEncrypt(const char* pszData, const size_t iLens);
    /*˽Կ����*/
    std::string priDecrypt(const char* pszData, const size_t iLens);

    /*˽Կ����*/
    std::string priEncrypt(const char* pszData, const size_t iLens);
    /*��Կ����*/
    std::string pubDecrypt(const char* pszData, const size_t iLens);

private:
    /*���ܷ�ʽ*/
    enum RSAEnType
    {
        EnType_Pub = 0,//��Կ����
        EnType_Pri   //˽Կ����
    };
    /*���ܷ�ʽ*/
    enum RSADeType
    {
        DeType_Pub = 0,//��Կ����
        DeType_Pri   //˽Կ����
    };

private:
    std::string RSAEncrypt(RSAEnType emEnType, const char* pszData, 
        const size_t &iDataLens);
    std::string RSADecrypt(RSADeType emEnType, const char* pszData, 
        const size_t &iDataLens);

private:
    void *m_pRandom;
    void *m_pPubKey;
    void *m_pPriKey;    
};

H_ENAMSP

#endif//H_RSA_H_