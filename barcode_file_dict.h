// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <toolbox/protocols/protocol_dict.h>
#include "protocols/barcode_protocols.h"

bool barcode_dict_file_load(ProtocolDict* dict, const char* filename, ProtocolId* out);
