//*****************************************************************************
//OpalSimParser.h
//*****************************************************************************
#ifndef _OPAL_SIM_PARSER_
#define _OPAL_SIM_PARSER_

typedef enum _OPAL_TOKENS_
{
    START_LIST  = 0xF0,
    END_LIST,
    START_NAME,
    END_NAME,
    RESERVED_1,
    RESERVED_2,
    RESERVED_3,
    RESERVED_4,
    CALL,
    END_OF_DATA,
    END_OF_SESSION,
    START_TRANSMISSION,
    END_TRANSACTION,
    RESERVED_5,
    RESERVED_6,
    EMPTY_ATOM
} OPAL_TOKENS;

typedef enum _ATOM_TOKENS_
{
    TINY_ATOM   = 0x00,
    SHORT_ATOM  = 0x80,
    MEDIUM_ATOM = 0xC0,
    LONG_ATOM   = 0xE0

} ATOM_TOKENS;

typedef enum _ATOM_SIGN_MASKS_
{
    TINY_ATOM_SIGN_MASK   = 0x40,
    SHORT_ATOM_SIGN_MASK  = 0x10,
    MEDIUM_ATOM_SIGN_MASK = 0x08,
    LONG_ATOM_SIGN_MASK   = 0x01
} ATOM_SIGN_MASKS;

typedef enum _TOKEN_LENGTH_
{
    TINY_ATOM_LENGTH    = 0x01,
    SHORT_ATOM_LENGTH   = 0x01,
    MEDIUM_ATOM_LENGTH   = 0x02,
    LONG_ATOM_LENGTH    = 0x04
} TOKEN_LENGTH;

typedef struct _COM_PACKET_
{
    uint32_t    Reserved;
    uint32_t    ExComID;
    uint32_t    OutstandingData;
    uint32_t    MinTransfer;
    uint32_t    Length;
} COM_PACKET;

typedef struct _PACKET_
{
    uint64_t    Session;
    uint32_t    SeqNumber;
    uint16_t    Reserved;
    uint16_t    AckType;
    uint32_t    Acknowledgement;
    uint32_t    Length;
} PACKET;

typedef struct _DATA_SUB_PACKET_
{
    uint8_t     Reserved[6];
    uint16_t    Kind;
    uint32_t    Length;
} DATA_SUB_PACKET;

typedef struct _TOKEN_
{
    uint8_t    token_type;  //token type from OPAL_TOKENS and ATOM_TOKENS
    uint8_t    sign;        //1 - signed, 0 - unsigned
    uint8_t    seq;         //1 - byte sequence, 0 - signed/unsigned
    uint8_t    token_lgth;  //token length in bytes
    uint32_t   data_lgth;   //data length in bytes
    uint8_t    *buf;        //pointer to byte or char sequence
} TOKEN;


#define GetDataPayload(buf)     ((uint8_t*)(buf + sizeof(COM_PACKET) + sizeof(PACKET) + sizeof(DATA_SUB_PACKET)))

uint64_t ChangeEndianness_U64(uint64_t);
uint32_t ChangeEndianness_U32(uint32_t);
uint16_t ChangeEndianness_U16(uint16_t);

void GetToken(uint8_t*, TOKEN*);
void GetComPacket(uint8_t*, COM_PACKET*);
void GetPacket(uint8_t*, PACKET*);
void GetDataSubPacket(uint8_t*, DATA_SUB_PACKET*);

bool CallTokenHandler(uint8_t *);

#endif //_OPAL_SIM_PARSER_
