#ifndef BMP_H
#define BMP_H

#include <stdint.h>

struct msg_types_t
{
    uint8_t SIM_ALT;
    uint8_t PAYLOAD_ALT;
    uint8_t ALT_REQUEST;
    uint8_t WAT_REQUEST;
    uint8_t WAT_REPLY;
    uint8_t UNKNOWN;
}

const struct msg_types_t MSG_TYPES;

uint8_t get_msg_type(uint8_t *frame, uint16_t frame_len);

uint8_t get_wat_type(uint8_t *frame, uint16_t frame_len);

void send_wat_reply(uint64_t addr);

void send_alt_request(uint64_t addr);

uint8_t get_alt(uint8_t *frame, uint16_t frame_len, int32_t *alt);

void send_alt(uint64_t addr);

#endif
