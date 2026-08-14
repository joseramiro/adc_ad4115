/**
 * @file plib_adc4115.h
 * @author Ramiro Najera
 * @brief Pilote pour l'AD4115 (125 kSPS, 24-bit, Sigma-Delta ADC with 16 inputs)
 * @date 2026-07-10
 */

#ifndef PLIB_AD4115_H
#define PLIB_AD4115_H

#include <stdint.h>

#include "plib_comm_struct.h"

// Enums concerning ADC Mode Regiter

typedef enum
{
    AD4115_REF_EN_DISABLED          = 0,
    AD4115_REF_EN_ENABLED           = 1
} AD4115_RefEnable_t;

typedef enum
{
    AD4115_SING_CYC_DISABLED        = 0,
    AD4115_SING_CYC_ENABLED         = 1
} AD4115_SingleCycle_t;

typedef enum
{
    AD4115_DELAY_0US                = 0b000,
    AD4115_DELAY_8US                = 0b001,
    AD4115_DELAY_32US               = 0b010,
    AD4115_DELAY_80US               = 0b011,
    AD4115_DELAY_200US              = 0b100,
    AD4115_DELAY_400US              = 0b101,
    AD4115_DELAY_1MS                = 0b110,
    AD4115_DELAY_2MS                = 0b111
} AD4115_ChannelSwitchDelay_t;

typedef enum
{
    AD4115_MODE_CONTINUOUS_CONVERSION = 0b000,
    AD4115_MODE_SINGLE_CONVERSION     = 0b001,
    AD4115_MODE_STANDBY               = 0b010,
    AD4115_MODE_POWER_DOWN            = 0b011,
    AD4115_MODE_INTERNAL_OFFSET_CAL   = 0b100,
    AD4115_MODE_INTERNAL_GAIN_CAL     = 0b101,
    AD4115_MODE_SYSTEM_OFFSET_CAL     = 0b110,
    AD4115_MODE_SYSTEM_GAIN_CAL       = 0b111
} AD4115_OperatingMode_t;

typedef enum
{
    AD4115_CLOCKSEL_INTERNAL_OSC      = 0b00,
    AD4115_CLOCKSEL_INTERNAL_OSC_OUT  = 0b01,
    AD4115_CLOCKSEL_EXTERNAL_CLOCK    = 0b10,
    AD4115_CLOCKSEL_EXTERNAL_CRYSTAL  = 0b11
} AD4115_ClockSelection_t;

// Enums concerning Interface Mode Register

typedef enum
{
    AD4115_ALT_SYNC_DISABLED        = 0,
    AD4115_ALT_SYNC_ENABLED         = 1
} AD4115_AltSync_t;

typedef enum
{
    AD4115_IO_STRENGTH_DISABLED     = 0,
    AD4115_IO_STRENGTH_ENABLED      = 1
} AD4115_IoStrength_t;

typedef enum
{
    AD4115_DOUT_RESET_DISABLED      = 0,
    AD4115_DOUT_RESET_ENABLED       = 1
} AD4115_DoutReset_t;

typedef enum
{
    AD4115_CONTREAD_DISABLED        = 0,
    AD4115_CONTREAD_ENABLED         = 1
} AD4115_Contread_t;

typedef enum
{
    AD4115_DATA_STAT_DISABLED       = 0,
    AD4115_DATA_STAT_ENABLED        = 1
} AD4115_DataStatMode_t;

typedef enum
{
    AD4115_REG_CHECK_DISABLED       = 0,
    AD4115_REG_CHECK_ENABLED        = 1
} AD4115_RegCheck_t;

typedef enum
{
    AD4115_CRC_DISABLED             = 0b00,
    AD4115_CRC_READ_ENABLED         = 0b01,
    AD4115_CRC_READ_WRITE_ENABLED   = 0b10
} AD4115_CrcEnabled_t;

typedef enum
{
    AD4115_WL16_24                  = 0,
    AD4115_WL16_16                  = 1
} AD4115_Wl16_t;

// Enums concerning Setup Configuration Register

typedef enum
{
    AD4115_UNIPOLAR                 = 0, /**< Unipolar coded output */
    AD4115_BIPOLAR                  = 1, /**< Bipolar coded output */
} AD4115_OutputCoding_t;

typedef enum
{
    REFBUF_P_DISABLED               = 0, /**< REF+ buffer disabled */
    REFBUF_P_ENABLED                = 1  /**< REF+ buffer enabled */
} AD4115_RefBufP_t;

typedef enum
{
    REFBUF_N_DISABLED               = 0, /**< REF- buffer disabled */
    REFBUF_N_ENABLED                = 1  /**< REF- buffer enabled */
} AD4115_RefBufN_t;

typedef enum
{
    AD4115_INBUF_DISABLED           = 0b00, /**< Input buffer disabled */
    AD4115_INBUF_ENABLED            = 0b11  /**< Input buffer enabled */
} AD4115_InputBuffer_t;

typedef enum
{
    AD4115_REF_SEL_EXT_REF          = 0b00, /**< External reference, REF+- */
    AD4115_REF_SEL_INT_REF          = 0b10, /**< Internal 2.5 V reference, must be enabled via ADCMODE */
    AD4115_REF_SEL_ADD_AVSS         = 0b11  /**< AVDD-AVSS */
} AD4115_RefSelection_t;

// Enums concerning Filter Configuration Register

typedef enum
{
    AD4115_ENHFILTEN_DISABLED       = 0,    /**< Postfilters for enhanced 50 Hz/60 Hz rejection disabled */
    AD4115_ENHFILTEN_ENABLED        = 1     /**< Postfilters for enhances 50 Hz/60 Hz rejection enabled*/
} AD4115_ENHFilterEnable_t;

typedef enum
{
    AD4115_ENHFILT_27SPS            = 0b00, /**< 27 SPS, 47 dB rejection, 36.7 ms settling */
    AD4115_ENHFILT_25SPS            = 0b01, /**< 25 SPS, 62 dB rejection, 40 ms settling */
    AD4115_ENHFILT_20SPS            = 0b10, /**< 20 SPS, 86 dB rejection, 50 ms settling */
    AD4115_ENHFILT_16PSP            = 0b11  /**< 16.67 SPS, 92 dB rejection, 60 ms settling */
} AD4115_ENHFILT_t;

typedef enum
{
    AD4115_ORDER_SINC5_SINC1        = 0b00, /**< Sinc5 + sinc1 (default) */
    AD4115_ORDER_SINC3              = 0b11  /**< Sinc3 */
} AD4115_Order_t;

typedef enum
{
    AD4115_ODR_125000_SPS           = 0b00000,
    AD4115_ODR_62500_SPS            = 0b00010,
    AD4115_ODR_31250_SPS            = 0b00100,
    AD4115_ODR_25000_SPS            = 0b00101,
    AD4115_ODR_15625_SPS            = 0b00110,
    AD4115_ODR_10390_SPS            = 0b00111,
    AD4115_ODR_4994_SPS             = 0b01000,
    AD4115_ODR_2498_SPS             = 0b01001,
    AD4115_ODR_1000_SPS             = 0b01010,
    AD4115_ODR_500_SPS              = 0b01011,
    AD4115_ODR_395_5_SPS            = 0b01100,  // 400.6 SPS for sinc3
    AD4115_ODR_200_SPS              = 0b01101,
    AD4115_ODR_100_SPS              = 0b01110,
    AD4115_ODR_59_87_SPS            = 0b01111,  // 59.87 SPS
    AD4115_ODR_49_92_SPS            = 0b10000,  // 50 SPS for sinc3
    AD4115_ODR_20_SPS               = 0b10001,
    AD4115_ODR_16_67_SPS            = 0b10010,  // 16.67 SPS
    AD4115_ODR_10_SPS               = 0b10011,
    AD4115_ODR_5_SPS                = 0b10100,
    AD4115_ODR_25_SPS               = 0b10101
} AD4115_OutputDataRate_t;

// Enums concerning Channel Register

typedef enum
{
    AD4115_CH_DISABLED              = 0, /**< Channel disabled */
    AD4115_CH_ENABLED               = 1  /**< Channel enabled */
} AD4115_ChannelEnabled_t;

typedef enum
{
    AD4115_SETUP_SEL_0              = 0b000, /**< Setup 0 */
    AD4115_SETUP_SEL_1              = 0b001, /**< Setup 1 */
    AD4115_SETUP_SEL_2              = 0b010, /**< Setup 2 */
    AD4115_SETUP_SEL_3              = 0b011, /**< Setup 3 */
    AD4115_SETUP_SEL_4              = 0b100, /**< Setup 4 */
    AD4115_SETUP_SEL_5              = 0b101, /**< Setup 5 */
    AD4115_SETUP_SEL_6              = 0b110, /**< Setup 6 */
    AD4115_SETUP_SEL_7              = 0b111  /**< Setup 7 */
} AD4115_SetupSelectiont_t;

typedef enum
{
    AD4115_INPUT_VIN0_VIN1          = 0b0000000001,
    AD4115_INPUT_VIN0_VINCOM        = 0b0000010000,
    AD4115_INPUT_VIN1_VIN0          = 0b0000100000,
    AD4115_INPUT_VIN1_VINCOM        = 0b0000110000,
    AD4115_INPUT_VIN2_VIN3          = 0b0001000011,
    AD4115_INPUT_VIN2_VINCOM        = 0b0001010000,
    AD4115_INPUT_VIN3_VIN2          = 0b0001100010,
    AD4115_INPUT_VIN3_VINCOM        = 0b0001110000,
    AD4115_INPUT_VIN4_VIN5          = 0b0010000101,
    AD4115_INPUT_VIN4_VINCOM        = 0b0010010000,
    AD4115_INPUT_VIN5_VIN4          = 0b0010100100,
    AD4115_INPUT_VIN5_VINCOM        = 0b0010110000,
    AD4115_INPUT_VIN6_VIN7          = 0b0011000111,
    AD4115_INPUT_VIN6_VINCOM        = 0b0011010000,
    AD4115_INPUT_VIN7_VIN6          = 0b0011100110,
    AD4115_INPUT_VIN7_VINCOM        = 0b0011110000,
    AD4115_INPUT_VIN8_VIN9          = 0b0100001001,
    AD4115_INPUT_VIN8_VINCOM        = 0b0100010000,
    AD4115_INPUT_VIN9_VIN8          = 0b0100101000,
    AD4115_INPUT_VIN9_VINCOM        = 0b0100110000,
    AD4115_INPUT_VIN10_VIN11        = 0b0101001011,
    AD4115_INPUT_VIN10_VINCOM       = 0b0101010000,
    AD4115_INPUT_VIN11_VIN10        = 0b0101101010,
    AD4115_INPUT_VIN11_VINCOM       = 0b0101110000,
    AD4115_INPUT_VIN12_VIN13        = 0b0110001101,
    AD4115_INPUT_VIN12_VINCOM       = 0b0110010000,
    AD4115_INPUT_VIN13_VIN12        = 0b0110101100,
    AD4115_INPUT_VIN13_VINCOM       = 0b0110110000,
    AD4115_INPUT_VIN14_VIN15        = 0b0111001111,
    AD4115_INPUT_VIN14_VINCOM       = 0b0111010000,
    AD4115_INPUT_VIN15_VIN14        = 0b0111101110,
    AD4115_INPUT_VIN15_VINCOM       = 0b0111110000,
    AD4115_INPUT_TEMPERATURE        = 0b1000110010,
    AD4115_INPUT_REFERENCE          = 0b1010110110,
} AD4115_InputSelection_t;

// Register list

typedef union
{
    uint16_t value;

    struct
    {
        uint16_t reserved0          : 2;   // Bits 1:0
        uint16_t clocksel           : 2;   // Bits 3:2
        uint16_t mode               : 3;   // Bits 6:4
        uint16_t reserved1          : 1;   // Bit 7
        uint16_t delay              : 3;   // Bits 10:8
        uint16_t reserved2          : 2;   // Bits 12:11
        uint16_t sing_cyc           : 1;   // Bit 13
        uint16_t reserved3          : 1;   // Bit 14
        uint16_t ref_en             : 1;   // Bit 15
    } bits;

} AD4115_ModeRegister_t;

typedef union
{
    uint16_t value;

    struct
    {
        uint16_t wl16               : 1;    // Bit 0
        uint16_t reserved0          : 1;    // Bit 1
        uint16_t crc_en             : 2;    // Bits 3:2
        uint16_t reserved1          : 1;    // Bit 4
        uint16_t reg_check          : 1;    // Bit 5
        uint16_t data_stat          : 1;    // Bit 6
        uint16_t contread           : 1;    // Bit 7
        uint16_t dout_reset         : 1;    // Bit 8
        uint16_t reserved2          : 2;    // Bits 10:9
        uint16_t io_strength        : 1;    // Bit 11
        uint16_t alt_sync           : 1;    // Bit 12
        uint16_t reserved3          : 3;    // Bit 15:13
    } bits;
    
} AD4115_InterfaceRegister_t;

typedef union
{
    uint16_t value;

    struct
    {
        uint16_t input_selection    : 10;   /**< Bits 9:0 */
        uint16_t reserved           : 2;    /**< Bits 11:10 */
        uint16_t setup_selection    : 3;    /**< Bits 14:12 */
        uint16_t enabled            : 1;    /**< Bit 15 */
    } bits;

} AD4115_ChannelConfRegister_t;

typedef union
{
    uint16_t value;

    struct
    {
        uint16_t reserved0          : 4;    /**< Bits 3:0 */
        uint16_t ref_sel            : 2;    /**< Bits 5:4 */
        uint16_t reserved1          : 2;    /**< Bits 7:6 */
        uint16_t inbuf              : 2;    /**< Bits 9:8 */
        uint16_t refbuf_minus       : 1;    /**< Bit 10 */
        uint16_t refbuf_plus        : 1;    /**< Bit 11 */
        uint16_t bi_unipolar        : 1;    /**< Bit 12 */
        uint16_t reserved2          : 3;    /**< Bits 15:13 */
    } bits;

} AD4115_SetupRegister_t;

typedef union
{
    uint16_t value;

    struct
    {
        uint16_t odr                : 5;    /**< Bits 4:0 */
        uint16_t order              : 2;    /**< Bits 6:5 */
        uint16_t reserved0          : 1;    /**< Bit 7 */
        uint16_t enhfilt            : 3;    /**< Bits 10:8 */
        uint16_t enhfilten          : 1;    /**< Bit 11 */
        uint16_t reserved1          : 3;    /**< Bits 14:12 */
        uint16_t sinc3_map          : 1;    /**< Bit 15 */
    } bits;

} AD4115_FilterRegister_t;

// Structs for object

typedef struct
{
    uint8_t channel;
    AD4115_ChannelConfRegister_t conf;
} AD4115_ChannelConf_t;

typedef struct
{
    SPI_t spi;          /**< Configuration SPI */
    uint8_t channels_count;
    // Registers
    AD4115_ModeRegister_t mode_reg;
    AD4115_InterfaceRegister_t interface_reg;
    AD4115_SetupRegister_t setup_reg;
    AD4115_FilterRegister_t filter_reg;
    // Channels configuration
    AD4115_ChannelConf_t *channels_conf;
    
} AD4115_t;

uint8_t AD4115_Init(AD4115_t *obj);

void AD4115_Reset(AD4115_t *obj);

uint8_t AD4115_CheckId(AD4115_t *obj);

uint8_t AD4115_UpdateMeasure(AD4115_t *obj, uint8_t *channel, uint32_t *data);


#endif  // PLIB_AD4115_H