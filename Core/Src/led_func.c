/*
 * led_func.c
 *
 *  Created on: Jul 16, 2024
 *      Author: lifei
 */
#include "main.h"
#include "led_func.h"
#define LENSINPUT 1
#define DELAY 150

void sendData(SPI_HandleTypeDef hspix, uint8_t* data, uint8_t lenght){
	for (int j = 0; j <lenght; j++) {
		uint8_t spi_data[3 * 8];
		for (uint16_t i = 0; i < 3; i++)
		{
			for (uint8_t bit = 0; bit < 8; bit++)
			{
				if (*((data+i)+j*3) & (1 << (7 - bit))) {
					spi_data[i * 8 + bit + 0] = 0b110; //1
				}
				else {
					spi_data[i * 8 + bit + 0] = 0b100; //1
				}
			}
		}

    HAL_SPI_Transmit(&hspix, spi_data, sizeof(spi_data), HAL_MAX_DELAY);
	}
}
void sendReverseData(SPI_HandleTypeDef hspix, uint8_t* data, uint8_t lenght){
	for (int j = lenght-1;; j--) {
		uint8_t spi_data[3 * 8];
		for (uint16_t i = 0; i < 3; i++)
		{
			for (uint8_t bit = 0; bit < 8; bit++)
			{
				if (*((data+i)+j*3) & (1 << (7 - bit))) {
					spi_data[i * 8 + bit + 0] = 0b110; //1
				}
				else {
					spi_data[i * 8 + bit + 0] = 0b100; //1
				}
			}
		}

	HAL_SPI_Transmit(&hspix, spi_data, sizeof(spi_data), HAL_MAX_DELAY);
	if(j == 0)break;
	}
}

void iterationRunRandomColor(SPI_HandleTypeDef hspix, uint8_t* buffer, uint8_t lenght, uint8_t LUX, uint8_t type_run)
{
	for(uint8_t i = 0; i < lenght; i++) {
		*((buffer+0)+i*3) = rand()%LUX;
		*((buffer+1)+i*3) = rand()%LUX;
		*((buffer+2)+i*3) = rand()%LUX;
		if(type_run == 0)
			sendData(hspix,buffer, lenght);
		else if(type_run == 1)
			sendReverseData(hspix,buffer, lenght);
		HAL_Delay(DELAY);
		}
	memset(buffer, 0, lenght*3);
	sendData(hspix,buffer, lenght);
}

void starfall(SPI_HandleTypeDef hspix, uint8_t* buffer, uint8_t lenght, uint8_t LUX, int8_t type_run)
{
	uint8_t data[]={
			rand()%LUX,
			rand()%LUX,
			rand()%LUX,
			};
	for(uint8_t i = 0; i < lenght+3; i++) {
		HAL_Delay(DELAY);
		if(i<(lenght)){
			*((buffer+0)+i*3) = data[0];
			*((buffer+1)+i*3) = data[1];
			*((buffer+2)+i*3) = data[2];
		}

		if(i<(lenght+1) && i>0){
			*((buffer+0)+(i-1)*3) /= 2;
			*((buffer+1)+(i-1)*3) /= 2;
			*((buffer+2)+(i-1)*3) /= 2;
		}

		if(i<(lenght+2) && i>1){
			*((buffer+0)+(i-2)*3) /= 2;
			*((buffer+1)+(i-2)*3) /= 2;
			*((buffer+2)+(i-2)*3) /= 2;
		}
		if(i>2){
			*((buffer+0)+(i-3)*3) = 0;
			*((buffer+1)+(i-3)*3) = 0;
			*((buffer+2)+(i-3)*3) = 0;
		}

		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}

	}
	HAL_Delay(DELAY);
	memset(buffer, 0, lenght*3);
	sendData(hspix,buffer, lenght);
}

void focusLens(SPI_HandleTypeDef hspix, uint8_t* buffer, uint8_t lenght, uint8_t LUX, uint8_t lens_point, int8_t type_run){
	// 	smooth accumulation on the lens_point  followed by soft overfeeding
	uint8_t data[]={
				rand()%LUX,
				rand()%LUX,
				rand()%LUX,
				};

	for(uint8_t j = 0 ; j<LENSINPUT ; j++){
		for(uint8_t i = 0 ; i < lens_point -1; i++){
			*((buffer+0)+i*3)=data[0]/LENSINPUT*2;
			*((buffer+1)+i*3)=data[1]/LENSINPUT*2;
			*((buffer+2)+i*3)=data[2]/LENSINPUT*2;
			if(type_run == 0){
				sendData(hspix,buffer, lenght);
			}
			if(type_run == 1){
				sendReverseData(hspix,buffer, lenght);
			}
			*((buffer+0)+i*3)=0;
			*((buffer+1)+i*3)=0;
			*((buffer+2)+i*3)=0;
			HAL_Delay(DELAY/2);
		}
		*((buffer+0)+(lens_point-1)*3)+=data[0]/LENSINPUT;
		*((buffer+1)+(lens_point-1)*3)+=data[1]/LENSINPUT;
		*((buffer+2)+(lens_point-1)*3)+=data[2]/LENSINPUT;
		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}
		HAL_Delay(DELAY);
	}

	for(uint8_t j=0; j<lenght; j++){
		for(uint8_t i = lens_point; i<lenght ; i++){
			if(*((buffer+0)+i*3)<*((buffer+0)+(i-1)*3))
				*((buffer+0)+i*3)+=*((buffer+0)+(i-1)*3)/2;
			if(*((buffer+1)+i*3)<*((buffer+1)+(i-1)*3))
				*((buffer+1)+i*3)+=*((buffer+1)+(i-1)*3)/2;
			if(*((buffer+2)+i*3)<*((buffer+2)+(i-1)*3))
				*((buffer+2)+i*3)+=*((buffer+2)+(i-1)*3)/2;
		}

		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}
		HAL_Delay(DELAY/5);
		for(uint8_t i=lens_point-1; i<=j; i++){
			*((buffer+0)+(i-1)*3)/=4;
			*((buffer+1)+(i-1)*3)/=4;
			*((buffer+2)+(i-1)*3)/=4;
		}
		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}
		HAL_Delay(DELAY/10);
	}


		HAL_Delay(DELAY);
		memset(buffer, 0, lenght*3);
		sendData(hspix,buffer, lenght);
}

void smoothAccumulation(SPI_HandleTypeDef hspix, uint8_t* buffer, uint8_t lenght, uint8_t LUX, int8_t type_run){
	//smooth accumulation until the end
	uint8_t data[]={
					rand()%LUX,
					rand()%LUX,
					rand()%LUX,
					};

	for(uint8_t i = 0; i< lenght; i++){
		for(uint8_t k=0;k<LENSINPUT;k++){
			for(uint8_t j=0; j<lenght-i; j++){
				//set next LED, and remove last

				*((buffer+0)+j*3)+=data[0]/LENSINPUT;
				*((buffer+1)+j*3)+=data[1]/LENSINPUT;
				*((buffer+2)+j*3)+=data[2]/LENSINPUT;
				if(j>0){
				*((buffer+0)+(j-1)*3)=0;
				*((buffer+1)+(j-1)*3)=0;
				*((buffer+2)+(j-1)*3)=0;
				}
				if(type_run == 0){
					sendData(hspix, buffer, lenght);
				}
				if(type_run == 1){
					sendReverseData(hspix, buffer, lenght);
				}
				HAL_Delay(DELAY/10);
			}
		}
	}
	//reverse action
	for(int i=lenght; i>0; i--){
		for(uint8_t k=0;k<LENSINPUT;k++){
				for(uint8_t j=i; j<=lenght; j++){
					if(j<lenght){
						*((buffer+0)+j*3)+=data[0]/LENSINPUT;
					*((buffer+1)+j*3)+=data[1]/LENSINPUT;
					*((buffer+2)+j*3)+=data[2]/LENSINPUT;}
					*((buffer+0)+(j-1)*3)-=data[0]/LENSINPUT;
					*((buffer+1)+(j-1)*3)-=data[1]/LENSINPUT;
					*((buffer+2)+(j-1)*3)-=data[2]/LENSINPUT;
					if(type_run == 0){
						sendData(hspix, buffer, lenght);
					}
					if(type_run == 1){
						sendReverseData(hspix, buffer, lenght);
					}
					HAL_Delay(DELAY/10);
				}

		}


	}
	HAL_Delay(DELAY);
	memset(buffer, 0, lenght*3);
	sendData(hspix,buffer, lenght);

}

void scannerKnightRider(SPI_HandleTypeDef hspix, uint8_t* buffer, uint8_t lenght, uint8_t LUX, uint8_t points, uint8_t reapets, int8_t type_run){
	uint8_t data[]={
				rand()%LUX,
				rand()%LUX,
				rand()%LUX,
				};
	//start Left LED
	for(uint8_t i = 0; i < points; i++){
		HAL_Delay(DELAY);
		*((buffer+0)+i*3) = data[0];
		*((buffer+1)+i*3) = data[1];
		*((buffer+2)+i*3) = data[2];
		data[0] = *((buffer+0)+i*3);
		data[1] = *((buffer+1)+i*3);
		data[2] = *((buffer+2)+i*3);
		if(i>0)
			for(uint8_t j=i-1;j>=0;j--){
				*((buffer+0)+(j)*3) /= 2;
				*((buffer+1)+(j)*3) /= 2;
				*((buffer+2)+(j)*3) /= 2;
				if(j==0)break;
			}

		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}
	}
	HAL_Delay(DELAY);

	//loop
	for(int k=0;k<reapets;k++){
		//left to right LED
		for(uint8_t i = points; i < lenght; i++) {
				for(uint8_t j=i;j>=i-points;j--){
					uint8_t temp_data[3]={
							*((buffer+0)+(j)*3),
							*((buffer+1)+(j)*3),
							*((buffer+2)+(j)*3)
									};
					*((buffer+0)+(j)*3)=*((buffer+0)+(j-1)*3);
					*((buffer+1)+(j)*3)=*((buffer+1)+(j-1)*3);
					*((buffer+2)+(j)*3)=*((buffer+2)+(j-1)*3);

					*((buffer+0)+(j-1)*3)=temp_data[0];
					*((buffer+1)+(j-1)*3)=temp_data[1];
					*((buffer+2)+(j-1)*3)=temp_data[2];
					if(j==0)break;
				}
			if(type_run == 0){
				sendData(hspix,buffer, lenght);
			}
			if(type_run == 1){
				sendReverseData(hspix,buffer, lenght);
			}
			HAL_Delay(DELAY);
		}

		//start back to left
		if(k+1<reapets){
			for(uint8_t i=lenght; i>lenght-points; i--){
				for(uint8_t j=i;j<lenght;j++)
					{
					uint8_t temp_data[3]={
										*((buffer+0)+(j)*3),
										*((buffer+1)+(j)*3),
										*((buffer+2)+(j)*3)
												};
					*((buffer+0)+(j)*3)=*((buffer+0)+(j-1)*3);
					*((buffer+1)+(j)*3)=*((buffer+1)+(j-1)*3);
					*((buffer+2)+(j)*3)=*((buffer+2)+(j-1)*3);

					*((buffer+0)+(j-1)*3)=temp_data[0];
					*((buffer+1)+(j-1)*3)=temp_data[1];
					*((buffer+2)+(j-1)*3)=temp_data[2];
					}

				if(type_run == 0){
					sendData(hspix,buffer, lenght);
				}
				if(type_run == 1){
					sendReverseData(hspix,buffer, lenght);
				}
				HAL_Delay(DELAY);
				if(i==0)break;
			}

			memset(buffer, 0, lenght*3);
			for(uint8_t i = 0; i < points; i++){
					*((buffer+0)+i*3) = data[0];
					*((buffer+1)+i*3) = data[1];
					*((buffer+2)+i*3) = data[2];
					data[0] = *((buffer+0)+i*3);
					data[1] = *((buffer+1)+i*3);
					data[2] = *((buffer+2)+i*3);
					if(i>0)
						for(uint8_t j=i-1;j>=0;j--){
							*((buffer+0)+(j)*3) /= 2;
							*((buffer+1)+(j)*3) /= 2;
							*((buffer+2)+(j)*3) /= 2;
							if(j==0)break;
						}
				}

			if(type_run)type_run=0;
			else type_run=1;
		}
	}


	//end_right
	for(uint8_t i=lenght-points; i<lenght; i++){
		*((buffer+0)+i*3)=0;
		*((buffer+1)+i*3)=0;
		*((buffer+2)+i*3)=0;
		for(uint8_t j=i;j<lenght;j++){
			*((buffer+0)+j*3)/=2;
			*((buffer+1)+j*3)/=2;
			*((buffer+2)+j*3)/=2;
		}
		if(type_run == 0){
			sendData(hspix,buffer, lenght);
		}
		if(type_run == 1){
			sendReverseData(hspix,buffer, lenght);
		}
		HAL_Delay(DELAY);
	}

	memset(buffer, 0, lenght*3);
	sendData(hspix,buffer, lenght);
}
