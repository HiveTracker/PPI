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

const nrf_ppi_channel_t channels[20] = {NRF_PPI_CHANNEL0, NRF_PPI_CHANNEL1,
    NRF_PPI_CHANNEL2, NRF_PPI_CHANNEL3, NRF_PPI_CHANNEL4, NRF_PPI_CHANNEL5,
    NRF_PPI_CHANNEL6, NRF_PPI_CHANNEL7, NRF_PPI_CHANNEL8, NRF_PPI_CHANNEL9,
    NRF_PPI_CHANNEL10, NRF_PPI_CHANNEL11, NRF_PPI_CHANNEL12, NRF_PPI_CHANNEL13,
    NRF_PPI_CHANNEL14, NRF_PPI_CHANNEL15, NRF_PPI_CHANNEL16, NRF_PPI_CHANNEL17,
    NRF_PPI_CHANNEL18, NRF_PPI_CHANNEL19};

const nrf_gpiote_tasks_t gpio_taskNo[] = {NRF_GPIOTE_TASKS_OUT_0,
    NRF_GPIOTE_TASKS_OUT_1, NRF_GPIOTE_TASKS_OUT_2, NRF_GPIOTE_TASKS_OUT_3,
    NRF_GPIOTE_TASKS_OUT_4, NRF_GPIOTE_TASKS_OUT_5, NRF_GPIOTE_TASKS_OUT_6,
    NRF_GPIOTE_TASKS_OUT_7};

const nrf_gpiote_events_t gpio_eventNo[] = {NRF_GPIOTE_EVENTS_IN_0,
    NRF_GPIOTE_EVENTS_IN_1, NRF_GPIOTE_EVENTS_IN_2, NRF_GPIOTE_EVENTS_IN_3,
    NRF_GPIOTE_EVENTS_IN_4, NRF_GPIOTE_EVENTS_IN_5, NRF_GPIOTE_EVENTS_IN_6,
    NRF_GPIOTE_EVENTS_IN_7};

NRF_TIMER_Type* const timers[] = { NRF_TIMER0, NRF_TIMER1, NRF_TIMER2,
                                   NRF_TIMER3, NRF_TIMER4 };

const nrf_gpiote_polarity_t gpio_polarity[] = { NRF_GPIOTE_POLARITY_LOTOHI,
    NRF_GPIOTE_POLARITY_HITOLO, NRF_GPIOTE_POLARITY_TOGGLE };


//public functions

PPIClass::PPIClass() {
    timerNo = 1; // 0 is used by soft device
}


int PPIClass::setShortcut(event_type event, task_type task){
    //check if there is still a free channel
    if(first_free_channel==20)
        return 0;

    configureGPIOEvent(event);
    configureTimer();

    nrf_timer_task_t nrf_task;

    switch(task){
        case TIMER_CLEAR:
            nrf_task = NRF_TIMER_TASK_CLEAR;
            break;
        case TIMER_START:
            nrf_task = NRF_TIMER_TASK_START;
            break;
        case TIMER_CAPTURE:
            nrf_task = NRF_TIMER_TASK_CAPTURE0; // TODO: MAKE IT GENERIC
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

    //enable sensing
    nrf_gpiote_event_enable(event_index);

    first_free_channel++;
    return 1;
}


void PPIClass::setInputPin(uint32_t pin){
    inputPin=pin;
}

void PPIClass::setTimer(int _timerNo) {
    timerNo = _timerNo;
}


//private function

//functions to configure events
void PPIClass::configureTimer(){                                  // TODO: CHECK WITH INTERRUPT LIB!
    nrf_timer_mode_set(timers[timerNo], NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(timers[timerNo], NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_frequency_set(timers[timerNo], NRF_TIMER_FREQ_16MHz);
}

void PPIClass::configureGPIOEvent(event_type event){
    //if user use more than the allowed number of gpio, overwrite previous configuration
    if(gpiote_config_index==8)
        gpiote_config_index=0;
    nrf_gpiote_event_configure(gpiote_config_index, inputPin, gpio_polarity[event]);

    event_index=gpiote_config_index;
    gpiote_config_index++;
}


PPIClass PPI;

