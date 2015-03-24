#include "HMWWrapper.h"
#include "HMWRS485.h"
#include "HMWModule.h"
#include "HMWDebug.h"
#include "rs485Stream.h"
#include "rs485.h"
#include "DebugStream.h"
#include "rgb.h"

// Klasse fuer Callbacks vom Protokoll
class HMWDevice : public HMWDeviceBase {
  public:

    //  channel 0..2 level 0..100
	void setLevel(byte channel,uint16_t level) {
        RGB_setChannel(channel+3,(float)level/200.0);

	}


	uint16_t getLevel(byte channel) {
		uint16_t ret;
		ret = RGB_getChannel(channel+3)*200.0;
		return ret;
	};


	void readConfig(){


	};

};

HMWDevice hmwdevice;
HMWRS485* hmwrs485;
HMWModule* hmwmodule;



void Init_HMW(){
	rs485Init();

	hmwdebugstream = new DebugStream();

	hmwrs485 = new HMWRS485(&RS485Stream, 0);
	hmwmodule = new HMWModule(&hmwdevice, hmwrs485, 0xA0);

    // initial Broadcast Announce at power up
    hmwmodule->broadcastAnnounce(0);

}

void HMW_loop(){
	hmwrs485->loop();
}

void HMWbroadcast(void)
{
    hmwmodule->broadcastAnnounce(0);

}

