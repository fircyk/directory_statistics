#ifndef _SENSOR
#define _SENSOR

#include "functions.h"
#include <stm32f446xx.h>
#include <system_stm32f4xx.h>
#include <stdbool.h>
#include <stm32f4xx.h>

#define TRUE  1
#define FALSE 0

// Basic response message definition
#define ACK_SUCCESS       		0x00
#define ACK_FAIL         		0x01
#define ACK_FULL	  			0x04
#define ACK_NO_USER	  			0x05
#define ACK_USER_OCCUPIED   	0x06
#define ACK_FINGER_OCCUPIED 	0x07
#define ACK_TIMEOUT       		0x08
#define ACK_GO_OUT	  			0x0F		

//User information definition
#define ACK_ALL_USER       		0x00
#define ACK_GUEST_USER 	  		0x01
#define ACK_NORMAL_USER 	  	0x02
#define ACK_MASTER_USER    		0x03

#define USER_MAX_CNT	   	 	 255	


// Command definition
#define CMD_HEAD	   				0xF5
#define CMD_TAIL	 	 			0xF5
#define CMD_ADD_1    				0x01
#define CMD_ADD_2    				0x02
#define CMD_ADD_3	   				0x03
#define CMD_MATCH_1ON1				0x0B
#define CMD_MATCH	   				0x0C
#define CMD_DEL		   				0x04
#define CMD_DEL_ALL	 				0x05
#define CMD_MODIFY_SN				0x08
#define CMD_USER_CNT 				0x09
#define CMD_COM_LEV	 				0x28
#define CMD_GET_SN					0x2A
#define CMD_LP_MODE  				0x2C
#define CMD_TIMEOUT  				0x2E

#define CMD_FINGER_DETECTED 0x14

//helpful Qt definitions

#define cmdNull  					0x0
#define cmdGetUserCount  			0x1
#define cmdAddUserAutoIncrement 	0x2
#define cmdAddUserAutoIncrement2  	0x3
#define cmdDeleteAllUsers  			0x4
#define cmdDeleteUserID  			0x5
#define cmdMatchFingerprint  		0x6
#define cmdEnableAutoMatch  		0x7
#define cmdDisableAutoMatch  		0x8
#define cmdAddUserAutoIncrement3 	0x9
#define cmdHead 					0x90

#define ackNull 					0xA
#define ackSuccess 					0xB
#define ackFail 					0xC
#define	ackUserOccupied 			0xD
#define	ackFingerOccupied 			0xE
#define	ackTimeout 					0xF
#define	ackNoUser 					0x10
#define	ackFull 					0x11
#define ackCheckSumFail  			0x12
#define ackGoOut 					0x13

uint8_t CountRXCheckSum(void);

uint8_t AddUserID(uint8_t ID);

uint8_t AddUser1ID(uint8_t ID);
uint8_t AddUser2ID(uint8_t ID);
uint8_t AddUser3ID(uint8_t ID);

uint16_t GetUserCount(void);

uint8_t DeleteAllUsers(void);
uint8_t DeleteUser(uint16_t ID);

uint16_t MatchFingerprint(void);

uint8_t QtCheckSum(uint8_t* buf, uint8_t length);

void AutoMode(void);

void PCCommandAnalysis(void);


#endif

