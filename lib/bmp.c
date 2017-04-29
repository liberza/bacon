#include "bmp.h"
#include "xbee.h"
#include <stdio.h>

const struct msg_types_t MSG_TYPES =
{
    .SIM_ALT_REQUEST        = (uint8_t)'S',
    .SIM_ALT                = (uint8_t)'s',
    .PAYLOAD_ALT_REQUEST    = (uint8_t)'A',
    .PAYLOAD_ALT            = (uint8_t)'a',
    .WAT_REQUEST            = (uint8_t)'W',
    .WAT_REPLY              = (uint8_t)'w',
    .PEER_ADDR              = (uint8_t)'p',
    .INITIAL_ALT_REQUEST    = (uint8_t)'i',
    .INITIAL_ALT_REPLY      = (uint8_t)'I',
    .UNKNOWN                = (uint8_t)'\0'
};

uint8_t get_msg_type(uint8_t *frame, uint16_t frame_len)
{
    uint8_t ret = MSG_TYPES.UNKNOWN;
    uint8_t t;
    uint8_t frame_type = get_frame_type(frame);

    // Make sure this is an actual RX frame.
    if ((frame_len >= 16) && frame_type == FRAME_TYPES.RX)
    {
        t = frame[15];
        if (t == MSG_TYPES.SIM_ALT)
            ret = MSG_TYPES.SIM_ALT;
        else if (t == MSG_TYPES.PAYLOAD_ALT)
            ret = MSG_TYPES.PAYLOAD_ALT;
        else if (t == MSG_TYPES.SIM_ALT_REQUEST)
            ret = MSG_TYPES.SIM_ALT_REQUEST;
        else if (t == MSG_TYPES.PAYLOAD_ALT_REQUEST)
            ret = MSG_TYPES.PAYLOAD_ALT_REQUEST;
        else if (t == MSG_TYPES.WAT_REQUEST)
            ret = MSG_TYPES.WAT_REQUEST;
        else if (t == MSG_TYPES.WAT_REPLY)
            ret = MSG_TYPES.WAT_REPLY;
        else if (t == MSG_TYPES.INITIAL_ALT_REQUEST)
            ret = MSG_TYPES.INITIAL_ALT_REQUEST;
        else if (t == MSG_TYPES.INITIAL_ALT_REPLY)
            ret = MSG_TYPES.INITIAL_ALT_REPLY;
    }
    return ret;
}

uint8_t get_wat_type(uint8_t *frame, uint16_t frame_len)
{
    uint8_t ret = MSG_TYPES.UNKNOWN;
    if ((frame_len >= FRAME_OHEAD.RX + 2) && (frame[15] == MSG_TYPES.WAT_REPLY))
    {
        if ((frame[16] == (uint8_t)'P') || (frame[16] == (uint8_t)'S'))
        {
            ret = (uint8_t)frame[16];
        }
    }
    return ret;
}

void send_wat_reply(uint64_t dest)
{
    tx((uint8_t*)"wP", 2, dest, 0x00);
}

void send_peer_addr(uint64_t addr, uint64_t dest)
{
    uint8_t msg[9];
    msg[0] = 'p';
    for (uint8_t i = 0; i < 8; i++)
    {
        msg[i+1] = (uint8_t)((addr >> (7-i)*8) & 0xFF);
    }
    tx(msg, 9, dest, 0x00);
}

void send_sim_alt_request(uint64_t dest, uint16_t time)
{
    // time is the time the valve was open for between now and the last request.
    uint8_t msg[7]; // one 'R', time up to 65535ms, and nul terminator.
    // left justify, make sure it takes up 5 chars
    sprintf((char*)msg, "S%-5u", (unsigned int)time);
    // transmit, don't care about nul terminator
    tx(msg, 6, dest, 0x00);
}


void send_payload_alt_request(uint64_t dest, uint32_t alt)
{
    // 1 type byte, 1 sign byte, 10 32-bit signed int bytes, 1 nul terminator.
    uint8_t msg[13]; 
    // left justify, make sure it takes up 5 chars
    sprintf((char*)msg, "A%-11ld", (signed long)alt);
    // transmit, don't care about nul terminator
    tx(msg, 12, dest, 0x00);
}

void send_initial_alt_request(uint64_t dest)
{
    tx((uint8_t*)"i", 1, dest, 0x00);
}

void send_initial_alt_reply(uint64_t dest, uint32_t alt)
{
    // 1 type byte, 1 sign byte, 10 32-bit signed int bytes, 1 nul terminator.
    uint8_t msg[13]; 
    // left justify, make sure it takes up 5 chars
    sprintf((char*)msg, "I%-11ld", (signed long)alt);
    // transmit, don't care about nul terminator
    tx(msg, 12, dest, 0x00);
}

int32_t parse_alt(uint8_t *frame, uint16_t frame_len)
{
    uint8_t tmp_checksum;
    int32_t alt = INT32_MIN;
    if ((frame_len > FRAME_OHEAD.RX + 2) && 
        ((frame[15]==MSG_TYPES.SIM_ALT) || (frame[15]==MSG_TYPES.PAYLOAD_ALT) || (frame[15]==MSG_TYPES.INITIAL_ALT_REPLY)))
    {
        tmp_checksum = frame[frame_len-1];
        frame[frame_len-1] = (uint8_t)'\0';
        // need moar error checking here.
        alt = (int32_t)strtol((char*)(frame+16), NULL, 10);
        frame[frame_len-1] = tmp_checksum;
    }
    return alt;
}

void send_alt(uint64_t dest, int32_t alt)
{
    // 1 type byte, 1 sign byte, 10 32-bit signed int bytes, 1 nul terminator.
    uint8_t msg[13];
    // left justify, make sure it takes up 11 chars
    sprintf((char*)msg, "a%-11ld", (signed long)alt);
    // transmit, don't care about nul terminator
    tx(msg, 12, dest, 0x00);
}
