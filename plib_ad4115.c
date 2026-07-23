#include "plib_ad4115.h"

#define WEN_READ_CMD_MASK       0b01000000

// todo: delete after implementing use of conf struct
#define CH_EN_MASK              0x8000

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
static void Reset(AD4115_t *obj);
static void ConfigureChannel(AD4115_t *obj, uint8_t channel, uint8_t setup, uint16_t input);
static void ReadId(AD4115_t *obj, uint8_t *data);
static void ConfigureSetup(AD4115_t *obj, uint16_t conf);
/*==============================================================================
 * Private data
 *============================================================================*/

/*==============================================================================
 * Public functions
 *============================================================================*/

uint8_t AD4115_Init(AD4115_t *obj)
{
    // Reset and configure AD4115
    Reset(obj);

    uint8_t check = AD4115_CheckConnection(obj);

    // channel configuration (select input and setup for each adc channel)
    // todo: modify with modular function (this is just for testing)
    ConfigureChannel(obj, 0, 0, INPUT_VIN0_VINCOM);
    ConfigureChannel(obj, 1, 0, INPUT_VIN0_VINCOM);
    ConfigureChannel(obj, 2, 0, INPUT_VIN0_VINCOM);
    ConfigureChannel(obj, 3, 0, INPUT_VIN0_VINCOM);
    
    // setup configuration (8 possible adc setups, filter order, output data rate, and more)
    ConfigureSetup(obj, 0);     //todo: continue developping this function
    // adc mode and interface mode configuration (adc mode, clock source, enable crc, data and status and more)

    return check;
}

uint8_t AD4115_CheckConnection(AD4115_t *obj)
{
    uint8_t id[2];
    ReadId(obj, id);
    if(id[0] == 0x34)
        return 1;
    return 0;
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

static void Reset(AD4115_t *obj)
{

    StartTranmission(&obj->spi);
    uint8_t buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Write(&obj->spi, buffer, 8);
    EndTramission(&obj->spi);
}

static void ConfigureChannel(AD4115_t *obj, uint8_t channel, uint8_t setup, uint16_t input)
{
    uint16_t ch_cfg = CH_EN_MASK | (((uint16_t)(setup & 0x07)) << 12) | (input & 0x03FF);

    uint8_t data[2] =
    {
        (uint8_t)(ch_cfg >> 8),
        (uint8_t)(ch_cfg)
    };

    WriteRegister(&obj->spi,
                  (uint8_t)(CHANNEL0_REG + channel),
                  data,
                  sizeof(data));
}

static void ReadId(AD4115_t *obj, uint8_t *data)
{
    ReadRegister(&obj->spi, ID_REG, data, 2);
}

static void ConfigureSetup(AD4115_t *obj, uint16_t conf)
{
    /*
    WriteRegister(&obj->spi, SETUP_CONFIG0_REG, )
    if (setup_number > 7) { return; }
	write_register(Register::SETUP0 + setup_number, setup_bits);
    */
}