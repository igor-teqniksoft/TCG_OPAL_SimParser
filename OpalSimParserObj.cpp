//*****************************************************************************
//OpalSimParserObj.cpp
//*****************************************************************************
#include <iostream>
#include <stdint.h>
#include "OpalSimParserObj.h"
using namespace std;

//*****************************************************************************
//Identify type of token
//Fill token structure with values
//*****************************************************************************
void CToken::GetTokenFromBuf(uint8_t *buf)
{
    if((buf[0] & START_LIST) != START_LIST) // we have atoms here
    {
        uint8_t sign_mask;

        if((buf[0] & LONG_ATOM) == LONG_ATOM) //it's long atom token
        {
            token.token_type = LONG_ATOM;
            sign_mask = LONG_ATOM_SIGN_MASK;
            token.token_lgth = LONG_ATOM_LENGTH;
            token.data_lgth = (buf[1] << 16) | (buf[2] << 8) | (buf[3]);
            token.buf = buf + LONG_ATOM_LENGTH;
        }
        else if((buf[0] & MEDIUM_ATOM) == MEDIUM_ATOM) //it's medium atom
        {
            token.token_type = MEDIUM_ATOM;
            sign_mask = MEDIUM_ATOM_SIGN_MASK;
            token.token_lgth = MEDIUM_ATOM_LENGTH;
            token.data_lgth = ((buf[0] & (MEDIUM_ATOM_SIGN_MASK - 1)) << 8) | buf[1];
            token.buf = buf + MEDIUM_ATOM_LENGTH;
        }
        else if((buf[0] & SHORT_ATOM) == SHORT_ATOM) //it's short atom
        {
            token.token_type = SHORT_ATOM;
            sign_mask = SHORT_ATOM_SIGN_MASK;
            token.token_lgth = SHORT_ATOM_LENGTH;
            token.data_lgth = buf[0] & (SHORT_ATOM_SIGN_MASK - 1);
            token.buf = buf + SHORT_ATOM_LENGTH;
        }
        else //it's tiny atom
        {
            token.token_type = TINY_ATOM;
            token.token_lgth = TINY_ATOM_LENGTH;
            token.data_lgth = 1;
            sign_mask = TINY_ATOM_SIGN_MASK;
            token.buf = buf;
        }

        if(buf[0] & sign_mask)
        {
            token.sign  = 1;
        }
    }
    else //we have OPAL tokens here
    {
        token.token_type = buf[0];
        token.sign = 0;
        token.token_lgth = 1;
        token.data_lgth = 0;
        token.buf = buf;
    }
}

//*****************************************************************************
//Get token type
//*****************************************************************************
uint8_t CToken::GetTokenType()
{
    return token.token_type;
}

//*****************************************************************************
//Get data sequence length
//*****************************************************************************
uint32_t CToken::GetDataLength()
{
    return token.data_lgth;
}

//*****************************************************************************
//Get token length - need it mostly for atoms
//*****************************************************************************
uint32_t CToken::GetTokenLength()
{
    return token.token_lgth;
}

//*****************************************************************************
//Get pointer to data buffer
//*****************************************************************************
uint8_t* CToken::GetBufPtr()
{
    return token.buf;
}

//*****************************************************************************
//Is the token data signed or not. Data flow can't be signed
//*****************************************************************************
bool CToken::IsSigned()
{
    bool sign_flag = false;

    if(token.seq == 0) //sequence data flow can't be signed
    {
        if(token.sign == 1)
        {
            sign_flag = true;
        }
    }

    return sign_flag;
}

//*****************************************************************************
//Check - have we the data flow or typed data
//*****************************************************************************
bool CToken::IsSequence()
{
    return (token.seq != 0);
}

//*****************************************************************************
//Check - have we final segment of byte sequence or not 
// false - final data segment
// true - non-final data segment
//*****************************************************************************
bool CToken::IsFinal()
{
    bool seg_flag = false;

    if(token.sign == 1)
    {
        if(token.seq == 1)
        {
            seg_flag = true;
        }
    }

    return seg_flag;
}

//*****************************************************************************
//Get Uint64 value from token object
//*****************************************************************************
uint64_t CToken::GetUint64()
{
    uint64_t tmp;

    tmp = *(reinterpret_cast<uint64_t*>(token.buf));

    return ChangeEndianness_U64(tmp);
}

//*****************************************************************************
//Get Uint32 value from token object
//*****************************************************************************
uint32_t CToken::GetUint32()
{
	uint32_t tmp;

    tmp = *(reinterpret_cast<uint32_t*>(token.buf));

	return ChangeEndianness_U32(tmp);
}

//*****************************************************************************
//Get Uint16 value from token object
//*****************************************************************************
uint16_t CToken::GetUint16()
{
    uint16_t tmp;

    tmp = *(reinterpret_cast<uint16_t*>(token.buf));

    return ChangeEndianness_U16(tmp);
}

//*****************************************************************************
//Get Uint8 value from token object
//*****************************************************************************
uint8_t CToken::GetUint8()
{
    return *(token.buf);
}

//*****************************************************************************
//Load the data payload, parse and save it in the list object
//*****************************************************************************
void CTokenList::LoadDataPayload(uint8_t* buf, uint32_t lgth)
{
    uint32_t itr = 0;
    CToken token_obj;

    DeleteList();

    p_data_payload = buf;
    length = lgth;

    if(buf != NULL)
    {
        while(itr <= lgth && token_obj.GetTokenType() != END_OF_DATA)
        {
            token_obj.GetTokenFromBuf(buf + itr);
            AddNode(token_obj);
            itr += token_obj.GetTokenLength() + token_obj.GetDataLength();
        }
    }
}
