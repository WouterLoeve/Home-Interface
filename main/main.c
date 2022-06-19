#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "ssd1306_draw.h"
#include "ssd1306_font.h"
#include "ssd1306_default_if.h"

#include "TempSensor.h"

#define OLED_USE_I2C_DISPLAY CONFIG_OLED_USE_I2C_DISPLAY
#define PLAY_REMOTE CONFIG_PLAY_REMOTE

#if OLED_USE_I2C_DISPLAY
    static const int I2CDisplayAddress = 0x3C;
    static const int I2CDisplayWidth = 128;
    static const int I2CDisplayHeight = 64;
    static const int I2CResetPin = -1;

    struct SSD1306_Device Display;
#else
    static const int SPIDisplayChipSelect = 12;
    static const int SPIDisplayWidth = 128;
    static const int SPIDisplayHeight = 64;
    static const int SPIResetPin = 16;

    struct SSD1306_Device Display;
#endif

bool DefaultBusInit( void ) {
    #if OLED_USE_I2C_DISPLAY
        assert( SSD1306_I2CMasterInitDefault( ) == true );
        assert( SSD1306_I2CMasterAttachDisplayDefault( &Display, I2CDisplayWidth, I2CDisplayHeight, I2CDisplayAddress, I2CResetPin ) == true );
    #else
        assert( SSD1306_SPIMasterInitDefault( ) == true );
        assert( SSD1306_SPIMasterAttachDisplayDefault( &Display, SPIDisplayWidth, SPIDisplayHeight, SPIDisplayChipSelect, SPIResetPin ) == true );
    #endif

    return true;
}

void draw_text_anchored(int color, TextAnchor anchor, char *text) {
	SSD1306_FontDrawAnchoredString( &Display, anchor, text, color);
}

void draw_text(int color, char *text, int x, int y) {
	SSD1306_FontDrawString(&Display, x, y, text, color);
}

void draw_outlined_text(TextAnchor anchor, char *text, bool fill) {
	int x = 0;
	int y = 0;
	SSD1306_FontGetAnchoredStringCoords( &Display, &x, &y, anchor, text);
	int width = SSD1306_FontMeasureString(&Display, text);
	int height = SSD1306_FontGetHeight(&Display);

	if (fill) {
		SSD1306_DrawBox( &Display, x, y, x+width - 1, y+height -1, SSD_COLOR_WHITE, fill);
	}

	int color = SSD_COLOR_WHITE;
	if (fill) {
		color = SSD_COLOR_BLACK;
	}
	draw_text_anchored(color, anchor, text);
}

void write_text_offset(TextAnchor anchor, char* text, int x_offset, int y_offset) {
	int OutX;
	int OutY;

	SSD1306_FontGetAnchoredStringCoords( &Display, &OutX, &OutY, anchor, text);

	draw_text(SSD_COLOR_WHITE, text, OutX+x_offset, OutY+y_offset);
}

/*
 *  Puts temperature and humidity into pointer location
 */
void get_temperature(float *temperature, float *humidity) {
    setDHTgpio( 4 );

    for (int i = 0; i < 5; i++) {
        int ret = readDHT();
            
        errorHandler(ret);

        *temperature = getTemperature();
        *humidity = getHumidity();

        // printf( "Hum %.1f\n", *humidity );
        // printf( "Tmp %.1f\n", *temperature );
        if ((*temperature == 0 && *humidity == 0) || *humidity == 0) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        } else {
            break;
        }

    }
}


void app_main( void ) {
	if ( DefaultBusInit( ) == true ) {
		SSD1306_SetFont( &Display, &Font_liberation_mono_13x21 );
		SSD1306_Clear( &Display, SSD_COLOR_BLACK );
		SSD1306_Update( &Display );
	}

	while(true) {
		SSD1306_Clear( &Display, SSD_COLOR_BLACK );
		
		float temp;
		float hum;
		get_temperature(&temp, &hum);

		char hum_str[10];
		char temp_str[10];
		sprintf(hum_str, "%.1f%%", hum);
		sprintf(temp_str, "%.1fC", temp);

		// TODO: Draw pictures of a water drop and a termometer
		// TODO: Activate screen and measurement with buttons
		// TODO: Light switches for Klik aan klik uit
		// TODO: Light sensor with photodiode

		write_text_offset(TextAnchor_North, temp_str, 0, 0);
		write_text_offset(TextAnchor_South, hum_str, 0, 0);

		SSD1306_Update( &Display );

		vTaskDelay(pdMS_TO_TICKS(10000));
	}

	
}