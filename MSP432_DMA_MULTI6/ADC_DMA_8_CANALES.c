#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include "preCompile.h"

const Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // 24Mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          //
        23,                                     // 23+1 period,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,      // Duty Cycle Defined in CCR1
        TIMER_A_OUTPUTMODE_SET_RESET,
        19                                      // Set on 19 and reset on 0
};

int main(void)
{
    volatile uint16_t ii,jj;
    /* Halting WDT  */
    MAP_WDT_A_holdTimer();

    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 1);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 1);


    MAP_CS_setDCOFrequency(48000000);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    P2->DIR |= (BIT4);
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;

    /* Configuring P1.0 as output for debug */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /*
     * Non-conventional usage of P1.2 and P1.3 as 'internal' interrupt
     * triggered from DMA.
     */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, (GPIO_PIN2 + GPIO_PIN3));
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, (GPIO_PIN2 + GPIO_PIN3));
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1,(GPIO_PIN2 + GPIO_PIN3),GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,(GPIO_PIN2 + GPIO_PIN3));
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1,(GPIO_PIN2 + GPIO_PIN3));

    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configure P2.0, P2.1, and P2.2 for debug*/
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

    /* Configuring GPIOs for Analog In P4.5,4.6,4.7 */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);
    /*
     * Initializing ADC (ADCOSC/1/1)
     * 25Mhz
     */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_1, ADC_DIVIDER_1,0);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM4 (A4 - A6)  with repeat)
     * with internal 2.5v reference */
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM7, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A0, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM1,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A1, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM2,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A6, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM3,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A8, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM4,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM5,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM6,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);
    MAP_ADC14_configureConversionMemory(ADC_MEM7,ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);
    /*
     * Configuring the sample trigger to be sourced from Timer_A0 CCR1
     * and setting it to automatic iteration after it is triggered
     */
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);
    /*
     * Disable sample timer (default), use pulse mode.
     */
    MAP_ADC14_disableSampleTimer();
    MAP_ADC14_enableConversion();

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);

     /* Disabling channel attributes */
    MAP_DMA_disableChannelAttribute(DMA_CH7_ADC14,
                                 UDMA_ATTR_USEBURST |
                                 UDMA_ATTR_HIGH_PRIORITY |
                                 UDMA_ATTR_REQMASK);

    MAP_DMA_setChannelScatterGather(DMA_CH7_ADC14,ADC_TASKS,(void*)&AdcDmaSeq_02[0],true);
    pongTask = MSP_EXP432P401RLP_DMAControlTable[7];

    MAP_DMA_setChannelScatterGather(DMA_CH7_ADC14,ADC_TASKS,(void*)&AdcDmaSeq_01[0],true);
    pingTask = MSP_EXP432P401RLP_DMAControlTable[7];

    MAP_DMA_assignChannel(DMA_CH7_ADC14);
    MAP_DMA_enableChannel(7);

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);


    while(1)
    {
        MAP_PCM_gotoLPM0();
        /*
         Hacer procesamiento you want....
         */
    }
}
/*
#if     __TI_COMPILER_VERSION__ >= 15009000
__attribute__((ramfunc))
#endif
*/
void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if(status == GPIO_PIN2)
    {
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN2);
        dataSet0Ready = true;
    }
    if(status == GPIO_PIN3)
    {
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
        dataSet0Ready = false;
    }
}
