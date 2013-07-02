#define usb_port                "/dev/ttyS0"

#ifndef SUBCONTROLLER_H
#define SUBCONTROLLER_H

#include <iostream>
#include <SerialStream.h>
#include <stdlib.h>
#include <pthread.h>

namespace {

using namespace LibSerial;
using namespace std;

// tipe data kelas
SerialStream porting;
pthread_mutex_t mutex;

void port_init()
{
    pthread_mutex_init(&mutex, NULL);

    //buka port usb
    porting.Open(usb_port);
    if(!porting.good())
    {
        cerr << "Error: Could not open serial device !"
           << endl;
        exit(1);
    }
    //set baudrate
    porting.SetBaudRate(SerialStreamBuf::BAUD_57600);
    if ( ! porting.good() )
    {
        cerr << "Error: Could not set baudrate" <<
                endl ;
        exit(1) ;
    }
    //set the number of data bits
    porting.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    if ( ! porting.good() )
    {
        cerr << "Error: Could not set the character size." <<
                endl ;
        exit(1) ;
    }
    //disable parity
    porting.SetParity(SerialStreamBuf::PARITY_NONE);
    if ( ! porting.good() )
    {
        cerr << "Error: Could not set the parity" <<
                endl ;
        exit(1) ;
    }
    //stop bit = 1
    porting.SetNumOfStopBits(1);
    if ( ! porting.good() )
    {
        cerr << "Error: Could not set the stop bit" <<
                endl ;
        exit(1) ;
    }
    //set flow control = none
    porting.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    if ( ! porting.good() )
    {
        cerr << "Error: Could not set the flow control." <<
                endl;
        exit(1) ;
    }
}

char first_head = 0xFF;
char sec_head = 0x55;

void port_write(unsigned int data)
{
    pthread_mutex_lock(&mutex);

    char data_msb = (data>>8);
    char data_lsb = data & ~(0xFF<<8);
    char inv_msb = ~data_msb;
    char inv_lsb = ~data_lsb;

    porting.write(&first_head,1);
    porting.write(&sec_head,1);
    porting.write(&data_lsb,1);
    porting.write(&inv_lsb,1);
    porting.write(&data_msb,1);
    porting.write(&inv_msb,1);

    pthread_mutex_unlock(&mutex);
}

}

#endif // SUBCONTROLLER_H
