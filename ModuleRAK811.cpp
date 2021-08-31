//**************************************************************************************************
// @Module        RAK811
// @Filename      ModuleRAK811.c
//--------------------------------------------------------------------------------------------------
// @Platform      arduino atmega2560
//--------------------------------------------------------------------------------------------------
// @Compatible    atmega2560
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the RAK811 functionality.
//                
//
//                Abbreviations:
//                  None.
//                  
//
//                Global (public) functions:
//                  
//                  
//
//                Local (private) functions:
//                  
//                  
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          30.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// Native header
#include "ModuleRAK811.h"

#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

//**************************************************************************************************
// Verification of the imported configuration parameters
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

#define RAK811_SIZE_BUF                 (512)


//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static char buf[RAK811_SIZE_BUF];


//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

// None.


//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************

//**************************************************************************************************
// @Function      RAK811_init()
//--------------------------------------------------------------------------------------------------
// @Description   Init RAK811
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void RAK811_init(void)
{
	Serial2.begin(115200);
}// end of RAK811_init()



//**************************************************************************************************
// @Function      RAK811_sendMessage()
//--------------------------------------------------------------------------------------------------
// @Description   Send message to RAK811
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    message - data to send
//**************************************************************************************************
void RAK811_sendMessage(const char* const message)
{
    Serial2.println(message);
	Serial.println(message);
}// end of RAK811_sendMessage()



//**************************************************************************************************
// @Function      RAK811_confMode()
//--------------------------------------------------------------------------------------------------
// @Description   Configure RAK811
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode - '0' LoRaWAN
//                     - '1' LoRa P2P
//**************************************************************************************************
extern void RAK811_confMode(char mode)
{
    if (RAK811_MODE_LoRaWAN == mode)
    {
        RAK811_sendMessage("at+set_config=lora:work_mode:0");
    }
    else if (RAK811_MODE_LORA_P2P == mode)
    {
        RAK811_sendMessage("at+set_config=lora:work_mode:1");
    }
}// end of RAK811_confMode



//**************************************************************************************************
// @Function      RAK811_confP2Pprm()
//--------------------------------------------------------------------------------------------------
// @Description   configure LoRaP2P parameters
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    freq - Frequency, the unit is Hz
//                spreadfactor - Spreading factor
//                bandwidth - 0: 125 kHz
//                            1: 250 kHz
//                            2: 500 kHz
//                codingrate - 1: 4/5
//                             2: 4/6
//                             3: 4/7
//                             4: 4/8 
//                preamlen - Preamble Length. 5~65535
//                power - TX power. The unit is in dBm. 5~20
//**************************************************************************************************
extern void RAK811_confP2Pprm(const char* const freq,
                              const char spreadfactor,
                              const char bandwidth,
                              const char codingrate,
                              const char preamlen,
                              const char power)
{
    char strPrm[64];
    
    snprintf(strPrm,64,"at+set_config=lorap2p:%s:%c:%c:%c:%c:%c",freq,spreadfactor,bandwidth,\
                                                                    codingrate,preamlen,power);
    RAK811_sendMessage(strPrm);                                                               
}// end of RAK811_confP2Pprm
                              

               
//**************************************************************************************************
// @Function      RAK811_confTransferMode()
//--------------------------------------------------------------------------------------------------
// @Description   configure LoRaP2P parameters
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode 1: receiver mode
//                     2: sender mode
//**************************************************************************************************               
extern void RAK811_confTransferMode(const char mode)
{
    if (RAK811_SENDER_MODE == mode)
    {
        RAK811_sendMessage("at+set_config=lorap2p:transfer_mode:2");
    }
    else if (RAK811_RECEIVER_MODE == mode)
    {
        RAK811_sendMessage("at+set_config=lorap2p:transfer_mode:1");
    }
}// end of RAK811_confTransferMode
                         
                         

//**************************************************************************************************
// @Function      RAK811_sendData()
//--------------------------------------------------------------------------------------------------
// @Description   send data
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - pointer on data to transmit              
//**************************************************************************************************
extern void RAK811_sendData(const char* data)
{
    int size = strlen(data);
    uint8_t temp=0;
    char *pBuf = buf;
    
    //clear buf
    for (int i=0;i<RAK811_SIZE_BUF;i++)
    {
        buf[i] = 0;
    }
    
    if (RAK811_SIZE_BUF > size)
    {
        for(int i=0;i<size;i++)
        {
            temp = (uint8_t)*data;
            itoa(((temp&0xf0)>>4),pBuf,16);
            pBuf++;
            itoa((temp&0x0f),pBuf,16);
        }
    }
    
    RAK811_sendMessage(buf);
    
}// end of RAK811_sendData



//**************************************************************************************************
// @Function      RAK811_hexToAscii
//--------------------------------------------------------------------------------------------------
// @Description   parsing data
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    dataIn - hex format
//                dataout - symbol format                 
//**************************************************************************************************
extern void RAK811_hexToAscii(const char* dataHex, char* dataAscii)
{
    long int temp=0;
    char strtm[3];
    
    if (strlen(dataHex) > 0)
    {
        strtm[0] = *dataHex;
        dataHex++;
        strtm[1] = *dataHex;
        dataHex++;
        strtm[2] = 0;
        
        temp = strtol(strtm,0,16);
        *dataAscii = (char)temp;
        dataAscii++;
    }
    
    
}// end of RAK811_hexToAscii
                         
//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

// None.



//****************************************** end of file *******************************************
