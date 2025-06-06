// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.11
// Project name: Funasr_1.0

#include "ui.h"


// COMPONENT startMeeting

lv_obj_t * ui_startMeeting_create(lv_obj_t * comp_parent)
{

    lv_obj_t * cui_startMeeting;
    cui_startMeeting = lv_btn_create(comp_parent);
    lv_obj_set_width(cui_startMeeting, 100);
    lv_obj_set_height(cui_startMeeting, 50);
    lv_obj_set_x(cui_startMeeting, 228);
    lv_obj_set_y(cui_startMeeting, -142);
    lv_obj_set_align(cui_startMeeting, LV_ALIGN_CENTER);
    lv_obj_add_flag(cui_startMeeting, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(cui_startMeeting, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_STARTMEETING_NUM);
    children[UI_COMP_STARTMEETING_STARTMEETING] = cui_startMeeting;
    lv_obj_add_event_cb(cui_startMeeting, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_startMeeting, del_component_child_event_cb, LV_EVENT_DELETE, children);
    ui_comp_startMeeting_create_hook(cui_startMeeting);
    return cui_startMeeting;
}

