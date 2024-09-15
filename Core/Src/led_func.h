/*
 * led_func.h
 *
 *  Created on: Jul 16, 2024
 *      Author: lifei
 */
#pragma once
#ifndef SRC_LED_FUNC_H_
#define SRC_LED_FUNC_H_

void sendData(SPI_HandleTypeDef, uint8_t*, uint8_t );

void iterationRunRandomColor(SPI_HandleTypeDef, uint8_t*, uint8_t, uint8_t, uint8_t );
void starfall(SPI_HandleTypeDef, uint8_t*, uint8_t, uint8_t, int8_t );
void focusLens(SPI_HandleTypeDef, uint8_t*, uint8_t, uint8_t, uint8_t, int8_t );
void smoothAccumulation(SPI_HandleTypeDef, uint8_t*, uint8_t, uint8_t, int8_t );
void scannerKnightRider(SPI_HandleTypeDef, uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t );

#endif /* SRC_LED_FUNC_H_ */

