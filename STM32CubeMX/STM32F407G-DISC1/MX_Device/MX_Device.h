/******************************************************************************
 * File Name   : MX_Device.h
 * Date        : 07/10/2024 08:24:12
 * Description : STM32Cube MX parameter definitions
 * Note        : This file is generated with a generator out of the
 *               STM32CubeMX project and its generated files (DO NOT EDIT!)
 ******************************************************************************/

#ifndef MX_DEVICE_H__
#define MX_DEVICE_H__

/* MX_Device.h version */
#define MX_DEVICE_VERSION                       0x01000000


/*------------------------------ I2C1           -----------------------------*/
#define MX_I2C1                                 1

/* Pins */

/* I2C1_SCL */
#define MX_I2C1_SCL_Pin                         PB6
#define MX_I2C1_SCL_GPIO_Pin                    GPIO_PIN_6
#define MX_I2C1_SCL_GPIOx                       GPIOB
#define MX_I2C1_SCL_GPIO_Mode                   GPIO_MODE_AF_OD
#define MX_I2C1_SCL_GPIO_PuPd                   GPIO_PULLUP
#define MX_I2C1_SCL_GPIO_Speed                  GPIO_SPEED_FREQ_LOW
#define MX_I2C1_SCL_GPIO_AF                     GPIO_AF4_I2C1

/* I2C1_SDA */
#define MX_I2C1_SDA_Pin                         PB9
#define MX_I2C1_SDA_GPIO_Pin                    GPIO_PIN_9
#define MX_I2C1_SDA_GPIOx                       GPIOB
#define MX_I2C1_SDA_GPIO_Mode                   GPIO_MODE_AF_OD
#define MX_I2C1_SDA_GPIO_PuPd                   GPIO_PULLUP
#define MX_I2C1_SDA_GPIO_Speed                  GPIO_SPEED_FREQ_LOW
#define MX_I2C1_SDA_GPIO_AF                     GPIO_AF4_I2C1

/*------------------------------ SPI1           -----------------------------*/
#define MX_SPI1                                 1

/* Pins */

/* SPI1_MISO */
#define MX_SPI1_MISO_Pin                        PA6
#define MX_SPI1_MISO_GPIO_Pin                   GPIO_PIN_6
#define MX_SPI1_MISO_GPIOx                      GPIOA
#define MX_SPI1_MISO_GPIO_Mode                  GPIO_MODE_AF_PP
#define MX_SPI1_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI1_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_LOW
#define MX_SPI1_MISO_GPIO_AF                    GPIO_AF5_SPI1

/* SPI1_MOSI */
#define MX_SPI1_MOSI_Pin                        PA7
#define MX_SPI1_MOSI_GPIO_Pin                   GPIO_PIN_7
#define MX_SPI1_MOSI_GPIOx                      GPIOA
#define MX_SPI1_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP
#define MX_SPI1_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI1_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_LOW
#define MX_SPI1_MOSI_GPIO_AF                    GPIO_AF5_SPI1

/* SPI1_SCK */
#define MX_SPI1_SCK_Pin                         PA5
#define MX_SPI1_SCK_GPIO_Pin                    GPIO_PIN_5
#define MX_SPI1_SCK_GPIOx                       GPIOA
#define MX_SPI1_SCK_GPIO_Mode                   GPIO_MODE_AF_PP
#define MX_SPI1_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI1_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_LOW
#define MX_SPI1_SCK_GPIO_AF                     GPIO_AF5_SPI1

/*------------------------------ USART2         -----------------------------*/
#define MX_USART2                               1

/* Virtual mode */
#define MX_USART2_VM                            VM_ASYNC
#define MX_USART2_VM_ASYNC                      1

/* Pins */

/* USART2_RX */
#define MX_USART2_RX_Pin                        PA3
#define MX_USART2_RX_GPIO_Pin                   GPIO_PIN_3
#define MX_USART2_RX_GPIOx                      GPIOA
#define MX_USART2_RX_GPIO_Mode                  GPIO_MODE_AF_PP
#define MX_USART2_RX_GPIO_PuPd                  GPIO_NOPULL
#define MX_USART2_RX_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USART2_RX_GPIO_AF                    GPIO_AF7_USART2

/* USART2_TX */
#define MX_USART2_TX_Pin                        PA2
#define MX_USART2_TX_GPIO_Pin                   GPIO_PIN_2
#define MX_USART2_TX_GPIOx                      GPIOA
#define MX_USART2_TX_GPIO_Mode                  GPIO_MODE_AF_PP
#define MX_USART2_TX_GPIO_PuPd                  GPIO_NOPULL
#define MX_USART2_TX_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USART2_TX_GPIO_AF                    GPIO_AF7_USART2

/*------------------------------ USB_OTG_FS     -----------------------------*/
#define MX_USB_OTG_FS                           1

/* Handle */
#define MX_USB_OTG_FS_HANDLE                    hhcd_USB_OTG_FS

/* Virtual mode */
#define MX_USB_OTG_FS_VM                        Host_Only
#define MX_USB_OTG_FS_Host_Only                 1

/* Pins */

/* USB_OTG_FS_DM */
#define MX_USB_OTG_FS_DM_Pin                    PA11
#define MX_USB_OTG_FS_DM_GPIO_Pin               GPIO_PIN_11
#define MX_USB_OTG_FS_DM_GPIOx                  GPIOA
#define MX_USB_OTG_FS_DM_GPIO_Mode              GPIO_MODE_AF_PP
#define MX_USB_OTG_FS_DM_GPIO_PuPd              GPIO_NOPULL
#define MX_USB_OTG_FS_DM_GPIO_Speed             GPIO_SPEED_FREQ_LOW
#define MX_USB_OTG_FS_DM_GPIO_AF                GPIO_AF10_OTG_FS

/* USB_OTG_FS_DP */
#define MX_USB_OTG_FS_DP_Pin                    PA12
#define MX_USB_OTG_FS_DP_GPIO_Pin               GPIO_PIN_12
#define MX_USB_OTG_FS_DP_GPIOx                  GPIOA
#define MX_USB_OTG_FS_DP_GPIO_Mode              GPIO_MODE_AF_PP
#define MX_USB_OTG_FS_DP_GPIO_PuPd              GPIO_NOPULL
#define MX_USB_OTG_FS_DP_GPIO_Speed             GPIO_SPEED_FREQ_LOW
#define MX_USB_OTG_FS_DP_GPIO_AF                GPIO_AF10_OTG_FS

/* USB_OTG_FS_ID */
#define MX_USB_OTG_FS_ID_Pin                    PA10
#define MX_USB_OTG_FS_ID_GPIO_Pin               GPIO_PIN_10
#define MX_USB_OTG_FS_ID_GPIOx                  GPIOA
#define MX_USB_OTG_FS_ID_GPIO_Mode              GPIO_MODE_AF_PP
#define MX_USB_OTG_FS_ID_GPIO_PuPd              GPIO_NOPULL
#define MX_USB_OTG_FS_ID_GPIO_Speed             GPIO_SPEED_FREQ_LOW
#define MX_USB_OTG_FS_ID_GPIO_AF                GPIO_AF10_OTG_FS

/* USB_OTG_FS_VBUS */
#define MX_USB_OTG_FS_VBUS_Pin                  PA9
#define MX_USB_OTG_FS_VBUS_GPIO_Pin             GPIO_PIN_9
#define MX_USB_OTG_FS_VBUS_GPIOx                GPIOA
#define MX_USB_OTG_FS_VBUS_GPIO_Mode            GPIO_MODE_INPUT
#define MX_USB_OTG_FS_VBUS_GPIO_PuPd            GPIO_NOPULL

#endif  /* MX_DEVICE_H__ */
