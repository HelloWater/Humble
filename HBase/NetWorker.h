
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "HStruct.h"
#include "HEnum.h"
#include "NetParser.h"
#include "NetBase.h"
#include "RWLock.h"

H_BNAMSP

class CNetWorker : public CNetBase
{
private:
    enum
    {
        NET_CMD_ADDLINK = 0,
        NET_CMD_DELLINK
    };
    struct H_WorkerCMD
    {
        char cCmd;
        bool bAccept;
        class CParser *pParser;
        H_LINK stLink;
    };

public:
    CNetWorker(void);
    ~CNetWorker(void);

    size_t onOrder(CEvBuffer *pEvBuffer);
    H_INLINE bool addLink(H_SOCK &sock, class CParser *pParser, const unsigned short &usType, const bool &bAccept)
    {
        H_WorkerCMD stCMD;
        stCMD.cCmd = NET_CMD_ADDLINK;
        stCMD.bAccept = bAccept;
        stCMD.stLink.usType = usType;
        stCMD.pParser = pParser;
        stCMD.stLink.sock = sock;

        return sendOrder((const void*)&stCMD, sizeof(stCMD));
    };
    H_INLINE bool closeLink(H_SOCK &sock)
    {
        H_WorkerCMD stCMD;
        stCMD.cCmd = NET_CMD_DELLINK;
        stCMD.stLink.sock = sock;

        return sendOrder((const void*)&stCMD, sizeof(stCMD));
    };

    void onLinked(H_Session *pSession);
    void onAccept(H_Session *pSession);
    void onRead(H_Session *pSession);
    void onClose(H_Session *pSession);

    static void tcpReadCB(struct bufferevent *bev, void *arg);
    static void tcpEventCB(struct bufferevent *bev, short, void *arg);

private:
    H_DISALLOWCOPY(CNetWorker);
    H_INLINE void addEvent(H_WorkerCMD *pCMD) 
    {
        (void)evutil_make_socket_nonblocking(pCMD->stLink.sock);

        if (SOCKTYPE_HTTP != pCMD->stLink.usType)
        {
            (void)setsockopt(pCMD->stLink.sock, IPPROTO_TCP, TCP_NODELAY, (char *)&m_iSockFlag, sizeof(m_iSockFlag));
            H_KeepAlive(pCMD->stLink.sock, H_SOCKKEEPALIVE_IDLE, H_SOCKKEEPALIVE_INTERVAL);
        }        

        H_Session *pSession = new(std::nothrow) H_Session;
        H_ASSERT(NULL != pSession, "malloc memory error.");

        pSession->pEv = bufferevent_socket_new(getBase(), pCMD->stLink.sock, BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pSession->pEv)
        {
            evutil_closesocket(pCMD->stLink.sock);
            H_SafeDelete(pSession);

            return;
        }

        pSession->m_uiPackCount = H_INIT_NUMBER;
        pSession->m_uiTime = time(NULL);
        pSession->pNetWorker = this;
        pSession->pParser = pCMD->pParser;
        pSession->stLink.usType = pCMD->stLink.usType;
        pSession->stLink.sock = pCMD->stLink.sock;
        pSession->bReLink = !(pCMD->bAccept);
        pSession->bHandShake = false;
        pSession->bMQTTConnected = false;
        if (SOCKTYPE_WS == pCMD->stLink.usType
            || SOCKTYPE_WSMQTT == pCMD->stLink.usType)
        {
            pSession->bHandShake = true;
        }

        bufferevent_setcb(pSession->pEv, tcpReadCB, NULL, tcpEventCB, pSession);
        (void)bufferevent_enable(pSession->pEv, EV_READ);

        if (pCMD->bAccept)
        {
            onAccept(pSession);
        }
        else
        {
            onLinked(pSession);
        }
    };

    bool checkRate(H_Session *pSession);

    void dispProto(H_Session *pSession, H_TCPBUF &stTcpBuf, H_Binary &stBinary, bool &bClose);

    void dispHttp(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void dispNomal(H_PROTOTYPE &iProto, H_TCPBUF &stTcpBuf, H_Binary &stBinary);

    void dispRPCCall(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void dispRPCRtn(H_TCPBUF &stTcpBuf, H_Binary &stInBinary);

    void dispCMD(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void sendCMD(const char *pszTaskName, H_LINK *pLink, H_CMD *pCmd, H_TCPBUF &stTcpBuf);

    void dispMQTT(H_Session *pSession, H_TCPBUF &stTcpBuf, H_Binary &stBinary, bool &bClose);

private:
    int m_iSockFlag;
};

H_ENAMSP

#endif//H_NETWORKER_H_
