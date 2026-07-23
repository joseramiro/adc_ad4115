/**
 * @file plib_adc4115.h
 * @author Ramiro Najera
 * @brief Pilote pour l'AD4115 (125 kSPS, 24-bit, Sigma-Delta ADC with 16 inputs)
 * @date 2026-07-10
 */

#ifndef PLIB_AD4115_H
#define PLIB_AD4115_H

#include <stdint.h>
#include "libs/common_c_libs/plib_comm_struct.h"

typedef enum
{
    INPUT_VIN0_VIN1         = 0b0000000001,
    INPUT_VIN0_VINCOM       = 0b0000010000,

    INPUT_VIN1_VIN0         = 0b0000100000,
    INPUT_VIN1_VINCOM       = 0b0000110000,

    INPUT_VIN2_VIN3         = 0b0001000011,
    INPUT_VIN2_VINCOM       = 0b0001010000,

    INPUT_VIN3_VIN2         = 0b0001100010,
    INPUT_VIN3_VINCOM       = 0b0001110000,

    INPUT_VIN4_VIN5         = 0b0010000101,
    INPUT_VIN4_VINCOM       = 0b0010010000,

    INPUT_VIN5_VIN4         = 0b0010100100,
    INPUT_VIN5_VINCOM       = 0b0010110000,

    INPUT_VIN6_VIN7         = 0b0011000111,
    INPUT_VIN6_VINCOM       = 0b0011010000,

    INPUT_VIN7_VIN6         = 0b0011100110,
    INPUT_VIN7_VINCOM       = 0b0011110000,

    INPUT_VIN8_VIN9         = 0b0100001001,
    INPUT_VIN8_VINCOM       = 0b0100010000,

    INPUT_VIN9_VIN8         = 0b0100101000,
    INPUT_VIN9_VINCOM       = 0b0100110000,

    INPUT_VIN10_VIN11       = 0b0101001011,
    INPUT_VIN10_VINCOM      = 0b0101010000,

    INPUT_VIN11_VIN10       = 0b0101101010,
    INPUT_VIN11_VINCOM      = 0b0101110000,

    INPUT_VIN12_VIN13       = 0b0110001101,
    INPUT_VIN12_VINCOM      = 0b0110010000,

    INPUT_VIN13_VIN12       = 0b0110101100,
    INPUT_VIN13_VINCOM      = 0b0110110000,

    INPUT_VIN14_VIN15       = 0b0111001111,
    INPUT_VIN14_VINCOM      = 0b0111010000,

    INPUT_VIN15_VIN14       = 0b0111101110,
    INPUT_VIN15_VINCOM      = 0b0111110000,

    INPUT_TEMPERATURE       = 0b1000110010,
    INPUT_REFERENCE         = 0b1010110110,

} AD4115_InputSelection_t;


typedef enum
{
    UNIPOLAR        = 0x0000,
    BIPOLAR         = 0x1000,
	REFBUF_P        = 0x0800,      /**< bit 11 REFBUF+ This bit enables or disables the REF+ input buffer.*/
    REFBUF_N        = 0x0400,      /**< bit 10 REFBUF- This bit enables or disables the REF- input buffer. */
    /**
     * bits 9..8 INBUF enables or disables input buffers:
     * 00 = disable input buffers
     * 01 = Reserved
     * 10 = Reserved
     * 11 = enable input buffers
     */
	INPUT_BUFFERS   = 0x0300,
    /**
     * bits 7..6 are reserved. Set to 0
     */
    /**
     * bits	5..4 REF_SEL selects the reference source for ADC conversion.
	   00 = External reference (REF+-).
	   10 = Internal reference (2.5V). Must also be enabled in via ADCMODE.
	   11 = AVDD - AVSS. Low voltage reference.
     */

	EXTERNAL_REF    = 0x0000,
	INTERNAL_REF    = 0x0020,
	ADD_AVSS_REF    = 0x0030,

    RESERVED        = 0x0000,  /**< bits 3..0 are reserved. Set to 0*/

} AD4115_SetupConfiguration_t;



typedef union
{
    uint16_t value;
    struct
    {
        uint16_t input_selection    : 10;   // Bits 9:0
        uint16_t reserved           : 2;    // Bits 11:10
        uint16_t setup_selection    : 3;    // Bits 14:12
        uint16_t enabled            : 1;    // Bit 15
    } bits;
} AD4115_ChannelRegister_t;

typedef struct
{
    uint8_t channel;
    AD4115_ChannelRegister_t conf;
} AD4115_ChannelConf_t;

typedef struct
{
    uint8_t id;         /**< Identifiant de l'objet */
    SPI_t spi;          /**< Configuration SPI */
    AD4115_ChannelConf_t *conf;
} AD4115_t;

uint8_t AD4115_Init(AD4115_t *obj);

uint8_t AD4115_CheckConnection(AD4115_t *obj);

#endif  // PLIB_AD4115_H