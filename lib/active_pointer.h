/**
 * @file ap_example_1.h
 * "Active pointer" is a simple library written in the process of exploring the LVGL.
 *  It helps to emulate LVGL's input device with programmable actions in a simple way.
 *  This may be useful (or not) for creating demos or for another tests purposes.
 *
 * @autor tgruzd
*/

#ifndef ACTIVE_POINTER_H
#define ACTIVE_POINTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"  
 
#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
/*********************
 *      DEFINES
 *********************/

#define  AP_MAX 128	/*max number of points, where pointer will runs to*/
#define  AP_MIN_TIME 100 /* minimum time of movings and delays > LV_INDEV_DEF_READ_PERIOD*/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	AP_MOVE_TO,
	AP_CLICK_TO,
	AP_DRAG_TO
} ap_action_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void ap_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
/*initializes as input deviceon driver level*/
int ap_init(const lv_img_dsc_t* ptr_image, lv_obj_t* scr);
/*adds new action with coordinates */
int  ap_add_xy(ap_action_t action, lv_coord_t x, lv_coord_t y, uint32_t delay_before, uint32_t move_time);
/*adds new action with object center as destination coordinates*/
int  ap_add_obj(ap_action_t action, lv_obj_t* obj, lv_coord_t dx, lv_coord_t dy, uint32_t delay_before, uint32_t move_time);
/*starts pointer actions */
void ap_start(void);
/*changes move times and/or delay times*/
void ap_modify_speed ( uint16_t multiplier ,uint16_t divider  , bool affects_delay, bool affects_move_time);
/*removes all points*/	
void ap_delete_points(void);
/*removes all points and brings all settings to initial state*/	
void ap_reset(void);
/*sets a callback to the end of the sequence*/	
void ap_set_end_cb (void (*cb)(void));
/*sets circular mode. after last action, pointer will do the first*/	
void ap_set_circular_mode( uint8_t mode);
/*sets pointer colors in pressed and released states*/
void ap_set_recolor (lv_color_t recolor_rel, lv_color_t recolor_pr);
/* makes all defined coordinates referenced to new origin. Not recommend as affects to object centers reference too*/
void ap_set_origin(lv_coord_t x, lv_coord_t y); 
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*ACTIVE_POINTER_H*/
