/*
 * pebble pedometer+
 * @author jathusant
 */

#include <run.h>
	
Window *window;
Window *menu_window;
Window *set_stepGoal;

ActionBarLayer *stepGoalSetter;

SimpleMenuLayer *pedometer_settings;
SimpleMenuItem menu_items[5];
SimpleMenuSection menu_sections[1];
char *item_names[5] = {"Start", "Step Goal", "Pedometer Theme", "About", "Developed By"};
char *item_sub[5] = {"Lets Go Running!", "Not Set", "Current: Dark", "v1.0-RELEASE", "Jathusan T, (c) 2014"};

TextLayer *main_message;
TextLayer *main_message2;
TextLayer *sub_message;
TextLayer *sub_message2;
TextLayer *stepGoalVisualizer;

//used for themes
bool isDark = true;
int stepGoal = 0;
int pedometerCount = 0;
const int STEP_INCREMENT = 100;

void start_callback(int index, void *ctx){
	
}

void info_callback(int index, void *ctx){
	
}

void inc_click_handler(ClickRecognizerRef recognizer, void *context){
	stepGoal+=STEP_INCREMENT;
	static char buf[]="123456";
	snprintf(buf, sizeof(buf), "%d", stepGoal);
	text_layer_set_text(stepGoalVisualizer, buf);
	if (stepGoal !=0){
		menu_items[1].subtitle = buf;
	}
	layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));
}

void dec_click_handler(ClickRecognizerRef recognizer, void *context){
	if (stepGoal >=STEP_INCREMENT){
		stepGoal-=STEP_INCREMENT;
		static char buf[]="123456";
		snprintf(buf, sizeof(buf), "%d", stepGoal);
		text_layer_set_text(stepGoalVisualizer, buf);
		if (stepGoal !=0){
			menu_items[1].subtitle = buf;
		}
		layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));
	}
}

void goal_set_click_config(void *context){
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) dec_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) inc_click_handler);
}

void stepGoal_load(Window *window){
	stepGoalSetter = action_bar_layer_create();
	
	action_bar_layer_add_to_window(stepGoalSetter, set_stepGoal);
	action_bar_layer_set_click_config_provider(stepGoalSetter, goal_set_click_config);
	
	stepGoalVisualizer = text_layer_create(GRect(0, 0, 150, 150));
	text_layer_set_background_color(stepGoalVisualizer, GColorClear);	
	text_layer_set_font(stepGoalVisualizer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_LT_30)));
    layer_add_child(window_get_root_layer(window), (Layer*) stepGoalVisualizer);

	static char buf[]="123456";
	snprintf(buf, sizeof(buf), "%d", stepGoal);
	text_layer_set_text(stepGoalVisualizer, buf);
	
	if(isDark) {
		window_set_background_color(set_stepGoal, GColorBlack);
		text_layer_set_background_color(stepGoalVisualizer, GColorClear);
		text_layer_set_text_color(stepGoalVisualizer, GColorWhite);
		action_bar_layer_set_background_color(stepGoalSetter, GColorWhite);
	} else {
		window_set_background_color(set_stepGoal, GColorWhite);
		text_layer_set_background_color(stepGoalVisualizer, GColorClear);
		text_layer_set_text_color(stepGoalVisualizer, GColorBlack);
		action_bar_layer_set_background_color(stepGoalSetter, GColorBlack);
	}
}

void stepGoal_unload(Window *window){
	//TODO add this later
}

void stepGoal_callback(int index, void *ctx){
	set_stepGoal = window_create();
	
	window_set_window_handlers(set_stepGoal, (WindowHandlers) {
      .load = stepGoal_load,
      .unload = stepGoal_unload,
    });
		
	window_stack_push(set_stepGoal, true);
		
	static char buf[]="123456";
	snprintf(buf, sizeof(buf), "%d", stepGoal); 
	
	//marking the layer as dirty, to acknowledge layer change
	if (stepGoal !=0){
		menu_items[1].subtitle = buf;
	}
	layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));
}

void theme_callback(int index, void *ctx){
	char *theme = "";
	if(isDark){
		isDark = false;
		theme = "Light";
	} else {
		isDark = true;
		theme = "Dark";
	}
	
	char* new_string;
	new_string = malloc(strlen(theme)+10);
	strcpy(new_string, "Current: ");
	strcat(new_string, theme);
	menu_items[2].subtitle = new_string;
	
	//marking the layer as dirty, to acknowledge layer change
	layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));
}


////////////////////////////////////////////////////////////////////
// Settings Menu
///////////////////////////////////////////////////////////////////

void setup_menu_items(){
	for (int i = 0; i < (int)(sizeof(item_names) / sizeof(item_names[0])); i++){
		menu_items[i] = (SimpleMenuItem){
    		.title = item_names[i],
			.subtitle = item_sub[i],
		};
		
		//Setting Callbacks
		if (i==0){
			menu_items[i].callback = start_callback;
		} else if (i==1){
			menu_items[i].callback = stepGoal_callback;
		} else if (i==2){
			menu_items[i].callback = theme_callback;
		} else {
			menu_items[i].callback = info_callback;
		}
	}
}

void setup_menu_sections(){
	menu_sections[0] =  (SimpleMenuSection){
		.items = menu_items,
		.num_items = sizeof(menu_items) / sizeof(menu_items[0])
	};
}

void setup_menu_window(){
	menu_window = window_create();
	
	window_set_window_handlers(menu_window, (WindowHandlers) {
      .load = settings_load,
      .unload = settings_unload,
    });
}

void settings_load(Window *window){	
	Layer *layer = window_get_root_layer(menu_window);
	
	pedometer_settings = simple_menu_layer_create(layer_get_bounds(layer), menu_window, menu_sections, 1, NULL);
	simple_menu_layer_set_selected_index(pedometer_settings, 0, true);
	layer_add_child(layer,simple_menu_layer_get_layer(pedometer_settings));
}

void settings_unload(Window *window){
    layer_destroy(window_get_root_layer(menu_window));
}

////////////////////////////////////////////////////////////////////
// Splash Menu
///////////////////////////////////////////////////////////////////
 
void select_click_handler(ClickRecognizerRef recognizer, void *context){
	window_stack_pop(true);
	setup_menu_items();
	setup_menu_sections();
	setup_menu_window();
	window_stack_push(menu_window, true);
}

void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

void window_load(Window *window){
    window_set_background_color(window, GColorBlack);
    
    //initializing text layers
    main_message = text_layer_create(GRect(0, 0, 150, 150));
    main_message2 = text_layer_create(GRect(0, 30, 150, 150));
    sub_message = text_layer_create(GRect(0,60,150,150));
    sub_message2 = text_layer_create(GRect(0,110,150,150));
    
    //"Welcome"
    text_layer_set_background_color(main_message, GColorClear);
    text_layer_set_text_color(main_message, GColorWhite);
    text_layer_set_font(main_message, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_LT_30)));
    layer_add_child(window_get_root_layer(window), (Layer*) main_message);
    
    //"to Cadenza"
    text_layer_set_background_color(main_message2, GColorClear);
    text_layer_set_text_color(main_message2, GColorWhite);
    text_layer_set_font(main_message2, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_LT_15)));
    layer_add_child(window_get_root_layer(window), (Layer*) main_message2);
    
    //"select a theme"
    text_layer_set_background_color(sub_message, GColorClear);
    text_layer_set_text_color(sub_message, GColorWhite);
    text_layer_set_font(sub_message, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_LT_30)));
    layer_add_child(window_get_root_layer(window), (Layer*) sub_message);
    
    //"current theme:"
    text_layer_set_background_color(sub_message2, GColorClear);
    text_layer_set_text_color(sub_message2, GColorWhite);
    text_layer_set_font(sub_message2, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_LT_15)));
    layer_add_child(window_get_root_layer(window), (Layer*) sub_message2);
    
    //setting text
    text_layer_set_text(main_message, " Welcome");
    text_layer_set_text(main_message2, "      to Pedometer+");
    text_layer_set_text(sub_message, "    <logo>");
    text_layer_set_text(sub_message2, "         SELECT to\n          continue!");
} 

void window_unload(Window *window){
    text_layer_destroy(main_message);
    text_layer_destroy(main_message);
    text_layer_destroy(main_message2);
    text_layer_destroy(sub_message);
    text_layer_destroy(sub_message2);
}

//Initializer/////////////////////////////////////////////////////////////////

void handle_init(void) {
    window = window_create();
	
    window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
	
    window_set_click_config_provider(window, click_config_provider);
	window_set_fullscreen(window, true);
    window_stack_push(window, true);
}

void handle_deinit(void) {
    window_destroy(window);
}