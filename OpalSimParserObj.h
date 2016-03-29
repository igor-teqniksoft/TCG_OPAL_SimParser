//*****************************************************************************
//OpalSimParserObj.h
//*****************************************************************************
#ifndef _OPAL_SIM_PARSER_OBJ_
#define _OPAL_SIM_PARSER_OBJ_

#include "OpalSimParser.h"

class CToken
{
    TOKEN token;

 public:

    void GetTokenFromBuf(uint8_t *);

    uint8_t GetTokenType();
    uint32_t GetDataLength();
    uint32_t GetTokenLength();
    uint8_t* GetBufPtr();

    bool IsSigned();
    bool IsSequence();
    bool IsFinal();

    uint64_t GetUint64();
    uint32_t GetUint32();
    uint16_t GetUint16();
    uint8_t GetUint8();
};

template<class T>
class CList
{
    struct Node 
    {
        T data;
        Node *p_next;
        Node *p_prev;
    };

    uint32_t nodes_cnt;
    Node *p_head;
    Node *p_tail;

 public:
    CList() 
    {
        p_head = NULL;
        p_tail = NULL;
        nodes_cnt = 0;
    }

    ~CList() 
    {
        DeleteList();
    }
    
    void AddNode(T);
    void DeleteList();

    class iterator;
    friend class iterator;
    class iterator
    {
        Node *p_cur;

      public:
         iterator(CList &tl) : p_cur(tl.p_head){}
         iterator(CList &tl, bool f)
         {
            if(f)
            {
                p_cur = tl.p_head;
            }
            else
            {
                p_cur = tl.p_tail;
            }
         }

         T operator*() const 
         { 
            return p_cur->data;
         }

         T operator++() 
         {
            if(p_cur != NULL)
            {
                p_cur = p_cur->p_next;
            }

            return p_cur->data;
         }

         T operator++(int)
         {
            Node *temp;
            temp = p_cur;
            if(p_cur != NULL)
            {
                p_cur = p_cur->p_next;
            }

            return temp->data;
         }

         iterator& operator+=(uint32_t i)
         {
            for(uint32_t j = 0; j < i; j++)
            {
                p_cur = p_cur->p_next;
                if(p_cur == NULL)
                {
                    break;
                }
            }

            return *this;
         }

         bool operator==(const iterator &n)
         {
             return p_cur == n.p_cur;
         }

         bool operator!=(const iterator &n)
         {
             return p_cur != n.p_cur;         
         }
    };

};

//*****************************************************************************
//Add token type to the list tail
//*****************************************************************************
template<class T>
void CList<T>::AddNode(T value)
{
    Node * p_nd = new Node;
    p_nd->data = value;

    if(p_head == NULL) //if it was first added element
    {
        p_head = p_tail = p_nd;
        p_nd->p_prev = NULL;
        p_nd->p_next = NULL;
    }
    else //for not first element
    {
        p_nd->p_prev = p_tail;
        p_nd->p_next = NULL;
        p_tail->p_next = p_nd;
        p_tail = p_nd;
    }

    ++nodes_cnt;
}

//*****************************************************************************
//Release allocated memory and delete all elements from the list
//*****************************************************************************
template<class T>
void CList<T>::DeleteList()
{
    while(p_head != NULL) 
    {
        Node * p_nd = p_head->p_next;
        delete p_head;
        p_head = p_nd;
    }
}

class CTokenList: public CList<CToken>
{
    uint8_t *p_data_payload;
    uint32_t length;

public:
    CTokenList()
    {
        p_data_payload = NULL;
        length = 0;
    }
     
    CTokenList(uint8_t* buf, uint32_t lgth)
    {
        LoadDataPayload(buf, lgth);
    }

    CTokenList::iterator GetBegin()
    {
        CTokenList::iterator it(*this, true);
        return it;
    }

    CTokenList::iterator GetEnd()
    {
        CTokenList::iterator it(*this, false);
        return it;
    }

    void LoadDataPayload(uint8_t*, uint32_t);

};

uint32_t GetUint32(CToken);

#endif //_OPAL_SIM_PARSER_OBJ_
