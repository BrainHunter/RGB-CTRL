#include "HMWWrapper.h"
#include "HMWRS485.h"
#include "HMWModule.h"
#include "rs485Stream.h"
#include "rs485.h"

// Klasse fuer Callbacks vom Protokoll
class HMWDevice : public HMWDeviceBase {
  public:

	void setLevel(byte channel,uint32_t level) {


	}


	unsigned int getLevel(byte channel) {
		return 12;
	};



	void readConfig(){


	};

};

HMWDevice hmwdevice;
HMWRS485* hmwrs485;
HMWModule* hmwmodule;


void Init_HMW(){
	rs485Init();

	hmwrs485 = new HMWRS485(&RS485Stream, 0);
	hmwmodule = new HMWModule(&hmwdevice, hmwrs485, 0xA0);


    hmwmodule->broadcastAnnounce(0);


}

void HMW_loop(){
	hmwrs485->loop();
}

void HMWbroadcast(void)
{
    hmwmodule->broadcastAnnounce(0);

}

