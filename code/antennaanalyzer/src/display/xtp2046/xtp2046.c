#include "xtp2046.h"
#include <string.h>
#include <math.h>

#define Z_THRESHOLD   3500 // Note: reversed for backwards compatiblity: 4095-x
#define MAX_OVERSAMPLING 32

uint8_t oversampling = MAX_OVERSAMPLING;

void xtp2046_select()
{
    MX_SPI1_ReInit(SPI_BAUDRATEPRESCALER_64);
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
}

void xtp2046_deselect()
{
    HAL_Delay(2);
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
    MX_SPI1_ReInit(SPI_BAUDRATEPRESCALER_2);
}

uint8_t spi_transfer(uint8_t tx)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 10);
    HAL_Delay(2);
    return rx;
}

uint16_t spi_transfer_16(uint16_t tx)
{
    uint16_t rx;
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&tx, (uint8_t*)&rx, 2, 10);
    HAL_Delay(2);
    return rx;
}

void xtp2046_init()
{
    xtp2046_deselect();
}

uint16_t _xpt2046_get_reading( uint8_t control )
{
	uint8_t tData[3] = { control , 0 , 0 };
	uint8_t rData[3] = { 0 , 0 , 0 };

	HAL_SPI_TransmitReceive(&hspi1, tData, rData, 3, 10);

	if ( ( control & 0x08 ) == 0 ) {
		return ( rData[1] << 5 ) | ( rData[2] >> 3 );
	}
	return ( rData[1] << 4 ) | ( rData[2] >> 4 );
}

touch_t g_touch = {0};

touch_t xtp2046_get_reading()
{
    touch_t touch = {0};
	int i;
	int allX[ 7 ] , allY[ 7 ];
    xtp2046_select();
	_xpt2046_get_reading( 0xd1 );
	_xpt2046_get_reading( 0x91 );
	for ( i = 0 ; i < 7 ; i ++ ) {
		allX[ i ] = _xpt2046_get_reading( 0xd1 );
		allY[ i ] = _xpt2046_get_reading( 0x91 );
	}

	int j;
	for ( i = 0 ; i < 4 ; i ++ ) {
		for ( j = i ; j < 7 ; j ++ ) {
			int temp = allX[ i ];
			if ( temp > allX[ j ] ) {
				allX[ i ] = allX[ j ];
				allX[ j ] = temp;
			}
			temp = allY[ i ];
			if ( temp > allY[ j ] ) {
				allY[ i ] = allY[ j ];
				allY[ j ] = temp;
			}
		}
	}
	_xpt2046_get_reading( 0x90 );

    xtp2046_deselect();
    
    if (HAL_GPIO_ReadPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin) != GPIO_PIN_SET) {
		touch.touched = 1;
        touch.pressure = 1;
	}
    else
    {
        touch.touched = 0;
        touch.pressure = 0;
    }

	touch.x = allX[ 3 ];
	touch.y = allY[ 3 ];

	return touch;
}

touch_t xtp2046_update()
{
	int nRead = 0;
	int xAcc = 0 , yAcc = 0;
    int nSamples = 10;

	while ( nRead < nSamples ) {
        touch_t touch = xtp2046_get_reading();
        if(!touch.touched)
            break;
		
		xAcc += touch.x;
		yAcc += touch.y;
		nRead++;
	}

	if ( nRead == 0 ) {
		g_touch.touched = 0;
	}
    else
    {
        g_touch.touched = 1;
	    g_touch.y = xAcc / nRead;
	    g_touch.x = yAcc / nRead;

        if(g_touch.x < 150)
            g_touch.x = 150;

        if(g_touch.y < 300)
            g_touch.y = 300;

        g_touch.y = (g_touch.y - 300) * 240 / (3800-300);
        g_touch.x = (g_touch.x - 150) * 320 / (3800-150);

        if(g_touch.x > 239)
            g_touch.x = 239;
        
        if(g_touch.y > 319)
            g_touch.y = 319;
    }

	return g_touch;
}