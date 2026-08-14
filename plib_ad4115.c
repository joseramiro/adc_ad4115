#include "plib_ad4115.h"

#include "libs/plib_data_struct/plib_data_struct.h"
#include "libs/common_c_libs/plib_delay.h"

#define WEN_READ_CMD_MASK       0b01000000

#define STATUS_RDY_MASK         0b10000000
#define STATUS_ADC_ERROR_MASK   0b01000000
#define STATUS_CRC_ERROR_MASK   0b00100000
#define STATUS_REG_ERROR_MASK   0b00010000

#define CHANNEL_MASK            0b00001111

#define DATA_STAT_ENABLED_MASK  0b01000000

#define MANUFACTURER_ID         0x38D0

typedef enum
{
    STATUS_REG              = 0b000000,
    ADC_MODE_REG            = 0b000001,
    INTERFACE_MODE_REG      = 0b000010,
    REGISTER_CHECKSUM_REG   = 0b000011,
    DATA_REG                = 0b000100,
    GPIO_CONFIG_REG         = 0b000110,
    ID_REG                  = 0b000111,

    CHANNEL0_REG            = 0b010000,
    CHANNEL1_REG            = 0b010001,
    CHANNEL2_REG            = 0b010010,
    CHANNEL3_REG            = 0b010011,
    CHANNEL4_REG            = 0b010100,
    CHANNEL5_REG            = 0b010101,
    CHANNEL6_REG            = 0b010110,
    CHANNEL7_REG            = 0b010111,
    CHANNEL8_REG            = 0b011000,
    CHANNEL9_REG            = 0b011001,
    CHANNEL10_REG           = 0b011010,
    CHANNEL11_REG           = 0b011011,
    CHANNEL12_REG           = 0b011100,
    CHANNEL13_REG           = 0b011101,
    CHANNEL14_REG           = 0b011110,
    CHANNEL15_REG           = 0b011111,

    SETUP_CONFIG0_REG       = 0b100000,
    SETUP_CONFIG1_REG       = 0b100001,
    SETUP_CONFIG2_REG       = 0b100010,
    SETUP_CONFIG3_REG       = 0b100011,
    SETUP_CONFIG4_REG       = 0b100100,
    SETUP_CONFIG5_REG       = 0b100101,
    SETUP_CONFIG6_REG       = 0b100110,
    SETUP_CONFIG7_REG       = 0b100111,

    FILTER_CONFIG0_REG      = 0b101000,
    FILTER_CONFIG1_REG      = 0b101001,
    FILTER_CONFIG2_REG      = 0b101010,
    FILTER_CONFIG3_REG      = 0b101011,
    FILTER_CONFIG4_REG      = 0b101100,
    FILTER_CONFIG5_REG      = 0b101101,
    FILTER_CONFIG6_REG      = 0b101110,
    FILTER_CONFIG7_REG      = 0b101111,

    OFFSET0_REG             = 0b110000,
    OFFSET1_REG             = 0b110001,
    OFFSET2_REG             = 0b110010,
    OFFSET3_REG             = 0b110011,
    OFFSET4_REG             = 0b110100,
    OFFSET5_REG             = 0b110101,
    OFFSET6_REG             = 0b110110,
    OFFSET7_REG             = 0b110111,

    GAIN0_REG               = 0b111000,
    GAIN1_REG               = 0b111001,
    GAIN2_REG               = 0b111010,
    GAIN3_REG               = 0b111011,
    GAIN4_REG               = 0b111100,
    GAIN5_REG               = 0b111101,
    GAIN6_REG               = 0b111110,
    GAIN7_REG               = 0b111111,

} AD4115_Register_t;

/*==============================================================================
 * Private function prototypes
 *============================================================================*/

static void Write(SPI_t *spi, uint8_t* data, const uint16_t size);
static void Read(SPI_t *spi, uint8_t* data, const uint16_t size);
static void StartTranmission(SPI_t *spi);
static void EndTramission(SPI_t *spi);
static void ReadRegister(SPI_t *spi, uint8_t reg, uint8_t *data, uint8_t len);
static void WriteRegister(SPI_t *spi, uint8_t reg, uint8_t *data, uint8_t len);
static void WriteRegister16(SPI_t *spi, uint8_t reg, uint16_t value);
static void Reset(AD4115_t *obj);
static void ReadId(AD4115_t *obj, uint8_t *data);
static void ReadStatus(AD4115_t *obj, uint8_t *data);
static void ReadData(AD4115_t *obj, uint8_t *data);
static void SetChannelConfRegister(AD4115_t *obj, AD4115_ChannelConf_t *cfg);
static void SetSetupRegister(AD4115_t *obj, AD4115_SetupRegister_t *cfg);
static void SetFilterRegister(AD4115_t *obj, AD4115_FilterRegister_t *cfg);
static void SetModeRegister(AD4115_t *obj, AD4115_ModeRegister_t *cfg);
static void SetInterfaceRegister(AD4115_t *obj, AD4115_InterfaceRegister_t *cfg);
static void ConfigureChannels(AD4115_t *obj);

/*==============================================================================
 * Private data
 *============================================================================*/

/*==============================================================================
 * Public functions
 *============================================================================*/

// 0 no error, else error
uint8_t AD4115_Init(AD4115_t *obj)
{
    // Reset and configure AD4115
    Reset(obj);

    Wait100ns(2000);

    // Check connection and stop if not connected
    if(AD4115_CheckId(obj) == 0)
        return 1;

    // channel configuration (select input and setup for each adc channel)
    ConfigureChannels(obj);
    // adc mode and interface mode configuration (adc mode, clock source, enable crc, data and status and more)
    SetModeRegister(obj, &obj->mode_reg);
    // set interface mode register (data stat)
    SetInterfaceRegister(obj, &obj->interface_reg);
    // setup configuration (8 possible adc setups, filter order, output data rate, and more)
    SetSetupRegister(obj, &obj->setup_reg);
    // filter register
    SetFilterRegister(obj, &obj->filter_reg);
    
    return 0;
}

void AD4115_Reset(AD4115_t *obj)
{
    Reset(obj);
}

// 1  same id, 0 wrong
uint8_t AD4115_CheckId(AD4115_t *obj)
{
    uint8_t raw_id[2];
    // Read id and check manufacturer id
    ReadId(obj, raw_id);
    uint16_t id = ((uint16_t)raw_id[0] << 8) | raw_id[1];
    if((id & 0xFFF0) == MANUFACTURER_ID)
        return 1;
    // Wrong id
    return 0;
}

// 0 ok, else error
uint8_t AD4115_UpdateMeasure(AD4115_t *obj, uint8_t *channel, uint32_t *data)
{
    uint8_t buf[4], tmp_ch = 0;

    // Read data and status depending on DATA STAT mode
    if(obj->interface_reg.bits.data_stat == AD4115_DATA_STAT_ENABLED)
    {
        ReadData(obj, buf);
    }
    else
    {
        ReadData(obj, buf);
        ReadStatus(obj, &buf[3]);
    }
    tmp_ch = buf[3] & CHANNEL_MASK;
    
    // Check RDY bit & no error occured
    if((buf[3] & STATUS_RDY_MASK) == 0 &&
       (buf[3] & STATUS_ADC_ERROR_MASK) == 0 &&
       (buf[3] & STATUS_CRC_ERROR_MASK) == 0 &&
       (buf[3] & STATUS_REG_ERROR_MASK) == 0 &&
       (tmp_ch <  obj->channels_count))
    {
        *channel = tmp_ch;
        *data = ((uint32_t)buf[0] << 16) |
                ((uint32_t)buf[1] << 8)  |
                ((uint32_t)buf[2]);
        return 0;
    }

    return 1;
}



/*==============================================================================
 * Private functions
 *============================================================================*/

static void Write(SPI_t *spi, uint8_t* data, const uint16_t size)
{
    if(spi->Write)
        spi->Write(data, size);
}

static void Read(SPI_t *spi, uint8_t* data, const uint16_t size)
{
    if(spi->Read)
        spi->Read(data, size);
}

static void StartTranmission(SPI_t *spi)
{
    __builtin_disable_interrupts();

    if(spi->pinEN.Clear != NULL)
        spi->pinEN.Clear();
    if(spi->pinCS.Clear != NULL) 
        spi->pinCS.Clear();
}

static void EndTramission(SPI_t *spi)
{
    if(spi->pinCS.Set != NULL)
        spi->pinCS.Set();
    if(spi->pinEN.Set != NULL)
        spi->pinEN.Set();
    __builtin_enable_interrupts();
}

static void ReadRegister(SPI_t *spi, uint8_t reg, uint8_t *data, uint8_t len)
{
    // Write the register address
    // bit 7 !WEN must be 0, bit 6 R/W must be 1 to read, bit 5..0 register address
    StartTranmission(spi);
    uint8_t addr = (WEN_READ_CMD_MASK) | (reg & 0x3F);
    Write(spi, &addr, 1);
    // Read data
    Read(spi, data, len);
    EndTramission(spi);
}

static void WriteRegister(SPI_t *spi, uint8_t reg, uint8_t *data, uint8_t len)
{
    StartTranmission(spi);
    uint8_t addr = (reg & 0x3F);
    Write(spi, &addr, 1);
    Write(spi, data, len);
    // Write CRC ?? 
    EndTramission(spi);
}

static void WriteRegister16(SPI_t *spi, uint8_t reg, uint16_t value)
{
    uint8_t tx[2];
    tx[0] = value >> 8;
    tx[1] = value;
    WriteRegister(spi, reg, tx, 2);
}

static void Reset(AD4115_t *obj)
{
    StartTranmission(&obj->spi);
    uint8_t buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Write(&obj->spi, buffer, 8);
    EndTramission(&obj->spi);
}

static void ReadId(AD4115_t *obj, uint8_t *data)
{
    ReadRegister(&obj->spi, ID_REG, data, 2);
}

static void ReadStatus(AD4115_t *obj, uint8_t *data)
{
    ReadRegister(&obj->spi, STATUS_REG, data, 1);
}

static void ReadData(AD4115_t *obj, uint8_t *data)
{
    // 3 data bytes when data stat disabled, appends status register if enabled
    uint8_t len;
    len = obj->interface_reg.bits.data_stat ? 4 : 3;
    ReadRegister(&obj->spi, DATA_REG, data, len);
}

static void SetChannelConfRegister(AD4115_t *obj, AD4115_ChannelConf_t *cfg)
{
    WriteRegister16(&obj->spi, CHANNEL0_REG + cfg->channel, cfg->conf.value);
}

static void SetSetupRegister(AD4115_t *obj, AD4115_SetupRegister_t *cfg)
{
    WriteRegister16(&obj->spi, SETUP_CONFIG0_REG, cfg->value);
}

static void SetFilterRegister(AD4115_t *obj, AD4115_FilterRegister_t *cfg)
{
    WriteRegister16(&obj->spi, FILTER_CONFIG0_REG, cfg->value);
}

static void SetModeRegister(AD4115_t *obj, AD4115_ModeRegister_t *cfg)
{
    WriteRegister16(&obj->spi, ADC_MODE_REG, cfg->value);
}

static void SetInterfaceRegister(AD4115_t *obj, AD4115_InterfaceRegister_t *cfg)
{
    WriteRegister16(&obj->spi, INTERFACE_MODE_REG, cfg->value);
}

static void ConfigureChannels(AD4115_t *obj)
{
    for(uint8_t i = 0; i < obj->channels_count; i++)
    {
        SetChannelConfRegister(obj, &obj->channels_conf[i]);
    }
}