/**
* @file active_pointer.c
*
* @author tgruzd
 */

/*********************
 *      INCLUDES
 *********************/
#include "active_pointer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
	size_t idx;
	lv_point_t point;
	lv_obj_t * ref_obj;
	uint32_t move_time;
	uint32_t delay;
	ap_action_t action;
} ap_target_t;

typedef struct {
	ap_target_t* target[AP_MAX];
	lv_point_t ref;
	void (* end_sequence_cb)(void);
	size_t cnt;
	lv_anim_t anim_x;
	lv_anim_t anim_y;
	uint8_t circ_mode;
} ap_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ap_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void ap_point_run( ap_target_t* target, ap_target_t* prev  );
static int  ap_add_action(ap_action_t action, lv_obj_t* ref_obj, lv_coord_t dx, lv_coord_t dy, uint32_t delay_before, uint32_t move_time);

/**********************
 *  STATIC VARIABLES
 **********************/
static ap_target_t  init_target = {
	.idx = 0,
	.point = {0, 0},
	.ref_obj  = NULL,
	.move_time  = AP_MIN_TIME,
	.delay = AP_MIN_TIME,
	.action  = AP_MOVE_TO
};

static	ap_t ap;
static ap_target_t* prev_target;
static ap_action_t last_action = AP_MOVE_TO;
static lv_obj_t* cursor_obj;
static lv_indev_data_t  cur_data;
static lv_timer_t* anim_timer;
static uint8_t	first_run = 1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void  ap_set_origin(lv_coord_t x, lv_coord_t y) {   //
	ap.ref.x = x;
	ap.ref.y = y;
}
void ap_start(void) {
	if( ap.cnt > 0 ) {
		if(first_run) {
			ap_point_run(ap.target[0], &init_target);
		}
		else {
			ap_point_run(ap.target[0], ap.target[ap.cnt - 1]);
		}
	}
	else {
		/* error */
	}
}



int  ap_add_xy(ap_action_t action, lv_coord_t x, lv_coord_t y, uint32_t delay_before, uint32_t move_time) {
	return	ap_add_action (action, NULL, x, y, delay_before, move_time);
}

int  ap_add_obj(ap_action_t action, lv_obj_t* obj, lv_coord_t dx, lv_coord_t dy, uint32_t delay_before, uint32_t move_time) {
	return	ap_add_action (action, obj, dx, dy, delay_before, move_time);
}

void ap_delete_points(void) {
	for (int i = 0; i < ap.cnt; i++) {
		if( ap.target[i]) {
			lv_mem_free  (ap.target[i]);
			ap.target[i] = NULL;
		}
	}

	ap.cnt = 0;
}

void ap_reset(void) {
	ap_delete_points();
	first_run = 1;
	ap.ref.x = 0;
	ap.ref.y = 0;
	ap.end_sequence_cb = NULL;
	lv_anim_init(&ap.anim_y) ;
	lv_anim_init(&ap.anim_x );
}

void ap_set_end_cb (  void (*cb)(void)) {
	ap.end_sequence_cb = cb;
}

void ap_modify_speed ( uint16_t mult, uint16_t div, bool affects_delay, bool affects_move_time) {
	for (int i = 0; i < ap.cnt; i++) {
		if (affects_delay) {
			ap.target[i]->delay = (ap.target[i]->delay * mult) / div  ;
		}
		if (affects_move_time) {
			ap.target[i]->move_time =  (ap.target[i]->move_time * mult) / div ;
		}
	}
}

int ap_init(const lv_img_dsc_t* ptr_image) {
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = ap_read;
	static lv_indev_t* indev ;
	indev = lv_indev_drv_register(&indev_drv);
	cursor_obj = lv_img_create(lv_disp_get_layer_sys(indev->driver->disp));
	lv_img_set_src(cursor_obj, ptr_image);
	static lv_style_t empty;
	lv_style_init(&empty);
	lv_obj_add_style(cursor_obj, &empty, LV_STATE_DEFAULT);
	lv_obj_set_style_img_recolor_opa(cursor_obj, LV_OPA_40, LV_STATE_DEFAULT);
	lv_indev_set_cursor(indev, cursor_obj);
	ap.ref.x = 0;
	ap.ref.y = 0;
	anim_timer = lv_anim_get_timer();
	first_run = 1;
	*prev_target = init_target;
	return LV_RES_OK;
}


void ap_set_recolor (	lv_color_t recolor_rel, lv_color_t recolor_pr ) {
	lv_obj_set_style_img_recolor(cursor_obj, recolor_rel, LV_STATE_DEFAULT);
	lv_obj_set_style_img_recolor(cursor_obj, recolor_pr, LV_STATE_PRESSED);
}

void ap_set_circular_mode( uint8_t mode) {
	ap.circ_mode = mode;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void ap_point_run( ap_target_t* target, ap_target_t* prev  );

static void press(void) {
	cur_data.state  = LV_INDEV_STATE_PR;
	lv_obj_add_state(cursor_obj, LV_STATE_PRESSED);
	lv_timer_pause(anim_timer);
}

static void release(void) {
	cur_data.state  = LV_INDEV_STATE_REL;
	lv_obj_clear_state(cursor_obj, LV_STATE_PRESSED);
	lv_timer_pause(anim_timer);
}



static void ap_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
	static lv_indev_state_t last = LV_INDEV_STATE_REL;
	data->point.x =	cur_data.point.x + ap.ref.x;
	data->point.y =	cur_data.point.y + ap.ref.y;
	data->state = cur_data.state;

	if (last == LV_INDEV_STATE_PR  ) {
		if( last_action == AP_MOVE_TO || last_action == AP_CLICK_TO) {
			release();
		}
		else {
			data->state = LV_INDEV_STATE_PR;
		}
	}

	last = cur_data.state;
	lv_timer_resume(anim_timer);
}

static void set_pos_x(ap_target_t* target,  int32_t v) {
	cur_data.point.x = v  + ap.ref.x;
	return;
}
static void set_pos_y( ap_target_t* target,  int32_t v) {
	cur_data.point.y = v  + ap.ref.y;
	return;
}

static int  ap_add_action(ap_action_t action, lv_obj_t* ref_obj, lv_coord_t dx, lv_coord_t dy, uint32_t delay_before, uint32_t move_time) {
	int result = 0;

	if (ap.cnt < AP_MAX) {
		ap_target_t *  ap_point = lv_mem_alloc(sizeof ( ap_target_t));
		ap_point->idx = ap.cnt;
		ap_point->point.x = dx;
		ap_point->point.y = dy;
		ap_point->ref_obj = ref_obj;
		ap_point->delay = 	(delay_before < AP_MIN_TIME) ? AP_MIN_TIME : delay_before;
		ap_point->move_time = (move_time < AP_MIN_TIME) ? AP_MIN_TIME : move_time;
		ap_point->action = action;
		ap.target[ap.cnt++] = ap_point;
		result = 1;
	}

	return result;
}

static void before_cb(lv_anim_t *a ) {  /* now pointer located in previous position*/
	if( a->var == NULL ) {
		return;
	}

	/* what do we do here? */
	return;
}

lv_point_t   get_ref_obj_center( ap_target_t* target) {
	lv_point_t res = {0, 0};

	if(target != NULL && target->ref_obj != NULL ) {
		lv_area_t a;
		lv_obj_get_coords(target->ref_obj, &a);
		res.x = a.x1 + (a.x2 - a.x1) / 2   ;
		res.y = a.y1 + (a.y2 - a.y1) / 2   ;
	}

	return res;
}

static void start_cb(lv_anim_t *a ) { /* now pointer located in previous position: good place to start dragging*/
	if( a->var != NULL ) {
		ap_target_t* target =  (ap_target_t* ) (a->var);

		if( target->action == AP_DRAG_TO) {
			lv_point_t  c = get_ref_obj_center( prev_target);
			set_pos_y (target, prev_target->point.y + c.y);
			set_pos_x (target, prev_target->point.x + c.x);
			press();
			last_action = target->action ;
		}
	}
}
static void stop_cb(lv_anim_t *a ) {     /* now pointer located in its defined coordinates: here it performs it's action, then load the next point definitions*/
	ap_target_t* target =  (ap_target_t*)(a->var);
	lv_point_t  c = get_ref_obj_center(target);
	set_pos_y (target, target->point.y + c.y);
	set_pos_x (target, target->point.x + c.x);

	switch ( target->action  ) {
	case   AP_MOVE_TO: {
			release();
		}
		break;

	case AP_CLICK_TO: {
			press();
		}
		break;

	case  AP_DRAG_TO: {
			release();
		}
		break;
	}

	last_action = target->action ;

	/* next target init */

	if( ((target ->idx + 1 ) < ap.cnt) && (ap.target[target->idx   + 1]   )   ) { /* is next present?*/
		ap_point_run(ap.target[target->idx   + 1], target  );
	}
	else	{
		/*  it is last */
		lv_anim_del(&ap.anim_y, NULL);
		lv_anim_del(&ap.anim_x, NULL);

		if(ap.circ_mode) {
			first_run = 0;
			ap_point_run(ap.target[0], target  );
		}
		else {
			if( ap.end_sequence_cb) {
				ap.end_sequence_cb();
			}
		}
	}
}
static void ap_point_anim_init( ap_target_t* target,  ap_target_t* prev ) {
	lv_point_t obj_ref = get_ref_obj_center(target);
	lv_point_t obj_ref_prev = get_ref_obj_center(prev);
	lv_anim_t ay;
	lv_anim_init(&ay);
	lv_anim_set_var(&ay, target);
	lv_anim_set_time(&ay, target->move_time);
	lv_anim_set_delay(&ay, target->delay);
	lv_anim_set_early_apply(&ay, false);

	if(prev != NULL) {
		lv_anim_set_values(&ay, prev->point.y + obj_ref_prev.y, target->point.y + obj_ref.y  );
	}
	else {
		lv_anim_set_values(&ay,  ap.ref.y + obj_ref_prev.y, target->point.y + obj_ref.y );
	}

	lv_anim_set_path_cb(&ay, lv_anim_path_ease_in_out);
	lv_anim_set_exec_cb(&ay, (lv_anim_exec_xcb_t) set_pos_y);
	ap.anim_y = ay;
	lv_anim_t ax;
	lv_anim_init(&ax);
	lv_anim_set_var(&ax, target);
	lv_anim_set_time(&ax, target->move_time);
	lv_anim_set_delay(&ax, target->delay);
	lv_anim_set_early_apply(&ax, false);

	if(prev != NULL) {
		lv_anim_set_values(&ax, prev->point.x + obj_ref_prev.x, target->point.x + obj_ref.x  );
	}
	else {
		lv_anim_set_values(&ax, ap.ref.x, target->point.x + obj_ref.x );
	}

	lv_anim_set_path_cb(&ax, lv_anim_path_ease_in_out);
	lv_anim_set_ready_cb(&ax, before_cb  );
	lv_anim_set_start_cb(&ax, start_cb );
	lv_anim_set_deleted_cb(&ax, (lv_anim_deleted_cb_t)stop_cb );
	lv_anim_set_exec_cb(&ax,  (lv_anim_exec_xcb_t)set_pos_x);
	ap.anim_x = ax;
	return;
}

static void ap_point_run( ap_target_t* target, ap_target_t* prev  ) {
	prev_target = prev;
	ap_point_anim_init(target, prev);
	lv_anim_start(&ap.anim_y);
	lv_anim_start(&ap.anim_x);
}




