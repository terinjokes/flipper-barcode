// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <toolbox/protocols/protocol.h>

typedef enum {
    BarcodeProtocolEAN13,
    BarcodeProtocolMax,
} BarcodeProtocol;

extern const ProtocolBase* barcode_protocols[];
