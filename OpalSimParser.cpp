//*****************************************************************************
//OpalSimParser.cpp
//*****************************************************************************
#include <iostream>
#include <stdint.h>
#include <string.h>
#include "OpalSimParser.h"
using namespace std;

uint64_t invoking_uid;
uint64_t method_uid;

//*****************************************************************************
//Convert uint16 value from big to little endianness and back
//*****************************************************************************
uint16_t ChangeEndianness_U16(uint16_t value)
{
    uint16_t result = 0;
    result |= (value & 0x00FF) << 8;
    result |= (value & 0xFF00) >> 8;
    return result;
}

//*****************************************************************************
//Convert uint32 value from big to little endianness and back
//*****************************************************************************
uint32_t ChangeEndianness_U32(uint32_t value)
{
    uint32_t result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    return result;
}

//*****************************************************************************
//Convert uint64 value from big to little endianness and back
//*****************************************************************************
uint64_t ChangeEndianness_U64(uint64_t value)
{
    uint64_t result = 0;
    result |= (value & 0x00000000000000FF) << 56;
    result |= (value & 0x000000000000FF00) << 40;
    result |= (value & 0x0000000000FF0000) << 24;
    result |= (value & 0x00000000FF000000) << 8;
    result |= (value & 0x000000FF00000000) >> 8;
    result |= (value & 0x0000FF0000000000) >> 24;
    result |= (value & 0x00FF000000000000) >> 40;
    result |= (value & 0xFF00000000000000) >> 56;
    return result;
}

//*****************************************************************************
//Identify type of token
//Fill token structure with values
//*****************************************************************************
void GetToken(uint8_t *buf, TOKEN *token)
{
    if((buf[0] & START_LIST) != START_LIST) // we have atoms here
    {
        uint8_t sign_mask;

        if((buf[0] & LONG_ATOM) == LONG_ATOM) //it's long atom token
        {
            token->token_type = LONG_ATOM;
            sign_mask = LONG_ATOM_SIGN_MASK;
            token->token_lgth = LONG_ATOM_LENGTH;
            token->data_lgth = (buf[1] << 16) | (buf[2] << 8) | (buf[3]);
            token->buf = buf + LONG_ATOM_LENGTH;
        }
        else if((buf[0] & MEDIUM_ATOM) == MEDIUM_ATOM) //it's medium atom
        {
            token->token_type = MEDIUM_ATOM;
            sign_mask = MEDIUM_ATOM_SIGN_MASK;
            token->token_lgth = MEDIUM_ATOM_LENGTH;
            token->data_lgth = ((buf[0] & (MEDIUM_ATOM_SIGN_MASK - 1)) << 8) | buf[1];
            token->buf = buf + MEDIUM_ATOM_LENGTH;
        }
        else if((buf[0] & SHORT_ATOM) == SHORT_ATOM) //it's short atom
        {
            token->token_type = SHORT_ATOM;
            sign_mask = SHORT_ATOM_SIGN_MASK;
            token->token_lgth = SHORT_ATOM_LENGTH;
            token->data_lgth = buf[0] & (SHORT_ATOM_SIGN_MASK - 1);
            token->buf = buf + SHORT_ATOM_LENGTH;
        }
        else //it's tiny atom
        {
            token->token_type = TINY_ATOM;
            token->token_lgth = TINY_ATOM_LENGTH;
            token->data_lgth = 1;
            sign_mask = TINY_ATOM_SIGN_MASK;
            token->buf = buf;
        }

        if(buf[0] & sign_mask)
        {
            token->sign  = 1;
        }
    }
    else //we have OPAL tokens here
    {
        token->token_type = buf[0];
	    token->sign = 0;
	    token->token_lgth = 1;
	    token->data_lgth = 0;
		token->buf = buf;
	}
}

//*****************************************************************************
//To get the COM_PACKET from from big endian array
//*****************************************************************************
void GetComPacket(uint8_t *buf, COM_PACKET *packet)
{
    COM_PACKET *tmp  = (COM_PACKET*)buf;

    memset(packet, 0, sizeof(COM_PACKET));

    packet->ExComID         = ChangeEndianness_U32(tmp->ExComID);
    packet->OutstandingData = ChangeEndianness_U32(tmp->OutstandingData);
    packet->MinTransfer	    = ChangeEndianness_U32(tmp->MinTransfer);
    packet->Length          = ChangeEndianness_U32(tmp->Length);
}

//*****************************************************************************
//To get PACKET from from big endian array
//*****************************************************************************
void GetPacket(uint8_t *buf, PACKET* packet)
{
    PACKET *tmp = (PACKET*)(buf + sizeof(COM_PACKET));

    memset(packet, 0, sizeof(PACKET));

    packet->Session = ChangeEndianness_U64(tmp->Session);
    packet->SeqNumber = ChangeEndianness_U32(tmp->SeqNumber);
    packet->AckType = ChangeEndianness_U16(tmp->AckType);
    packet->Acknowledgement = ChangeEndianness_U32(tmp->Acknowledgement);
    packet->Length = ChangeEndianness_U32(tmp->Length);
}

//*****************************************************************************
//To get DATA_SUB_PACKET from from big endian array
//*****************************************************************************
void GetDataSubPacket(uint8_t *buf, DATA_SUB_PACKET *packet)
{
    DATA_SUB_PACKET *tmp = (DATA_SUB_PACKET*)(buf + sizeof(COM_PACKET) + sizeof(PACKET));

    memset(packet, 0, sizeof(DATA_SUB_PACKET));

    packet->Kind = ChangeEndianness_U16(tmp->Kind);
    packet->Length = ChangeEndianness_U32(tmp->Length);
}

//*****************************************************************************
//Get Uint64 value from token
//*****************************************************************************
uint64_t GetUint64(TOKEN *token)
{
    uint64_t tmp;

    tmp = *((uint64_t*)(token->buf));

    return ChangeEndianness_U64(tmp);
}

//*****************************************************************************
//Get Uint32 value from token
//*****************************************************************************
uint32_t GetUint32(TOKEN *token)
{
	uint32_t tmp;

	tmp = *((uint32_t*)(token->buf));

	return ChangeEndianness_U32(tmp);
}

//*****************************************************************************
//Get Uint16 value from token
//*****************************************************************************
uint16_t GetUint16(TOKEN *token)
{
    uint16_t tmp;

    tmp = *((uint16_t*)(token->buf));

    return ChangeEndianness_U16(tmp);
}

//*****************************************************************************
//Get Uint8 value from token
//*****************************************************************************
uint8_t GetUint8(TOKEN *token)
{
    return *((uint8_t*)(token->buf));
}

//*****************************************************************************
//Method for handling all Call Tokens
//*****************************************************************************
bool CallTokenHandler(uint8_t *source)
{
    TOKEN token;
    uint32_t offset = 0;
    uint8_t *buf = source;

    GetToken(buf + offset, &token);
    if(token.token_type == SHORT_ATOM && token.data_lgth == sizeof(uint64_t))
    {
        invoking_uid = GetUint64(&token);
        offset += token.data_lgth + token.token_lgth; //iterator
    }
    else
    {
        return false;
    }

    GetToken(buf + offset, &token);
    if(token.token_type == SHORT_ATOM && token.data_lgth == sizeof(uint64_t))
    {
       method_uid = GetUint64(&token);
       offset += token.data_lgth + token.token_lgth; //iterator
    }
    else
    {
        return false;
    }

    //TODO handling by gotten Invoking UID and Method UID

    return true;
}


