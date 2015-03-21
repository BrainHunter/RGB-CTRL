#ifndef __HM485_H__
#define __HM485_H__


// Protocol defines:

#define FRAME_START_SHORT	0xFE
#define FRAME_START_LONG	0xFD
#define ESCAPE_CHAR			0xFC

// Commands from client
#define CMD_SEND  			0x53
#define CMD_DISCOVERY  		0x44
// Commands to client
#define CMD_RESPONSE 		0x72
#define CMD_ERROR 			0x61
#define CMD_EVENT 			0x65
#define CMD_DISCOVERY_RESULT 0x64
#define CMD_DISCOVERY_END 	0x63


/*
MAX_SEND_RETRY => 3,
SEND_RETRY_TIMEOUT => 200, # die CCU macht nach 200ms eine Neusendung?
DISCOVERY_TRIES => 3,
DISCOVERY_TIMEOUT => 20, # 15ms
# DISCOVERY_TIMEOUT => 50,
STATE_IDLE => 0x00,
STATE_WAIT_ACK => 0x03,
STATE_WAIT_RESPONSE => 0x04,
STATE_ACKNOWLEDGED => 0x05,
STATE_DISCOVERY => 0x06,
STATE_DISCOVERY_WAIT => 0x07,*/


#endif
