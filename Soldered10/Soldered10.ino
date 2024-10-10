#include "Inkplate.h"
#include "ui.h"
#include "lvgl.h"
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager  version 2.0.17
#include <ArduinoJson.h> // 7.1.0
#include <HTTPClient.h> 
// lv_obj_set_style_text_font(ui_Calendar1, &ui_font_small, LV_PART_MAIN | LV_STATE_DEFAULT);



//needed for openWeather map data
String town = "Zagreb";  //place your town here
String myAPI = "xxxxxxxx";  //plac your api key here
String units = "metric";  //  metric, imperial
String server = "https://api.openweathermap.org/data/2.5/weather?q=" + town + "&appid=" + myAPI + "&units=" + units;

// wifi
String ssid = "IGK20";
String password = "18072019";

//weather data

float temperature=0;
float humidity=0;
float wind=0;
String Wmsg="";
String nameS, countryS;

          String mm="";
          String hh="";
          String timeNow="";
          int minutes=0;
          int hour=0;
          int year,month,day;

          int currentMinute=-3;
          int currentHour=-2;;

// neede for graph
float minT=0;
float maxT=0;
float PPtemp[24]={0};
float PPtempT[24]={0};
int PPgraph[24]={0};

// Initialize the display object
Inkplate display(INKPLATE_3BIT);

// Define LVGL-related variables and structures
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
lv_chart_series_t *new_series;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    const uint32_t full_w = 1200;
    const uint32_t full_h = 820;
    uint32_t buffer_size = (full_w * full_h) / 2;  // 2 pixels per byte (4-bit per pixel)
    Serial.println("ajd2");
    // Allocate a buffer to store packed 4-bit grayscale values
    uint8_t* packed_buffer = new uint8_t[buffer_size];

    // Iterate through the pixel buffer and pack 2 pixels per byte
    for (uint32_t y = 0; y < full_h; y++) {
        for (uint32_t x = 0; x < full_w; x += 2) {
            // Calculate the index in the `color_p` buffer for the current pixel
            uint32_t color_index = y * full_w + x;

            // Extract two pixels
            lv_color_t pixel1 = color_p[color_index];
            lv_color_t pixel2 = color_p[color_index + 1];

            // Convert pixels to 4-bit grayscale values
            uint8_t grayscale1 = get_grayscale_4bit(pixel1);
            uint8_t grayscale2 = get_grayscale_4bit(pixel2);

            // Pack the two 4-bit grayscale values into a single byte
            uint8_t packed_pixel = (grayscale1 << 4) | grayscale2;

            // Store the packed byte in the buffer
            uint32_t packed_index = (y * (full_w / 2)) + (x / 2);
            packed_buffer[packed_index] = packed_pixel;
        }
    }

    // Send the packed grayscale buffer to the display
    display.clearDisplay();  // Clear the entire display before the update
    display.drawImage(packed_buffer, 0, 0, full_w, full_h);  // Full-screen update
    // Update the display with the new content
    display.display();
    // Free the temporary packed buffer
    delete[] packed_buffer;

    // Inform LittlevGL that flushing is complete
    lv_disp_flush_ready(disp);
}

// Function to convert lv_color_t to 4-bit grayscale value (0-15)
uint8_t get_grayscale_4bit(lv_color_t pixel) {
    uint8_t r, g, b;

    #if LV_COLOR_DEPTH == 16  // Assuming 16-bit RGB565 format
        r = (pixel.full >> 11) & 0x1F;  // Red component (5 bits)
        g = (pixel.full >> 5) & 0x3F;   // Green component (6 bits)
        b = pixel.full & 0x1F;          // Blue component (5 bits)

        // Scale the 5-bit and 6-bit values to 8-bit (0-255) range
        r = (r * 255) / 31;
        g = (g * 255) / 63;
        b = (b * 255) / 31;
    #elif LV_COLOR_DEPTH == 32  // Assuming 32-bit ARGB8888 format
        r = pixel.ch.red;
        g = pixel.ch.green;
        b = pixel.ch.blue;
    #else
        // Handle other color depths as needed
        r = g = b = 0;  
    #endif

    // Calculate the grayscale value (simple average method)
    uint8_t grayscale = (r + g + b) / 3;

    // Map grayscale value (0-255) to 4-bit value (0-15)
    return grayscale >> 4;  // Shift to fit 4-bit range (0-15)
}


/* Dummy function to read the touchpad */
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    // Implement touchpad read functionality here, if needed
}

void lvgl_initialization(void) {
    lv_init();

    // Define display resolution
    uint32_t screen_width = 1200;
    uint32_t screen_height = 820;

    // Allocate LVGL draw buffer in PSRAM, adjust buffer size as necessary
    lv_color_t *buf_1 = (lv_color_t *)heap_caps_malloc((screen_width * screen_height)*2,  MALLOC_CAP_8BIT);
    lv_disp_draw_buf_init(&draw_buf, buf_1, NULL, screen_width * screen_height);

    // Initialize the display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screen_width;
    disp_drv.ver_res = screen_height;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;  // Use full buffer refresh due to lack of partial updates
    lv_disp_drv_register(&disp_drv);

    // Initialize input device driver (dummy)
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void getData() {
  HTTPClient http;
  http.begin(server);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println("server response");
    Serial.println(payload);

    // Parsiranje JSON odgovora
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      temperature = doc["main"]["temp"];
      humidity = doc["main"]["humidity"]; 
      wind= doc["wind"]["speed"];

      int visibility = doc["visibility"];
      const char* description = doc["weather"][0]["description"];
      long dt = doc["dt"];

        const char* name = doc["name"];
        const char* country = doc["sys"]["country"];

        nameS="LOCATION: "+String(name);
        countryS="COUNTRY: "+String(country);

      Wmsg = "#Description: " + String(description) + "  #Visbility: " + String(visibility) + " #Updated: "; //+ rtc.getTime();

      // Show temperature on serial monitor
      Serial.print("Temperature: ");
      Serial.print(temperature);

    } else {
      Serial.print("ERROR JSON-a: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("HTTP ERROR ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup() {
    
    Serial.begin(115200);
    Serial.println("start");
    display.begin();  

    //display.rtcSetTime(8, 5, 0);    // Send time to RTC
    //display.rtcSetDate(3, 9, 10, 2024); 

    WiFi.mode(WIFI_MODE_STA); 
    Serial.println("Connecting to WIFI");
    WiFi.begin(ssid, password);  
     while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
         Serial.print(".");
     }
    Serial.println("CONNECTED :)"); 
    delay(10);
   
    // Initialize LVGL and UI elements
    lvgl_initialization();
    ui_init();
    new_series = lv_chart_add_series(ui_Chart1, lv_color_hex(0x393939), LV_CHART_AXIS_PRIMARY_Y);

          getData();
          getMyTime();
          updateScreen();
}

unsigned long previousMillis = 0;
const long interval = 20000;   

void getMyTime()
{         display.rtcGetRtcData();           // Get the time and date from RTC
          minutes = display.rtcGetMinute();  // Store minutes in a variable
          hour = display.rtcGetHour(); 
          year=display.rtcGetYear();
          month=display.rtcGetMonth();
          day=display.rtcGetDay();
          if(minutes<10) mm="0"+String(minutes); else mm=String(minutes);
          if(hour<10) hh="0"+String(hour); else hh=String(hour);
          timeNow=hh+":"+mm;
}

void updateScreen()
{
       lv_calendar_set_today_date(ui_Calendar1, year, month, day); 
       lv_calendar_set_showed_date(ui_Calendar1, year, month);  
        Wmsg=Wmsg+timeNow;
        _ui_label_set_property(ui_weather, _UI_LABEL_PROPERTY_TEXT, Wmsg.c_str());
        _ui_label_set_property(ui_timeLBL, _UI_LABEL_PROPERTY_TEXT, timeNow.c_str());
        _ui_label_set_property(ui_temp, _UI_LABEL_PROPERTY_TEXT, String(temperature).substring(0,4).c_str());
        _ui_label_set_property(ui_hum, _UI_LABEL_PROPERTY_TEXT, String((int)humidity).c_str());
        _ui_label_set_property(ui_wind, _UI_LABEL_PROPERTY_TEXT, String((int)wind).c_str());
        _ui_label_set_property(ui_max, _UI_LABEL_PROPERTY_TEXT, String(maxT).c_str());
        _ui_label_set_property(ui_min, _UI_LABEL_PROPERTY_TEXT, String(minT).c_str());
        _ui_label_set_property(ui_Label15, _UI_LABEL_PROPERTY_TEXT, nameS.c_str());
        _ui_label_set_property(ui_Label16, _UI_LABEL_PROPERTY_TEXT, countryS.c_str());

        for(int i = 0; i < 24; i++) 
        lv_chart_set_value_by_id(ui_Chart1, new_series, i, PPgraph[i] );
            
        lv_chart_refresh(ui_Chart1);
        Serial.println("ajd");
}

void loop() {
    lv_timer_handler();  
    getMyTime();
    
    if(currentMinute!=minutes)
    {
      currentMinute=minutes;
      
          if(currentHour!=hour)
          {
            currentHour=hour;

            // ################part for voltage graph#################################
            PPtemp[23]=temperature;
           
            for(int i=23;i>0;i--)
            PPtemp[i-1]=PPtempT[i];

            maxT=-50;
            minT=16000;
            for(int i=0; i<24;i++){
            PPtempT[i]=PPtemp[i];
            if(PPtemp[i]<minT) minT=PPtemp[i];
            if(PPtemp[i]>maxT) maxT=PPtemp[i];
            }

            for(int i=0; i<24;i++)
            PPgraph[i]=map(PPtemp[i]*100,minT*100,maxT*100,0,100);
          }


      getData();
      updateScreen();
    }
     

    vTaskDelay(10 / portTICK_PERIOD_MS); 
}
