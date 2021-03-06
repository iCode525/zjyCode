/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*! @name PORTB0 (number 53), ARM1_USER_LED1
  @{ */
#define BOARD_INITPINS_ARM1_USER_LED1_GPIO GPIOB /*!<@brief GPIO device name: GPIOB */
#define BOARD_INITPINS_ARM1_USER_LED1_PORT PORTB /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_ARM1_USER_LED1_PIN 0U     /*!<@brief PORTB pin index: 0 */
                                                 /* @} */

/*! @name PORTC16 (number 90), UART3_RX
  @{ */
#define BOARD_INITPINS_UART3_RX_PORT PORTC /*!<@brief PORT device name: PORTC */
#define BOARD_INITPINS_UART3_RX_PIN 16U    /*!<@brief PORTC pin index: 16 */
                                           /* @} */

/*! @name PORTC17 (number 91), UART3_TX
  @{ */
#define BOARD_INITPINS_UART3_TX_PORT PORTC /*!<@brief PORT device name: PORTC */
#define BOARD_INITPINS_UART3_TX_PIN 17U    /*!<@brief PORTC pin index: 17 */
                                           /* @} */

/*! @name PORTC15 (number 87), ARM1_UART3_DRE
  @{ */
#define BOARD_INITPINS_ARM1_UART3_DRE_GPIO GPIOC /*!<@brief GPIO device name: GPIOC */
#define BOARD_INITPINS_ARM1_UART3_DRE_PORT PORTC /*!<@brief PORT device name: PORTC */
#define BOARD_INITPINS_ARM1_UART3_DRE_PIN 15U    /*!<@brief PORTC pin index: 15 */
                                                 /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
