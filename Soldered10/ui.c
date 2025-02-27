// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////


// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Panel14;
lv_obj_t * ui_Chart1;
lv_obj_t * ui_Calendar1;
lv_obj_t * ui_Panel1;
lv_obj_t * ui_timeLBL;
lv_obj_t * ui_Panel3;
lv_obj_t * ui_temp;
lv_obj_t * ui_Label5;
lv_obj_t * ui_Panel7;
lv_obj_t * ui_Panel8;
lv_obj_t * ui_Label8;
lv_obj_t * ui_Panel2;
lv_obj_t * ui_Panel5;
lv_obj_t * ui_Panel6;
lv_obj_t * ui_Image1;
lv_obj_t * ui_Image2;
lv_obj_t * ui_Panel4;
lv_obj_t * ui_hum;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Panel10;
lv_obj_t * ui_Label6;
lv_obj_t * ui_Panel11;
lv_obj_t * ui_Panel9;
lv_obj_t * ui_wind;
lv_obj_t * ui_Label9;
lv_obj_t * ui_Panel12;
lv_obj_t * ui_Label10;
lv_obj_t * ui_Panel13;
lv_obj_t * ui_Panel15;
lv_obj_t * ui_Label11;
lv_obj_t * ui_Label12;
lv_obj_t * ui_Label13;
lv_obj_t * ui_max;
lv_obj_t * ui_Label15;
lv_obj_t * ui_Label16;
lv_obj_t * ui_weather;
lv_obj_t * ui_first;
lv_obj_t * ui_second;
lv_obj_t * ui_third;
lv_obj_t * ui_Label19;
lv_obj_t * ui_Label20;
lv_obj_t * ui_Label21;
lv_obj_t * ui_wifi;
lv_obj_t * ui_Panel19;
lv_obj_t * ui_Panel20;
lv_obj_t * ui_forth;
lv_obj_t * ui_fift;
lv_obj_t * ui_min;
lv_obj_t * ui_Panel23;
lv_obj_t * ui_Panel24;
lv_obj_t * ui____initial_actions0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Screen1);
}
