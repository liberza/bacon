#ifndef BMP_H
#define BMP_H

#include <stdint.h>

struct msg_types_t
{
    uint8_t SIM_ALT_REQUEST;
    uint8_t SIM_ALT;
    uint8_t PAYLOAD_ALT_REQUEST;
    uint8_t PAYLOAD_ALT;
    uint8_t WAT_REQUEST;
    uint8_t WAT_REPLY;
    uint8_t PEER_ADDR;
    uint8_t INITIAL_ALT_REQUEST;
    uint8_t INITIAL_ALT_REPLY;
    uint8_t UNKNOWN;
};

const struct msg_types_t MSG_TYPES;

uint8_t get_msg_type(uint8_t *frame, uint16_t frame_len);

uint8_t get_wat_type(uint8_t *frame, uint16_t frame_len);

void send_wat_reply(uint64_t dest);

void send_peer_addr(uint64_t addr, uint64_t dest);

void send_sim_alt_request(uint64_t dest, uint16_t time);

void send_payload_alt_request(uint64_t dest, uint32_t alt);

void send_initial_alt_reply(uint64_t dest, uint32_t alt);

void send_initial_alt_request(uint64_t dest);

int32_t parse_alt(uint8_t *frame, uint16_t frame_len);

void send_alt(uint64_t dest, int32_t alt);

#endif
