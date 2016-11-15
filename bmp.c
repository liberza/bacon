#include "bmp.h"
#include "xbee.h"
#include <stdio.h>

const struct msg_types_t MSG_TYPES =
{
    .SIM_ALT        = (uint8_t)'s',
    .PAYLOAD_ALT    = (uint8_t)'a',
    .ALT_REQUEST    = (uint8_t)'R',
    .WAT_REQUEST    = (uint8_t)'W',
    .WAT_REPLY      = (uint8_t)'w',
    .UNKNOWN        = (uint8_t)'\0'
};

uint8_t get_msg_type(uint8_t *frame, uint16_t frame_len)
{
    uint8_t ret = MSG_TYPES.UNKNOWN;

    // Make sure this is an actual RX frame.
    if ((frame_len >= 16) && frame[3] == FRAME_TYPES.RX)
    {
        switch (frame[15])
        {
            case MSG_TYPES.SIM_ALT:
                ret = MSG_TYPES.SIM_ALT;
                break;
            case MSG_TYPES.PAYLOAD_ALT:
                ret = MSG_TYPES.PAYLOAD_ALT;
                break;
            case MSG_TYPES.ALT_REQUEST:
                ret = MSG_TYPES.ALT_REQUEST;
                break;
            case MSG_TYPES.WAT_REQUEST:
                ret = MSG_TYPES.WAT_REQUEST;
                break;
            case MSG_TYPES.WAT_REPLY:
                ret = MSG_TYPES.WAT_REPLY;
                break;
            default:
                break;
        }
    }
    return ret;
}

uint8_t get_wat_type(uint8_t *frame, uint16_t frame_len)
{
    uint8_t ret = MSG_TYPES.UNKNOWN;
    if ((frame_len == FRAME_OHEAD.RX + 2) && (frame[15] == MSG_TYPES.WAT_REPLY))
    {
        if ((frame[16] == (uint8_t)'P') || (frame[16] == (uint8_t)'S'))
        {
            ret = FRAME[16];
        }
    }
    return ret;
}

void send_wat_reply(uint64_t addr)
{
    tx((uint8_t*)"wP", 2, addr, 0x00);
}

void send_alt_request(uint64_t addr)
{
    tx((uint8_t*)"R", 1, addr, 0x00);
}

void get_alt(uint8_t *frame, uint16_t frame_len, int32_t *alt)
{
    uint8_t tmp_checksum;
    if ((frame_len > FRAME_OHEAD.RX + 2) && (frame[15] == MSG_TYPES.SIM_ALT))
    {
        tmp_checksum = frame[frame_len-1];
        frame[frame_len-1] = (uint8_t)'\0';
        // need moar error checking here.
        alt = strtol((frame+16), NULL, 10);
    }
}

void send_alt(uint64_t addr, int32_t alt)
{
    // 1 type byte, 1 sign byte, 10 32-bit signed int bytes, 1 nul terminator.
    uint8_t msg[13];
    sprintf(&msg, "a%d", alt);
    tx(msg, 12, addr, 0x00);
}
