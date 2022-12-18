// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#include "barcode_protocols.h"
#include "protocol_ean13.h"

const ProtocolBase* barcode_protocols[] = {[BarcodeProtocolEAN13] = &protocol_ean13};
