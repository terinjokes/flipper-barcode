// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#include "protocol_ean13.h"

#define EAN13_DATA_SIZE 12

typedef struct {
    uint8_t data[EAN13_DATA_SIZE];
} ProtocolEAN13;

ProtocolEAN13* protocol_ean13_alloc(void) {
    ProtocolEAN13* protocol = malloc(sizeof(ProtocolEAN13));

    return protocol;
}

void protocol_ean13_free(ProtocolEAN13* protocol) {
    free(protocol);
}

uint8_t* protocol_ean13_get_data(ProtocolEAN13* protocol) {
    return protocol->data;
}

const ProtocolBase protocol_ean13 = {
    .name = "EAN-13",
    .manufacturer = "Generic",
    .data_size = EAN13_DATA_SIZE,
    .alloc = (ProtocolAlloc)protocol_ean13_alloc,
    .free = (ProtocolFree)protocol_ean13_free,
    .get_data = (ProtocolGetData)protocol_ean13_get_data,
};
