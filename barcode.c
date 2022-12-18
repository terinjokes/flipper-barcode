// Copyright 2022 Terin Stock.
// SPDX-License-Identifier: MPL-2.0
#include <furi.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>

#include "barcode_file_dict.h"
#include "barcode_icons.h"

#define BARCODE_STARTING_POS 16
#define BARCODE_HEIGHT 50
#define BARCODE_Y_START 3

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct BarcodeApp {
    Storage* storage;
    DialogsApp* dialogs;
    ProtocolDict* dict;
    ProtocolId protocol_id;
    uint8_t* data;
} BarcodeApp;

static void render_callback(Canvas* const canvas, void* context) {
    BarcodeApp* app = context;

    canvas_set_color(canvas, ColorBlack);

    for (int i = 0; i < 12; i++) {
        for (int j = 8 * sizeof(app->data[i]) - 1; j >= 0; j--) {
            if (app->data[i] & (1 << j)) {
                canvas_draw_box(
                    canvas,
                    BARCODE_STARTING_POS + i * 8 + (7 - j),
                    BARCODE_Y_START,
                    1,
                    BARCODE_HEIGHT + 2);
            }
        }
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

void barcode_make_app_folder(BarcodeApp* app) {
    furi_assert(app);

    if (!storage_simply_mkdir(app->storage, ANY_PATH("barcode"))) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
}

int32_t barcode_app(void* p) {
    UNUSED(p);

    BarcodeApp* app = malloc(sizeof(BarcodeApp));
    app->storage = furi_record_open(RECORD_STORAGE);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->protocol_id = PROTOCOL_NO;
    barcode_make_app_folder(app);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".bar", &I_barcode_10px);
    browser_options.base_path = ANY_PATH("barcode");
    FuriString* file_path = furi_string_alloc_set(ANY_PATH("barcode"));

    dialog_file_browser_show(app->dialogs, file_path, file_path, &browser_options);

    app->dict = protocol_dict_alloc(barcode_protocols, BarcodeProtocolMax);
    if (!barcode_dict_file_load(app->dict, furi_string_get_cstr(file_path), &app->protocol_id)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot load file");
        return 0;
    }
    app->data = malloc(protocol_dict_get_max_data_size(app->dict));

    protocol_dict_get_data(app->dict, app->protocol_id, app->data, 12);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    // set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, app);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for (bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        if (event_status == FuriStatusOk) {
            if (event.type == EventTypeKey) {
                if (event.input.type == InputTypePress) {
                    switch (event.input.key) {
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D("Barcode", "FuriMessageQueue: event timeout");
        }

        view_port_update(view_port);
    }

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    protocol_dict_free(app->dict);
    free(app->data);
    free(app);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    return 0;
}
