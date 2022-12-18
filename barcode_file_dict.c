// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#include "barcode_file_dict.h"
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#define BARCODE_DICT_FILETYPE "Flipper Barcode"

bool barcode_dict_file_load(ProtocolDict* dict, const char* filename, ProtocolId* out) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    uint8_t* data = malloc(protocol_dict_get_max_data_size(dict));
    FuriString* str_result;
    str_result = furi_string_alloc();

    if (!flipper_format_file_open_existing(file, filename)) {
        return false;
    }

    uint32_t version;
    if (!flipper_format_read_header(file, str_result, &version)) {
        return false;
    }
    if (furi_string_cmp_str(str_result, BARCODE_DICT_FILETYPE) != 0) {
        return false;
    }
    if (version != 1) {
        return false;
    }

    if (!flipper_format_read_string(file, "Barcode Type", str_result)) {
        return false;
    }

    *out = protocol_dict_get_protocol_by_name(dict, furi_string_get_cstr(str_result));

    if (*out == PROTOCOL_NO) {
        return false;
    }

    size_t data_size = protocol_dict_get_data_size(dict, *out);
    if (!flipper_format_read_hex(file, "Data", data, data_size)) {
        return false;
    }

    protocol_dict_set_data(dict, *out, data, data_size);

    free(data);
    furi_string_free(str_result);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}
