
#include "ap_example_1.h"
#include "math.h"
#include "active_pointer.h"


lv_obj_t* scrn_example_1_scrn;
lv_obj_t* slider[10];
lv_obj_t* drag_obj[10];


static void ap_test(void );

__weak void  ui_init (void) {
	scrn_example_1_init();
	scrn_example_1_show ();
};


static void scrn_example_1_show(void) {
	lv_scr_load( scrn_example_1_scrn );
}


static void slider_rel_cb ( lv_event_t * e) {
	lv_obj_t * obj = lv_event_get_user_data  (e);
	lv_obj_clear_state( obj,  LV_STATE_USER_1);
}



static void slider_press_cb ( lv_event_t * e) {
	lv_obj_t * obj = lv_event_get_user_data  (e);
	lv_obj_add_state( obj,  LV_STATE_USER_1);
	lv_obj_move_foreground( obj );
}


static void drag_cb(lv_event_t * e) {
	lv_obj_t * obj = lv_event_get_target(e);
	lv_indev_t * indev = lv_indev_get_act();

	if(indev == NULL)  return;

	lv_point_t vect;
	lv_indev_get_vect(indev, &vect);
	lv_coord_t x = lv_obj_get_x(obj) + vect.x;
	lv_coord_t y = lv_obj_get_y(obj) + vect.y;
	lv_obj_set_pos(obj, x, y);
}

static void slider_change_cb ( lv_event_t * e) {
	lv_obj_t * sld = lv_event_get_target  (e);
	lv_obj_t * obj = lv_event_get_user_data  (e);
	lv_obj_move_foreground(obj);
	lv_obj_set_style_bg_opa( obj, lv_slider_get_value(sld) * 255 / 100, LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa( obj, lv_slider_get_value(sld) * 255 / 100, LV_STATE_USER_1);
}

static void run_ap_cb(lv_event_t* e ) {  /* objects must be redrawn at least once to make sure their coordinates are updated. (affects: ap_add_obj()) */
	lv_obj_t*  obj = lv_event_get_target(e);
	lv_obj_remove_event_cb(obj, run_ap_cb);
	ap_test();
}

static void scrn_example_1_init(void) {
	static lv_style_t style_empty;
	lv_style_init(&style_empty);
	scrn_example_1_scrn = lv_obj_create(NULL);
	lv_obj_add_style(scrn_example_1_scrn, &style_empty, LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(scrn_example_1_scrn, lv_color_make(0, 0, 0), LV_STATE_DEFAULT);
	lv_obj_clear_flag(scrn_example_1_scrn, LV_OBJ_FLAG_SCROLLABLE );
	lv_obj_set_width(scrn_example_1_scrn, 800);
	lv_obj_set_height(scrn_example_1_scrn, 480);
	lv_obj_add_event_cb(scrn_example_1_scrn, run_ap_cb, LV_EVENT_DRAW_POST, 0);  /*objects must be redrawn at least once to make sure their coordinates are updated. (affects: ap_add_obj()) */

	for (int i = 0 ; i < 10; i++) {
		/*slider*/
		slider[i] = lv_slider_create(scrn_example_1_scrn);
		lv_obj_add_style(slider[i], &style_empty, LV_STATE_DEFAULT);
		lv_obj_set_style_radius(slider[i], LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
		/*main*/
		lv_obj_set_style_bg_color(slider[i], lv_color_make(0, 50, 100), LV_PART_MAIN );
		lv_obj_set_style_bg_grad_color(slider[i], lv_color_make(0, 44, 50), LV_PART_MAIN );
		lv_obj_set_style_border_width (slider[i], 1, LV_PART_MAIN);
		lv_obj_set_style_border_color (slider[i], lv_color_make(0, 100, 100), LV_PART_MAIN );
		/*indicator*/
		lv_obj_add_style(slider[i], &style_empty, LV_PART_INDICATOR);
		lv_obj_set_style_radius(slider[i], LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
		lv_obj_set_style_bg_color(slider[i], lv_color_make(0, 70, 100), LV_PART_INDICATOR );
		lv_obj_set_style_bg_grad_color (slider[i], lv_color_make(0, 40, 50), LV_PART_INDICATOR );
		lv_obj_set_style_bg_grad_dir (slider[i], LV_GRAD_DIR_HOR, LV_PART_INDICATOR );
		lv_obj_set_style_border_width (slider[i], 2, LV_PART_INDICATOR);
		lv_obj_set_style_border_color (slider[i], lv_color_make(0, 150, 100), LV_PART_INDICATOR );
		lv_obj_set_style_pad_hor(slider[i], 1, LV_PART_INDICATOR);
		lv_obj_set_style_pad_ver(slider[i], 1, LV_PART_INDICATOR);
		/*knob*/
		lv_obj_add_style(slider[i], &style_empty, LV_PART_KNOB);
		lv_obj_set_style_radius(slider[i], LV_RADIUS_CIRCLE, LV_PART_KNOB);
		lv_obj_set_style_bg_color(slider[i], lv_color_make(0, 100, 200), LV_PART_KNOB );
		lv_obj_set_style_bg_grad_color (slider[i], lv_color_make(0, 50, 50), LV_PART_KNOB );
		lv_obj_set_style_bg_grad_dir(slider[i], LV_GRAD_DIR_HOR, LV_PART_KNOB );
		//
		lv_obj_set_style_border_width (slider[i], 1, LV_PART_KNOB );
		lv_obj_set_style_border_color (slider[i], lv_color_make(0, 200, 150), LV_PART_KNOB );
		//
		lv_obj_set_style_pad_hor(slider[i], 3, LV_PART_KNOB);
		lv_obj_set_style_pad_ver(slider[i], 6, LV_PART_KNOB);
		lv_obj_set_size(slider[i], 20, 200);
		lv_obj_set_pos(slider[i], 40 + i * 35, 20);
		lv_slider_set_range(slider[i], 0, 100);
		lv_slider_set_value(slider[i], 0, true);
		/* draggable objects*/
		drag_obj[i] = lv_obj_create(scrn_example_1_scrn);
		lv_obj_remove_style(drag_obj[i], NULL, LV_STATE_ANY | LV_PART_ANY);
		lv_obj_set_width(drag_obj[i], 50);
		lv_obj_set_height(drag_obj[i], 50);
		lv_obj_add_style (drag_obj[i], &style_empty, LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color( drag_obj[i], lv_color_make(255, 0, 255), LV_STATE_DEFAULT );
		lv_obj_set_style_bg_grad_color(drag_obj[i], lv_color_make(100, 0, 100 ), LV_STATE_DEFAULT );
		lv_obj_set_style_bg_grad_dir( drag_obj[i], LV_GRAD_DIR_HOR, LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa( drag_obj[i], 0, LV_STATE_DEFAULT);
		lv_obj_set_style_border_width (drag_obj[i], 1, LV_STATE_DEFAULT);
		lv_obj_set_style_border_color (drag_obj[i], lv_color_make(50, 255, 150), LV_STATE_DEFAULT );
		lv_obj_set_style_radius (drag_obj[i], LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
		lv_obj_add_style (drag_obj[i], &style_empty, LV_STATE_USER_1);
		lv_obj_set_style_bg_color( drag_obj[i], lv_color_make(255, 0, 255), LV_STATE_USER_1 );
		lv_obj_set_style_bg_grad_color(drag_obj[i], lv_color_make(100, 0, 100 ), LV_STATE_USER_1 );
		lv_obj_set_style_bg_grad_dir( drag_obj[i], LV_GRAD_DIR_HOR, LV_STATE_USER_1);
		lv_obj_set_style_bg_opa( drag_obj[i], LV_OPA_COVER, LV_STATE_USER_1);
		lv_obj_set_style_border_width (drag_obj[i], 3, LV_STATE_USER_1);
		lv_obj_set_style_border_color (drag_obj[i], lv_color_make(0, 255, 0), LV_STATE_USER_1 );
		lv_obj_set_style_border_opa (drag_obj[i], LV_OPA_COVER, LV_STATE_USER_1);
		lv_obj_set_style_radius (drag_obj[i], LV_RADIUS_CIRCLE, LV_STATE_USER_1);
		lv_obj_add_flag(drag_obj[i], LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_event_cb(drag_obj[i], drag_cb, LV_EVENT_PRESSING, 0);
		lv_obj_align_to(drag_obj[i], slider[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
		//
		lv_obj_add_event_cb(slider[i], slider_change_cb, LV_EVENT_VALUE_CHANGED, drag_obj[i] );
		lv_obj_add_event_cb(slider[i], slider_press_cb, LV_EVENT_PRESSED, drag_obj[i] );
		lv_obj_add_event_cb(slider[i], slider_rel_cb, LV_EVENT_RELEASED, drag_obj[i] );
	}
}

void gen_random_sequence() {
	
	ap_delete_points();
	ap_set_circular_mode(0);
	srand(lv_tick_get());

	for(int i = 0; i < 10; i++ ) {
		lv_coord_t h;
		h = lv_obj_get_height(slider[i]);
		ap_add_obj(AP_MOVE_TO, slider[i], 0, 0, 500, 500);
		ap_add_obj(AP_DRAG_TO, slider[i], 0, rand() % h - h/2, 500, 1000);
		ap_add_obj(AP_DRAG_TO, slider[i], 0, rand() % h - h/2, 500, 1000);
		ap_add_obj(AP_MOVE_TO, drag_obj[i], 0, 0, 500, 500);
		ap_add_xy(AP_DRAG_TO, 40 + rand() % 300, 260 + rand() %  100, 500, 1000);
	}

	ap_modify_speed(7, 8, 1, 1);
	ap_start();
}

LV_IMG_DECLARE(pntr_image);

static void ap_test(void ) {
	ap_init(&pntr_image, lv_scr_act());
	ap_set_recolor (lv_color_make(0, 255, 0), lv_color_make(255, 0, 0) );
	ap_set_end_cb(gen_random_sequence);
	gen_random_sequence();
}

