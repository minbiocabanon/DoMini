// Test ACK : EMITER SIDE
// http://jeelabs.org/2011/06/10/rf12-broadcasts-and-acks/
// http://jeelabs.org/2011/06/09/rf12-packet-format-and-design/
// http://jeelabs.net/boards/7/topics/698?r=845

#include <JeeLib.h>
#define RADIO_SYNC_MODE 2
#define ACK_TIME 5000
#define NB_ATTEMPTS_ACK 3

char payload[]="$TEST_ACK\0";

static byte waitForAck() {
	MilliTimer ackTimer;
	while (!ackTimer.poll(ACK_TIME)){
		if (rf12_recvDone() && rf12_crc == 0 && ((rf12_hdr & RF12_HDR_ACK) == 0) && ((rf12_hdr & RF12_HDR_CTL) == 128) ){
			Serial.println("ACK Received");
			Serial.print("Node ID:");Serial.println(rf12_hdr & RF12_HDR_MASK); 
			// Serial.println("received something");
			// Serial.println(rf12_hdr);
			// Serial.print("RF12_HDR_DST=");Serial.println(rf12_hdr & RF12_HDR_DST);
			// Serial.print("RF12_HDR_CTL=");Serial.println(rf12_hdr & RF12_HDR_CTL);
			// Serial.print("RF12_HDR_ACK=");Serial.println(rf12_hdr & RF12_HDR_ACK);
			// Serial.print("rf12_len=");Serial.println(rf12_len);
			return 1;
		}
	}
	return 0;
}

void setup() {
	Serial.begin(57600);
	rf12_initialize(5, RF12_868MHZ, 33);
	// RF12 air baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.println("\nInit Radio : 1200 bauds");
}

void loop() {
	
	int nAttempt = 1;
	bool flag_ACK_received = false;
	while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received ){

		while (!rf12_canSend())
			rf12_recvDone();

		rf12_sendStart(RF12_HDR_ACK, payload, sizeof payload);
		rf12_sendWait(1);
		
		Serial.print("Attemps : ");Serial.println(nAttempt);
		
		if (waitForAck()){
			Serial.println("ACK received\n");
			flag_ACK_received = true;
		}else {Serial.println("ACK NOK received\n");}		
		nAttempt++;
	}	
	delay(10000);
}