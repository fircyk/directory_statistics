#include "sensor.h"
#include "functions.h"
#include <stdio.h>

uint8_t bufferTX[9];
uint8_t bufferRX[9];

extern volatile uint32_t timeout;

extern volatile uint8_t DMABufRX[100];
extern volatile uint8_t BufRead[100];

extern volatile uint8_t DMABufTX[100];
extern volatile uint8_t BufWrite[100];

volatile uint16_t match;
volatile uint16_t UserCount;
volatile uint16_t UserTrack;
volatile bool SleepFlag;

/***************************************************************************
*  	Calculation of received data's checksum
****************************************************************************/
uint8_t CountRXCheckSum(void){
		
	uint8_t RXCheckSum = 0;
	
	for(int i = 1; i<7; i++){
		RXCheckSum ^= BufRead[i];		
	}
	return RXCheckSum;
}	

/***************************************************************************
*  	Getting User Count from sensor memory
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint16_t GetUserCount(){
	
		uint8_t temp;
		uint8_t CheckSum=0;
		uint8_t resp[5];
		ZeroDMABufRX();
	
		char buff[5];
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_USER_CNT;
		bufferTX[2]=0;
		bufferTX[3]=0;
		bufferTX[4]=0;
		bufferTX[5]=0;
	
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		uint8_t highCountNumber;
		uint8_t lowCountNumber;
		
		uint16_t UserCount;
		
		USART2SendDMAUINT(bufferTX, 8);
		
		delay_ms2(200);
		
		CheckSum = CountRXCheckSum();
		resp[0]=cmdHead;
		
		if(CheckSum != 0){
				
				resp[1]=cmdGetUserCount;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		if(BufRead[4] == ACK_SUCCESS && bufferTX[1] == BufRead[1]){
			highCountNumber = BufRead[2];
			lowCountNumber = BufRead[3];
			UserCount = lowCountNumber + (highCountNumber << 8);
			
			resp[1]=cmdGetUserCount;
			resp[2]=ackSuccess;
			resp[3]=lowCountNumber;
			resp[4]=QtCheckSum(resp,3);
			
			USART3SendDMAUINT(resp, 5);
			
			return UserCount;
			
		}else{
			
			resp[1]=cmdGetUserCount;
			resp[2]=ackFail;
			resp[3]=QtCheckSum(resp,2);
			resp[4]=0;
			USART3SendDMAUINT(resp, 5);
			return 0xFF;
			
		}
		
}
	
/***************************************************************************
*  	Adding user part 1 
*	Sending 8byte data frame and waiting for response	
****************************************************************************/

uint8_t AddUser1ID(uint8_t ID){

		
		uint8_t CheckSum=0;
		uint8_t resp[5];
		ZeroDMABufRX();
		
		uint8_t tempHigh = ID >> 8;
		uint8_t tempLow = ID-(256*tempHigh);
	
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_ADD_1;
		bufferTX[2]=tempHigh;
		bufferTX[3]=tempLow;
		bufferTX[4]=1;
		bufferTX[5]=0;
	
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
	
		delay_ms2(4000);
		
		CheckSum = CountRXCheckSum();
		
		resp[0]=cmdHead;
		
		if(CheckSum != 0){
				
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
		if(BufRead[4]==ACK_SUCCESS && bufferTX[1] == BufRead[1]){
				
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackSuccess;
				resp[3]=QtCheckSum(resp, 2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_SUCCESS;
		}else if(BufRead[4]==ACK_FAIL){
				
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}else if(BufRead[4]==ACK_FULL){
			
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackFull;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FULL;
		}else if(BufRead[4]==ACK_USER_OCCUPIED){
				
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackUserOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_USER_OCCUPIED;
		}else if(BufRead[4]==ACK_FINGER_OCCUPIED){
				
				resp[1]=cmdAddUserAutoIncrement;
				resp[2]=ackFingerOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FINGER_OCCUPIED;
		}else if(BufRead[4]==ACK_TIMEOUT){
				
			resp[1]=cmdAddUserAutoIncrement;
			resp[2]=ackTimeout;
			resp[3]=QtCheckSum(resp,2);
			resp[4]=0;
			USART3SendDMAUINT(resp, 5);
			return ACK_TIMEOUT;
		}else if(BufRead[4]==ACK_GO_OUT){
				
			resp[1]=cmdAddUserAutoIncrement;
			resp[2]=ackGoOut;
			resp[3]=QtCheckSum(resp,2);
			resp[4]=0;
			USART3SendDMAUINT(resp, 5);
			return ACK_GO_OUT;
		}else{
			
			resp[1]=cmdAddUserAutoIncrement;
			resp[2]=ackFail;
			resp[3]=QtCheckSum(resp,2);
			resp[4]=0;
			USART3SendDMAUINT(resp, 5);
			return ACK_FAIL;
		}
		
}

/***************************************************************************
*  	Adding user part 3 
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint8_t AddUser3ID(uint8_t ID){

		uint8_t CheckSum=0;
		uint8_t resp[5];
		
		uint8_t tempHigh = ID >> 8;
		uint8_t tempLow = ID-(256*tempHigh);
	
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_ADD_3;
		bufferTX[2]=tempHigh;
		bufferTX[3]=tempLow;
		bufferTX[4]=1;
		bufferTX[5]=0;
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
	
		delay_ms2(4000);
		
		CheckSum = CountRXCheckSum();
		resp[0]=cmdHead;
		if(CheckSum != 0){
				
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
		if(BufRead[4]==ACK_SUCCESS && bufferTX[1] == BufRead[1]){
				
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackSuccess;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_SUCCESS;
		}else if(BufRead[4]==ACK_FAIL){
				
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}else if(BufRead[4]==ACK_FULL){
			
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackFull;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FULL;
		}else if(BufRead[4]==ACK_USER_OCCUPIED){
				
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackUserOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_USER_OCCUPIED;
		}else if(BufRead[4]==ACK_FINGER_OCCUPIED){
			
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackFingerOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FINGER_OCCUPIED;
		}else if(BufRead[4]==ACK_TIMEOUT){
				
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackTimeout;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_TIMEOUT;
		}else{
			
				resp[1]=cmdAddUserAutoIncrement3;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
}

/***************************************************************************
*  	Adding user part 2 
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint8_t AddUser2ID(uint8_t ID){

		uint8_t CheckSum=0;
		uint8_t resp[5];
		
		uint8_t tempHigh = ID >> 8;
		uint8_t tempLow = ID-(256*tempHigh);
	
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_ADD_2;
		bufferTX[2]=tempHigh;
		bufferTX[3]=tempLow;
		bufferTX[4]=1;
		bufferTX[5]=0;
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
	
		delay_ms2(4000);
		
		resp[0]=cmdHead;
		CheckSum = CountRXCheckSum();
		if(CheckSum != 0){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
		if(BufRead[4]==ACK_SUCCESS && bufferTX[1] == BufRead[1]){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackSuccess;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_SUCCESS;
		}else if(BufRead[4]==ACK_FAIL){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}else if(BufRead[4]==ACK_FULL){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackFull;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FULL;
		}else if(BufRead[4]==ACK_USER_OCCUPIED){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackUserOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_USER_OCCUPIED;
		}else if(BufRead[4]==ACK_FINGER_OCCUPIED){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackFingerOccupied;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FINGER_OCCUPIED;
		}else if(BufRead[4]==ACK_TIMEOUT){
				
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackTimeout;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_TIMEOUT;
		}else{
			
				resp[1]=cmdAddUserAutoIncrement2;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
			return ACK_FAIL;
		}
		
}

/***************************************************************************
*  	Adding user - complete function	
****************************************************************************/

uint8_t AddUserID(uint8_t ID){
	
	uint8_t temp;
	
	ZeroDMABufRX();
	
	temp = AddUser1ID(ID);
	if(temp == ACK_SUCCESS){
			temp = AddUser2ID(ID);
			if(temp == ACK_SUCCESS){
					temp = AddUser3ID(ID);
					if(temp == ACK_SUCCESS){
							UserCount++;
					}else{
							return BufRead[4];
					}
			}else{
					return BufRead[4];
			}
	}else{
			return BufRead[4];
	}

	
}

/***************************************************************************
*  	Deleting all users
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint8_t DeleteAllUsers(void){
		
		uint8_t CheckSum=0;
		uint8_t resp[5];
		ZeroDMABufRX();
	
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_DEL_ALL;
		bufferTX[2]=0;
		bufferTX[3]=0;
		bufferTX[4]=0;
		bufferTX[5]=0;
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
		delay_ms2(500);
		
		CheckSum = CountRXCheckSum();
		resp[0]=cmdHead;
		if(CheckSum != 0){
			
				resp[1]=cmdDeleteAllUsers;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
		if(BufRead[4] == ACK_SUCCESS && bufferTX[1] == BufRead[1]){
				
				resp[1]=cmdDeleteAllUsers;
				resp[2]=ackSuccess;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				UserCount=0;
				return ACK_SUCCESS;
		}			
		else{
				
				resp[1]=cmdDeleteAllUsers;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
}
	
/***************************************************************************
*  	Deleting single user with ID passed as function argument
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint8_t DeleteUser(uint16_t ID){
	
		uint8_t CheckSum=0;
		
		ZeroDMABufRX();
	
		uint8_t resp[5];
		uint8_t highID = ID >> 8;
		uint8_t lowID = (ID & 0xFF);
		
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_DEL;
		bufferTX[2]=highID;
		bufferTX[3]=lowID;
		bufferTX[4]=0;
		bufferTX[5]=0;
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
	
		delay_ms2(400);
		CheckSum = CountRXCheckSum();
		resp[0]=cmdHead;
		
		if(CheckSum != 0){
				
				resp[1]=cmdGetUserCount;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
		if(BufRead[4] == ACK_SUCCESS && bufferTX[1] == BufRead[1]){
				
				resp[1]=cmdDeleteUserID;
				resp[2]=ackSuccess;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_SUCCESS;
		}			
		else{
				
				resp[1]=cmdDeleteUserID;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				return ACK_FAIL;
		}
		
}


/***************************************************************************
*  	Matching fingerprint 
*	Sending 8byte data frame and waiting for response	
****************************************************************************/
uint16_t MatchFingerprint(void){

		uint8_t CheckSum=0;
		uint8_t resp[5];
		ZeroDMABufRX();
	
		bufferTX[0]=CMD_HEAD;
		bufferTX[1]=CMD_MATCH;
		bufferTX[2]=0;
		bufferTX[3]=0;
		bufferTX[4]=0;
		bufferTX[5]=0;
		for(int i = 1; i<6; i++){
			CheckSum ^= bufferTX[i];
		}
		bufferTX[6]=CheckSum;
		bufferTX[7]=CMD_TAIL;
		
		USART2SendDMAUINT(bufferTX, 8);
		delay_ms2(1000);
		
		CheckSum = CountRXCheckSum();
		resp[0]=cmdHead;
		if(CheckSum != 0){
				
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackCheckSumFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				LedOnOff(red, LedOn);
				delay_ms(300);
				LedOnOff(red, LedOff);
				return ACK_FAIL;
		}else if(BufRead[4] == ACK_NO_USER){
				
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackNoUser;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				LedOnOff(red, LedOn);
				delay_ms(300);
				LedOnOff(red, LedOff);
				return ACK_NO_USER;
		}else if(BufRead[4] == ACK_TIMEOUT){
			
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackTimeout;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				LedOnOff(red, LedOn);
				delay_ms(300);
				LedOnOff(red, LedOff);
				return ACK_TIMEOUT;
		}else if(BufRead[4] == ACK_GO_OUT){
			
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackGoOut;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				LedOnOff(red, LedOn);
				delay_ms(300);
				LedOnOff(red, LedOff);
				return ACK_GO_OUT;
		}else if(bufferTX[1] == BufRead[1] && BufRead[4]<=3){
				
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackSuccess;
				resp[3]=BufRead[3];
				resp[4]=QtCheckSum(resp,3);
				USART3SendDMAUINT(resp, 5);
				LedOnOff(green, LedOn);
				delay_ms(300);
				LedOnOff(green, LedOff);
				return ACK_SUCCESS;
		}else{
			
				resp[1]=cmdMatchFingerprint;
				resp[2]=ackFail;
				resp[3]=QtCheckSum(resp,2);
				resp[4]=0;
				USART3SendDMAUINT(resp, 5);
				LedOnOff(red, LedOn);
				delay_ms(300);
				LedOnOff(red, LedOff);
				return ACK_FAIL;
		}

}

/***************************************************************************
*  	Calculation checksum helpful for communication with Qt app 
****************************************************************************/
uint8_t QtCheckSum(uint8_t* buf, uint8_t length){

		uint8_t QtSum=0;
		for(int i=0; i<(length+1); i++){
				QtSum ^= buf[i];
		}
		return QtSum;
}
/***************************************************************************
*  	Enabling auto matching mode 
****************************************************************************/
void AutoMode(void){
	
	uint8_t resp[5];
	
	ZeroDMABufRX();
	
	resp[0]=cmdHead;
	resp[1]=cmdEnableAutoMatch;
	resp[2]=ackSuccess;
	resp[3]=QtCheckSum(resp, 2);
	resp[4]=0;
	
	USART3SendDMAUINT(resp, 5);
	
	while(SleepFlag){
		
		LedOnOff(blue, LedOn);
		
		if(B1Read()){
		
			MatchFingerprint();
		
		}
		
		if(DMABufRX[0]==8){
		
			resp[0]=cmdHead;
			resp[1]=cmdDisableAutoMatch;
			resp[2]=ackSuccess;
			resp[3]=QtCheckSum(resp, 2);
			resp[4]=0;
			
			LedOnOff(blue, LedOff);
			
			USART3SendDMAUINT(resp, 5);
			
			break;
			
		}
		
	}
	
	
	
}


/***************************************************************************
*  	Analysis of the command sent from Qt app
****************************************************************************/

void PCCommandAnalysis(void){
	uint16_t ID;
	switch(DMABufRX[0]){
		case 0:
			ZeroDMABufRX();
			break;
		case cmdGetUserCount:
			UserCount = GetUserCount();
		break;
		case cmdAddUserAutoIncrement:
			delay_ms(100);
			ID = DMABufRX[1];
			AddUserID(ID);
		break;
		case cmdDeleteAllUsers:
			DeleteAllUsers();
		break;
		case cmdDeleteUserID:
			delay_ms(100);
			ID = DMABufRX[1];
			DeleteUser(ID);
		break;
		case cmdMatchFingerprint:
			MatchFingerprint();
		break;
		case cmdEnableAutoMatch:
			SleepFlag = true;
			AutoMode();
		break;
		case cmdDisableAutoMatch:
			SleepFlag = false;
		break;
		default:
			ZeroDMABufRX();
		break;
	}

}



