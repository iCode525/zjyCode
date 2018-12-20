/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.1
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 4.0.1
pin_labels:
- {pin_num: '53', pin_signal: ADC0_SE8/ADC1_SE8/PTB0/LLWU_P5/I2C0_SCL/FTM1_CH0/RMII0_MDIO/MII0_MDIO/FTM1_QD_PHA, label: ARM1_USER_LED1, identifier: ARM1_USER_LED1}
- {pin_num: '91', pin_signal: PTC17/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b, label: UART3_TX, identifier: UART3_TX}
- {pin_num: '90', pin_signal: PTC16/UART3_RX/ENET0_1588_TMR0/FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b, label: UART3_RX, identifier: UART3_RX}
- {pin_num: '87', pin_signal: PTC15/UART4_TX/FB_AD24, label: ARM1_UART3_DRE, identifier: ARM1_UART3_DRE}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '53', peripheral: GPIOB, signal: 'GPIO, 0', pin_signal: ADC0_SE8/ADC1_SE8/PTB0/LLWU_P5/I2C0_SCL/FTM1_CH0/RMII0_MDIO/MII0_MDIO/FTM1_QD_PHA, direction: OUTPUT,
    gpio_init_state: 'false', pull_select: up}
  - {pin_num: '90', peripheral: UART3, signal: RX, pin_signal: PTC16/UART3_RX/ENET0_1588_TMR0/FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b, pull_select: up, pull_enable: enable}
  - {pin_num: '91', peripheral: UART3, signal: TX, pin_signal: PTC17/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b, direction: OUTPUT, pull_select: up,
    pull_enable: enable}
  - {pin_num: '87', peripheral: GPIOC, signal: 'GPIO, 15', pin_signal: PTC15/UART4_TX/FB_AD24, direction: OUTPUT, gpio_init_state: 'false', pull_select: down}
  - {pin_num: '55', peripheral: I2C0, signal: SCL, pin_signal: ADC0_SE12/PTB2/I2C0_SCL/UART0_RTS_b/ENET0_1588_TMR0/FTM0_FLT3, pull_select: up, pull_enable: enable}
  - {pin_num: '56', peripheral: I2C0, signal: SDA, pin_signal: ADC0_SE13/PTB3/I2C0_SDA/UART0_CTS_b/UART0_COL_b/ENET0_1588_TMR1/FTM0_FLT0, pull_select: up, pull_enable: enable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    gpio_pin_config_t ARM1_USER_LED1_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTB0 (pin 53)  */
    GPIO_PinInit(BOARD_INITPINS_ARM1_USER_LED1_GPIO, BOARD_INITPINS_ARM1_USER_LED1_PIN, &ARM1_USER_LED1_config);

    gpio_pin_config_t ARM1_UART3_DRE_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTC15 (pin 87)  */
    GPIO_PinInit(BOARD_INITPINS_ARM1_UART3_DRE_GPIO, BOARD_INITPINS_ARM1_UART3_DRE_PIN, &ARM1_UART3_DRE_config);

    /* PORTB0 (pin 53) is configured as PTB0 */
    PORT_SetPinMux(BOARD_INITPINS_ARM1_USER_LED1_PORT, BOARD_INITPINS_ARM1_USER_LED1_PIN, kPORT_MuxAsGpio);

    PORTB->PCR[0] = ((PORTB->PCR[0] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | PORT_PCR_PS(kPORT_PullUp));

    /* PORTB2 (pin 55) is configured as I2C0_SCL */
    PORT_SetPinMux(PORTB, 2U, kPORT_MuxAlt2);

    PORTB->PCR[2] = ((PORTB->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTB3 (pin 56) is configured as I2C0_SDA */
    PORT_SetPinMux(PORTB, 3U, kPORT_MuxAlt2);

    PORTB->PCR[3] = ((PORTB->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTC15 (pin 87) is configured as PTC15 */
    PORT_SetPinMux(BOARD_INITPINS_ARM1_UART3_DRE_PORT, BOARD_INITPINS_ARM1_UART3_DRE_PIN, kPORT_MuxAsGpio);

    PORTC->PCR[15] = ((PORTC->PCR[15] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                      /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
                       * corresponding PE field is set. */
                      | PORT_PCR_PS(kPORT_PullDown));

    /* PORTC16 (pin 90) is configured as UART3_RX */
    PORT_SetPinMux(BOARD_INITPINS_UART3_RX_PORT, BOARD_INITPINS_UART3_RX_PIN, kPORT_MuxAlt3);

    PORTC->PCR[16] = ((PORTC->PCR[16] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                      /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                       * corresponding PE field is set. */
                      | (uint32_t)(kPORT_PullUp));

    /* PORTC17 (pin 91) is configured as UART3_TX */
    PORT_SetPinMux(BOARD_INITPINS_UART3_TX_PORT, BOARD_INITPINS_UART3_TX_PIN, kPORT_MuxAlt3);

    PORTC->PCR[17] = ((PORTC->PCR[17] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                      /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                       * corresponding PE field is set. */
                      | (uint32_t)(kPORT_PullUp));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
