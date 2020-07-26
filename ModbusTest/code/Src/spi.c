// Includes --------------------------------------------------------------------
#include "spi.h"
#include "gpio.h"

// Private Typedef -------------------------------------------------------------
typedef enum
{
  SPI_NOINIT = 0,  // SPI не инициализирован
  SPI_READY,       // SPI готов к обмену
  SPI_BUSY         // SPI занят
} SpiStatus_t;

// Private Macro ---------------------------------------------------------------
#define SPI_SCLK_PIN SPI_SCLK_Pin
#define SPI_MISO_PIN SPI_MISO_Pin
#define SPI_MOSI_PIN SPI_MOSI_Pin
#define SPI_PORT     SPI_GPIO_Port

#define SPI_CS1_PIN  HEAT_CS_Pin
#define SPI_CS1_PORT HEAT_CS_GPIO_Port
#define SPI_CS2_PIN  FIRE_CS_Pin
#define SPI_CS2_PORT FIRE_CS_GPIO_Port

#define SPI SPI1

#define SPI_BAUD_RATE 115200
// Private Variables -----------------------------------------------------------
static SpiStatus_t spiStatus = SPI_NOINIT;  // Переменная статуса модуля SPI
static uint16_t data = {0};
// Private Function prototypes -------------------------------------------------

// Functions -------------------------------------------------------------------
void SpiInit()
{
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPBEN;

  NVIC_SetPriority(SPI1_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(SPI1_IRQn);

  GpioSetMode(SPI_PORT,
              SPI_SCLK_PIN | SPI_MOSI_PIN,
              GPIO_MODE_ALTERNATE_PUSHPULL,
              GPIO_SPEED_HIGH);
  GpioSetMode(SPI_PORT, SPI_MISO_PIN, GPIO_MODE_FLOATING, GPIO_SPEED_INPUT);
  GpioSetMode(SPI_CS1_PORT,
              SPI_CS1_PIN,
              GPIO_MODE_OUTPUT_PUSHPULL,
              GPIO_SPEED_LOW);
  GpioSetMode(SPI_CS2_PORT,
              SPI_CS2_PIN,
              GPIO_MODE_OUTPUT_PUSHPULL,
              GPIO_SPEED_LOW);
  GpioOutSet(SPI_CS1_PORT, SPI_CS1_PIN);
  GpioOutSet(SPI_CS2_PORT, SPI_CS2_PIN);
  // Enable the remapping of SPI1 alternate function NSS, SCK, MISO and MOSI.
  AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP | AFIO_MAPR_SWJ_CFG;

  SPI->CR1 |= SPI_CR1_DFF | SPI_CR1_BR | SPI_CR1_MSTR;
  SPI->CR1 |= SPI_CR1_SPE;
  spiStatus = SPI_READY;
}

uint16_t SpiCs1TxRx(uint16_t data)
{
  while (SPI->SR & SPI_SR_BSY)
  {
    ;
  }
  // Сброс вывода СS (выбор ведомого устройства)
  GpioOutReset(SPI_CS1_PORT, SPI_CS1_PIN);
  SPI->DR = data;
  while (!(SPI->SR & SPI_SR_RXNE))
  {
    ;
  }
  uint16_t outData = SPI->DR;
  while (SPI->SR & SPI_SR_BSY)
  {
    ;
  }
  GpioOutSet(SPI_CS1_PORT, SPI_CS1_PIN);
  return outData;
}

uint16_t SpiCs2TxRx(uint16_t data)
{
  while (SPI->SR & SPI_SR_BSY)
  {
    ;
  }
  // Сброс вывода СS (выбор ведомого устройства)
  GpioOutReset(SPI_CS2_PORT, SPI_CS2_PIN);
  SPI->DR = data;
  while (!(SPI->SR & SPI_SR_RXNE))
  {
    ;
  }
  uint16_t outData = SPI->DR;
  while (SPI->SR & SPI_SR_BSY)
  {
    ;
  }
  GpioOutSet(SPI_CS2_PORT, SPI_CS2_PIN);
  return outData;
}
