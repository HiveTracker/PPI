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


#ifndef PPI_h
#define PPI_h

#include "Arduino.h"
#include "nRF_SDK/nrf_ppi.h"
#include "nRF_SDK/nrf_timer.h"
#include "nRF_SDK/nrf_gpiote.h"


//enumerate events and tasks
typedef enum{
    PIN_HIGH = 0,
    PIN_LOW = 1,
    PIN_CHANGE = 2,
}event_type;

typedef enum{
    TIMER_START = 0,
    TIMER_STOP = 1,
    TIMER_CLEAR = 2,
    TIMER_CAPTURE = 3,
}task_type;

class PPIClass{

    public:

        PPIClass();

        /**
         * @brief
         * Name:
         *            setShortcut
         * Description:
         *            The function allows to make an action when a given event occurs without
         *            involving MCU.
         * Arguments:
         *            -event: event to which bind the related action.
         *            -task: action to be taken when the event occurs.
         */
        int setShortcut(event_type event, task_type task);


        /**
         * @brief
         * Name:
         *            setInputPin
         * Description:
         *            Select the pin that will be taken into account in the next
         *            event -> action interaction.
         * Argument:
         *            pin: pin's number.
         */
        void setInputPin(uint32_t pin);


        /**
         * @brief
         * Name:
         *            setTimer
         * Description:
         *            Set timer number for the object
         * Argument:
         *            _timerNo: the number of the timer
         */
        void setTimer(int _timerNo);


        /**
         * @brief
         * Name:
         *            resetChannels
         * Description:
         *            Reset PPI channels
         */
        inline void resetChannels(){
            nrf_ppi_channel_disable_all();
            first_free_channel = 0;
        }

    private:

        uint8_t timerNo;

        uint8_t first_free_channel;
        uint8_t gpiote_config_index;
        uint8_t event_index;
        uint8_t task_index;

        uint32_t inputPin;

        void configureTimer();
        void configureGPIOEvent(event_type event);

};

extern PPIClass PPI;


#endif //PPI_h

