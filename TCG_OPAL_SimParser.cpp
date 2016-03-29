//*****************************************************************************
//TCG_OPAL_SimParser.cpp
//*****************************************************************************

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OpalSimParserObj.h"
using namespace std;

uint32_t TPer_Properties_req[] = {
0x00000000, 0x07FE0000, 0x00000000, 0x00000000,
0x000000D0, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x000000B8, 0x00000000,
0x00000000, 0x000000AB, 0xF8A80000, 0x00000000,
0x00FFA800, 0x00000000, 0x00FF01F0, 0xF200F0F2,
0xD0104D61, 0x78436F6D, 0x5061636B, 0x65745369,
0x7A658210, 0x00F3F2D0, 0x184D6178, 0x52657370,
0x6F6E7365, 0x436F6D50, 0x61636B65, 0x7453697A,
0x65821000, 0xF3F2AD4D, 0x61785061, 0x636B6574,
0x53697A65, 0x820FECF3, 0xF2AF4D61, 0x78496E64,
0x546F6B65, 0x6E53697A, 0x65820FC8, 0xF3F2AA4D,
0x61785061, 0x636B6574, 0x7301F3F2, 0xAD4D6178,
0x53756270, 0x61636B65, 0x747301F3, 0xF2AA4D61,
0x784D6574, 0x686F6473, 0x01F3F1F3, 0xF1F9F000,
0x0000F100, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000
};

uint8_t *tcg_send_packet;

COM_PACKET com_packet;
PACKET packet;
DATA_SUB_PACKET data_sub_packet;

uint8_t *p_data_payload;

//*****************************************************************************
//Get and convert payload as we have it from the real host
//*****************************************************************************
void GetPayload(uint32_t *source, uint8_t *dest, uint32_t size)
{
    for(unsigned int i =0; i < size; i++)
    {
        ((uint32_t*)dest)[i] = ChangeEndianness_U32(source[i]);
    }
}

//*****************************************************************************
//Helpful functions for printing info about the token
//*****************************************************************************
std::ostream& operator << (std::ostream& out, const OPAL_TOKENS& t)
{
    switch(t)
    {
        case START_LIST:        return(out << "START_LIST");
        case END_LIST:          return(out << "END_LIST");
        case START_NAME:        return(out << "START_NAME");
        case END_NAME:          return(out << "END_NAME");
        case CALL:              return(out << "CALL");
        case END_OF_DATA:       return(out << "END_OF_DATA");
        case END_OF_SESSION:    return(out << "END_OF_SESSION");
        case START_TRANSMISSION:return(out << "START_TRANSMISSION");
        case END_TRANSACTION:   return(out << "END_TRANSMISSION");
        case EMPTY_ATOM:        return(out << "EMPTY_ATOM");
        case RESERVED_1:
        case RESERVED_2:
        case RESERVED_3:
        case RESERVED_4:
        case RESERVED_5:
        case RESERVED_6:        return(out << "RESERVED");
        case TINY_ATOM:         return(out << "TINY_ATOM");
        case SHORT_ATOM:        return(out << "SHORT_ATOM");
        case MEDIUM_ATOM:       return(out << "MEDIUM_ATOM");
        case LONG_ATOM:         return(out << "LONG_ATOM");
        default:                return(out << "UNKNOWN");
    }

   return (out);
}

void PrintTokenInfo(CToken &token)
{
    OPAL_TOKENS tkn;

    cout << boolalpha << showbase << uppercase << endl;
    tkn = static_cast<OPAL_TOKENS>(token.GetTokenType());
    cout << "Type         = " << hex << static_cast<int>(token.GetTokenType()) << " : " << tkn << endl;
    cout << "Is sign      = " << token.IsSigned() << endl; 
    cout << "Is sequence  = " << token.IsSequence() << endl;
    cout << "Is final     = " << token.IsFinal() << endl;
    cout << "Token length = " << dec << token.GetTokenLength() << endl;
    cout << "Data length  = " << dec << token.GetDataLength() << endl;

    uint8_t *buf = token.GetBufPtr();
    cout << showbase << hex;
    for(uint32_t i=0; i<token.GetDataLength(); i++)
    {
        if((i % 16) == 0 && i != 0)
        {
            cout << endl;
        }

        cout << static_cast<uint32_t>(buf[i]) << " ";
    }
    cout << noboolalpha << noshowbase << nouppercase << dec << endl;
}

//*****************************************************************************
//Main
//*****************************************************************************
int main()
{
    tcg_send_packet = (uint8_t*) malloc(sizeof(TPer_Properties_req));
    if(tcg_send_packet == NULL)
    {
        printf("FAIL: Unable to allocate memory for tcg_send_packet \n");
        return 1;
    }
    memset(tcg_send_packet, 0x00, sizeof(TPer_Properties_req));
	GetPayload(TPer_Properties_req, tcg_send_packet, sizeof(TPer_Properties_req)/sizeof(uint32_t));

	GetComPacket(tcg_send_packet, &com_packet);
	GetPacket(tcg_send_packet, &packet);
	GetDataSubPacket(tcg_send_packet, &data_sub_packet);

	p_data_payload = GetDataPayload(tcg_send_packet);

//*****************************************************************************
//Example for C++ parsing version
//*****************************************************************************
    CTokenList *p_token_list = new(CTokenList);
    p_token_list->LoadDataPayload(p_data_payload, data_sub_packet.Length);
    CTokenList::iterator it_begin = p_token_list->GetBegin();
    CTokenList::iterator it_end = p_token_list->GetEnd();
    CToken obj_token;

    while(it_begin != it_end)
    {
        obj_token = it_begin++;
        PrintTokenInfo(obj_token);
    }

    delete(p_token_list);

//*****************************************************************************
//Example for ANSI C parser version
//*****************************************************************************
    TOKEN token;

    GetToken(p_data_payload, &token);

	switch(token.token_type)
	{
		case CALL:
			if(!CallTokenHandler(p_data_payload + 1))
			{
			    printf("FAIL: Packet error \n");

			}
			break;

		default:
            printf("token %d \n", token.token_type);

	}

	free(tcg_send_packet);
}
