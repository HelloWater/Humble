
#ifndef H_CONSISTENTHASH_H_
#define H_CONSISTENTHASH_H_

#include "HObject.h"
#include "Utils.h"
#include "conhash.h"

H_BNAMSP

//consistenthashing
class CConHash : public CObject
{
public:
    CConHash(void) 
    {
        m_pConHash = conhash_init(CUtils::hashStr);
    };
    ~CConHash(void) 
    {
        if (NULL != m_pConHash)
        {
            conhash_fini((struct conhash_s *)m_pConHash);
            m_pConHash = NULL;
        }

        struct node_s *pNode;
        for (nodeit itNode = m_mapNode.begin(); m_mapNode.end() != itNode; ++itNode)
        {
            pNode = (struct node_s *)itNode->second;
            H_SafeDelete(pNode);
        }
        m_mapNode.clear();
    };

    //添加节点 pszNode 节点名   iReplica 虚拟节点数
    bool addNode(const char *pszNode, size_t iReplica) 
    {
        H_ASSERT(strlen(pszNode) < 64, "node name is too long.");
        struct node_s *pNode = new(std::nothrow) struct node_s;
        H_ASSERT(NULL != pNode, H_ERR_MEMORY);

        conhash_set_node(pNode, pszNode, (u_int)iReplica);
        if (H_RTN_OK != conhash_add_node((struct conhash_s *)m_pConHash, pNode))
        {
            H_SafeDelete(pNode);
            return false;
        }

        m_mapNode[pszNode] = pNode;
        return true;
    };
    //删除节点
    void delNode(const char *pszNode) 
    {
        nodeit itNode(m_mapNode.find(pszNode));
        if (m_mapNode.end() == itNode)
        {
            return;
        }

        struct node_s *pNode((struct node_s *)itNode->second);
        (void)conhash_del_node((struct conhash_s *)m_pConHash, pNode);

        m_mapNode.erase(itNode);
        H_SafeDelete(pNode);
    };
    //查找属于那个节点
    const char *findNode(const char *pszObject) 
    {
        struct node_s *pNode(conhash_lookup((struct conhash_s *)m_pConHash, pszObject));
        if (NULL == pNode)
        {
            return NULL;
        }

        return pNode->iden;
    };
    //虚拟节点数
    size_t getVNodeNum(void) 
    {
        return conhash_get_vnodes_num((struct conhash_s *)m_pConHash);
    };

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<std::string, void * >::iterator nodeit;
    typedef std::unordered_map<std::string, void * > node_map;
#else
    typedef std::tr1::unordered_map<std::string, void * >::iterator nodeit;
    typedef std::tr1::unordered_map<std::string, void * > node_map;
#endif

    void *m_pConHash;
    node_map m_mapNode;
};

H_ENAMSP

#endif//H_CONSISTENTHASH_H_
