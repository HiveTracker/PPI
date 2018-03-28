/*
   PPI class for nRF52.
   Written by Chiara Ruggeri (chiara@arduino.org)

   Copyright (c) 2016 Arduino.  All right reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   Enjoy!
 */


#include "PPI.h"
#include "nRF_SDK/nrf_timer.h"

nrf_ppi_channel_t channels[20] = {NRF_PPI_CHANNEL0, NRF_PPI_CHANNEL1,
    NRF_PPI_CHANNEL2, NRF_PPI_CHANNEL3, NRF_PPI_CHANNEL4, NRF_PPI_CHANNEL5,
    NRF_PPI_CHANNEL6, NRF_PPI_CHANNEL7, NRF_PPI_CHANNEL8, NRF_PPI_CHANNEL9,
    NRF_PPI_CHANNEL10, NRF_PPI_CHANNEL11, NRF_PPI_CHANNEL12, NRF_PPI_CHANNEL13,
    NRF_PPI_CHANNEL14, NRF_PPI_CHANNEL15, NRF_PPI_CHANNEL16, NRF_PPI_CHANNEL17,
    NRF_PPI_CHANNEL18, NRF_PPI_CHANNEL19};
nrf_gpiote_tasks_t gpio_taskNo[] = {NRF_GPIOTE_TASKS_OUT_0,
    NRF_GPIOTE_TASKS_OUT_1, NRF_GPIOTE_TASKS_OUT_2, NRF_GPIOTE_TASKS_OUT_3,
    NRF_GPIOTE_TASKS_OUT_4, NRF_GPIOTE_TASKS_OUT_5, NRF_GPIOTE_TASKS_OUT_6,
    NRF_GPIOTE_TASKS_OUT_7};
nrf_gpiote_events_t gpio_eventNo[] = {NRF_GPIOTE_EVENTS_IN_0,
    NRF_GPIOTE_EVENTS_IN_1, NRF_GPIOTE_EVENTS_IN_2, NRF_GPIOTE_EVENTS_IN_3,
    NRF_GPIOTE_EVENTS_IN_4, NRF_GPIOTE_EVENTS_IN_5, NRF_GPIOTE_EVENTS_IN_6,
    NRF_GPIOTE_EVENTS_IN_7};
NRF_TIMER_Type * timers[] = { NRF_TIMER0, NRF_TIMER1, NRF_TIMER2,
                              NRF_TIMER3, NRF_TIMER4 };
int timerNo = 1; // 0 is used by soft device

uint8_t first_free_channel=0;
uint8_t gpiote_config_index=0;
uint8_t event_index;
uint8_t task_index;

uint32_t milliSec=1000;
uint32_t outputPin=LED_BUILTIN;
uint32_t inputPin=0;                                                        // TODO


//public functions

int PPIClass::setShortcut(event_type event, task_type task){
    //check if there is still a free channel
    if(first_free_channel==20)
        return 0;

    switch(event & 0xF0){

        case 0x00:
            //event is related to timer
            configureTimer();
            switch(task & 0xF0){
                case 0x10: //task is related to GPIO
                    configureGPIOTask(task);
                    //enable PPI peripheral
                    nrf_ppi_channel_endpoint_setup(channels[first_free_channel],
                            (uint32_t)nrf_timer_event_address_get(timers[timerNo], NRF_TIMER_EVENT_COMPARE0),
                            nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
                    nrf_ppi_channel_enable(channels[first_free_channel]);
                    break;
                case 0x30: //task is related to NFC
                    if((task & 0x0F)==0) //start sensing
                        nrf_ppi_channel_endpoint_setup(channels[first_free_channel],
                                (uint32_t)nrf_timer_event_address_get(timers[timerNo], NRF_TIMER_EVENT_COMPARE0),
                                0x40005008);
                    else    //stop sensing
                        nrf_ppi_channel_endpoint_setup(channels[first_free_channel],
                                (uint32_t)nrf_timer_event_address_get(timers[timerNo], NRF_TIMER_EVENT_COMPARE0),
                                0x40005004);

                    nrf_ppi_channel_enable(channels[first_free_channel]);
                    break;
                default: //task not recognized
                    return 0;
                    break;
            }
            //start the timer
            nrf_timer_task_trigger(timers[timerNo], NRF_TIMER_TASK_START);
            break;

        case 0x10:
            //event is related to gpio
            configureGPIOEvent(event);
            switch(task & 0xF0){
                case 0x00: //task is related to timer
                    nrf_timer_task_t nrf_task;

                    switch(task){                               // TODO: move to configureTimer()
                        case TIMER_CLEAR:
                            nrf_task = NRF_TIMER_TASK_CLEAR;
                            break;
                        case TIMER_START:
                            nrf_task = NRF_TIMER_TASK_START;
                            configureTimer(TIMER_START);
                            break;
                        case TIMER_CAPTURE:
                            nrf_task = NRF_TIMER_TASK_CAPTURE0; // TODO: make it generic
                            configureTimer(TIMER_CAPTURE);
                            break;
                        case TIMER_STOP:
                            nrf_task = NRF_TIMER_TASK_STOP;
                            break;
                        default: //task not recognized
                            return 0;
                    }

                    nrf_ppi_channel_endpoint_setup(channels[first_free_channel],
                            (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]),
                            (uint32_t)nrf_timer_task_address_get(timers[timerNo], nrf_task));
                    nrf_ppi_channel_enable(channels[first_free_channel]);
                    break;
                case 0x10: //task is related to GPIO
                    configureGPIOTask(task);
                    //enable PPI peripheral
                    nrf_ppi_channel_endpoint_setup(channels[first_free_channel],
                            (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]),
                            nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
                    nrf_ppi_channel_enable(channels[first_free_channel]);
                    break;
                default: //task not recognized
                    return 0;
                    break;
            }
            //enable sensing
            nrf_gpiote_event_enable(event_index);
            break;

        default:
            return 0;    //event not recognized
            break;
    }
    first_free_channel++;
    return 1;
}


void PPIClass::setInputPin(uint32_t pin){
    inputPin=pin;
}

void PPIClass::setOutputPin(uint32_t pin){
    outputPin=pin;
}

void PPIClass::setTimerInterval(uint32_t msec){
    milliSec=msec;
}

//private function

//functions to configure events
void PPIClass::configureTimer(task_type task){                                  // TODO: CHECK WITH INTERRUPT LIB!
    nrf_timer_mode_set(timers[timerNo], NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(timers[timerNo], NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_frequency_set(timers[timerNo], NRF_TIMER_FREQ_16MHz);

    if(task == TIMER_DEFAULT){ // default value is for the
        nrf_timer_shorts_enable(timers[timerNo], NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);
        //Clear the timer when it finishes to count
        uint32_t ticks=nrf_timer_ms_to_ticks(milliSec, NRF_TIMER_FREQ_16MHz);
        nrf_timer_cc_write(timers[timerNo], NRF_TIMER_CC_CHANNEL0, ticks);      // TODO: ALLOW OTHER CHANNELS!
    }
}

void PPIClass::configureGPIOEvent(event_type event){
    nrf_gpiote_polarity_t mode;
    switch(event&0x0F){
        case 0:
            mode=NRF_GPIOTE_POLARITY_LOTOHI;
            break;
        case 1:
            mode=NRF_GPIOTE_POLARITY_HITOLO;
            break;
        default: // case 2:                                                                        // TODO?
            mode=NRF_GPIOTE_POLARITY_TOGGLE;
            break;
    }
    //if user use more than the allowed number of gpio, overwrite previous configuration
    if(gpiote_config_index==8)
        gpiote_config_index=0;
    nrf_gpiote_event_configure(gpiote_config_index, inputPin, mode);

    event_index=gpiote_config_index;
    gpiote_config_index++;
}

//function to configure tasks
void PPIClass::configureGPIOTask(task_type task){
    //if user use more than the allowed number of gpio, overwrite previous configuration
    if(gpiote_config_index==8)
        gpiote_config_index=0;
    nrf_gpiote_polarity_t job;
    nrf_gpiote_outinit_t initValue;
    switch(task&0x0F){
        case 0:
            job=NRF_GPIOTE_POLARITY_LOTOHI;
            initValue=NRF_GPIOTE_INITIAL_VALUE_LOW;
            break;
        case 1:
            job=NRF_GPIOTE_POLARITY_HITOLO;
            initValue=NRF_GPIOTE_INITIAL_VALUE_HIGH;
            break;
        default: // case 2:                                                                        // TODO?
            job=NRF_GPIOTE_POLARITY_TOGGLE;
            initValue=NRF_GPIOTE_INITIAL_VALUE_LOW;
            break;
    }

    nrf_gpiote_task_configure(gpiote_config_index, outputPin, job, initValue);
    nrf_gpiote_task_enable(gpiote_config_index);

    task_index=gpiote_config_index;
    gpiote_config_index++;
}

void PPIClass::setTimer(int _timerNo) {
    timerNo = _timerNo;
}

PPIClass PPI;
