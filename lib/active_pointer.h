/**
 * @file active_pointer.h

 * @brief "Active pointer" is a simple library written in the process of exploring the LVGL.
 *  It helps to emulate LVGL's input device with programmable actions in a simple way.
 *  This may be useful (or not) for creating demos or for another tests purposes.
 *
 * @author tgruzd
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

#define  AP_MAX 128	/**Maximum number of points, where pointer will runs to*/
#define  AP_MIN_TIME 100 /**Minimum time of movings and delays > LV_INDEV_DEF_READ_PERIOD*/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	AP_MOVE_TO,		/**The pointer only moves to destination*/
	AP_CLICK_TO,	/**The pointer moves to destination, and clicks there*/
	AP_DRAG_TO		/**The pointer presses on the home position, remains pressed when moving to the destination, and releases there*/
} ap_action_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

//static void ap_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**@brief initializes the input device on a driver level*/
int ap_init(
	const lv_img_dsc_t* ptr_image/**Defines the appearance of the pointer, if NULL then pointer become invisible */
);

/**@brief Adds new action with coordinates */
int  ap_add_xy(
				ap_action_t action 	/**Action that will be performed on destination point*/,
				lv_coord_t x	/**Destination point coordinate*/,  
				lv_coord_t y	/**Destination point coordinate*/, 
				uint32_t delay_before	/**Time before pointer starts moving (staying on home position)*/,
				uint32_t move_time	/**Time during pointer moves to destination*/ 
);
/** Adds new action with object center as destination coordinates*/
int  ap_add_obj(
ap_action_t action	 /**Action that will be performed on the destination point*/,
lv_obj_t* obj	/**the object whoose center used as the destination @note objects coordinates picked up in the moment of the function call*/,
lv_coord_t dx	/**Value to adjust the destination  */,
lv_coord_t dy	/**Value to adjust the destination  */,
uint32_t delay_before	/**Time before pointer starts moving (staying on home position)*/, 
uint32_t move_time	/**Time during pointer moves to destination*/ 
);

/**@brief Starts pointer actions */
void ap_start(void);
/**@brief Changes move times and/or delay times*/
void ap_modify_speed ( uint16_t multiplier, uint16_t divider, bool affects_delay, bool affects_move_time);
/**@brief removes all points*/
void ap_delete_points(void);
/**@brief Removes all points and brings all settings to initial state*/
void ap_reset(void);
/**@brief Sets the callback to the end of the sequence*/
void ap_set_end_cb (void (*cb)(void));
/**@brief Sets circular mode of execution: after last action, pointer will do the first*/
void ap_set_circular_mode( uint8_t mode);
/**@brief Sets pointer colors in pressed and released states*/
void ap_set_recolor (lv_color_t recolor_rel, lv_color_t recolor_pr);
/**@brief Makes all defined coordinates referenced to new origin. @note Affects to object centers reference too*/
void ap_set_origin(lv_coord_t x, lv_coord_t y);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*ACTIVE_POINTER_H*/
