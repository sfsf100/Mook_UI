/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <TFT_eSPI.h>

//GPIO PIN FOR BTN
const int buttonPin = 34; // 使用 GPIO 0
unsigned long previousMillis = 0; // 上次切換的時間
const long interval = 3000;       // 設置3秒的間隔

bool screenToggle = false;        // 用於追蹤目前是顯示哪個頁面
/*To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 *You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 Note that the `lv_examples` library is for LVGL v7 and you shouldn't install it for this version (since LVGL v8)
 as the examples and demos are now part of the main LVGL library. */

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight /10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

lv_obj_t *screen1;
lv_obj_t *screen2;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}


void button_event_handler(lv_event_t *e){
    lv_obj_t *btn =lv_event_get_target(e);
    if(lv_obj_has_state(btn,LV_STATE_CHECKED)){
        lv_scr_load(screen2);
    }else{
        lv_scr_load(screen1);
    }
}

void create_screens(){
    screen1 =lv_obj_create(NULL);
    lv_obj_t *label_1 = lv_label_create(screen1);
    lv_label_set_text(label_1,"This is screen 1");
    lv_obj_align(label_1,LV_ALIGN_CENTER,0,0);

    screen2 =lv_obj_create(NULL);
    lv_obj_t *label_2 = lv_label_create(screen2);
    lv_label_set_text(label_2,"This is screen 2");
    lv_obj_align(label_2,LV_ALIGN_CENTER,0,0);
}

void create_button() {
    lv_obj_t *button = lv_btn_create(lv_scr_act());
    lv_obj_set_size(button, 120, 50);
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 50);
    
    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, "Switch Screen");
    
    lv_obj_add_event_cb(button, button_event_handler, LV_EVENT_CLICKED, NULL);
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();
    tft.begin();          /* TFT init */
    tft.setRotation( 0 ); /* Landscape orientation, flipped  3*/

    /*Set the touchscreen calibration data,
     the actual data for your display can be acquired using
     the Generic -> Touch_calibrate example from the TFT_eSPI library*/
    uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
    tft.setTouch( calData );

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;//// Implement this function
    lv_disp_drv_register( &disp_drv );

    //gpio set
    pinMode(buttonPin,INPUT_PULLUP);
    create_screens();
    create_button();

    lv_scr_load(screen1); // Load the first screen

    /*Initialize the (dummy) input device driver*/
   /* static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );*/
  
    /* Create simple label */
    /*lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, "Hello Ardino and LVGL!");
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );*/

}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay( 5 );

    unsigned long currentMillis = millis();

    // 如果距離上次切換已超過設定的間隔時間，則切換頁面
    if (currentMillis - previousMillis >= interval) {
        // 記錄這次切換的時間
        previousMillis = currentMillis;

        // 切換頁面
        screenToggle = !screenToggle;
        if (screenToggle) {
            lv_scr_load(screen2); // 切換到 screen2
            Serial.println("Switched to Screen 2");
        } else {
            lv_scr_load(screen1); // 切換回 screen1
            Serial.println("Switched to Screen 1");
        }
    }

    /*if(digitalRead(buttonPin)==LOW){
        static bool screenToggle =false;//切換page state
        if(screenToggle){
            lv_scr_load(screen2);
        }else{
             lv_scr_load(screen1);
        }
        delay(200);
    }*/
}