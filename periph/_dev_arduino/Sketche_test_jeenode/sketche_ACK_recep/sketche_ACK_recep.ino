// Test ACK : RECEIVER SIDE
#include <JeeLib.h>

char buffer_recept_rf12[66]="";

void setup() {
	Serial.begin(57600);
	rf12_initialize(1, RF12_868MHZ, 33);
	// Air baudrate : 1200 bauds
	rf12_control(0xC6A3);
	Serial.println("\nInit Radio : 1200 bauds");
}

void loop() {
	if (rf12_recvDone() && rf12_crc == 0){
		memcpy(buffer_recept_rf12, (void*) rf12_data, rf12_len);
		buffer_recept_rf12[rf12_len]=0;

		if(RF12_WANTS_ACK){
			rf12_sendStart(RF12_ACK_REPLY, 0, 0);
			Serial.println("\n ACK sent");
		}	
		Serial.println(buffer_recept_rf12);
	}
}