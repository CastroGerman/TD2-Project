/*
 * PR_Serie.h
 *
 *  Created on: 09/07/2013
 *      Author: Pablo
 */

#ifndef PR_SERIE_H_
#define PR_SERIE_H_

#include "UART.h"
#include "lz4.h"

#define TXBUFFER_SIZE	200
#define RXBUFFER_SIZE	200
#define CANT 3


uint8_t Serial_PushTx(uint8_t dato);
uint8_t Serial_PopRx(uint8_t *dato);
uint8_t Serial_Send(const uint8_t *buffer, uint8_t count);
void Serial_RxIntHandler(void);
void Serial_TxIntHandler(void);

void SerialManager (void);

#define N_LEDS 32
#define N_CIR N_LEDS*4
#define N_PIX N_LEDS * N_CIR
#define IW (Image.Buffer_Index)
#define MYIR ((Image.Buffer_Index + 1) % 2)

/**
 * @struct	Buffer
 * @brief	Buffer de descompresion
 *
 * En esta estructura se almacenan los datos post-procesado por el algortmo de descompresion lz4.
 * Contiene en si la matriz de la imagen completa a imprimir.
 */
struct Buffer
{
    uint8_t 	Buffers[2][N_PIX];
    uint8_t 	Stream_Buffer[LZ4_COMPRESSBOUND(N_PIX)];
    uint8_t		Buffer_Index;
    uint16_t	Stream_Size;

};

extern struct Buffer Image;
#endif /* PR_SERIE_H_ */
