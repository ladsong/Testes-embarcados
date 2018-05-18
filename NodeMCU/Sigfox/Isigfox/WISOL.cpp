/*
  WISOL.cpp - Interface for Sigfox library.
  Created by Thomas Ho, Thinxtra Solutions Pty.
  July 17, 2017.
  Released into the public domain.
*/

#include "Arduino.h"
#include "WISOL.h"

// #define SERIAL1_RXPIN 12 // Pinos para a Serial1
// #define SERIAL1_TXPIN 13

//Criação das novas portas seriais
HardwareSerial Serial2(2);


// Assign IO for WISOL - Not needed since WISOL share TX, RX with Ardruino
void WISOL::configIO(pinIO pin){

}


/*
int init()
	- Output: return 0 if init succesfully. Return -1 otherwise.
	- Purpose: Init Serial and set UL Frequency to 920.8 MHz
*/
int WISOL::initSigfox(){ // OK
	// Init Serial
	Serial2.begin(9600, SERIAL_8N1); // SIGFOX
	//Serial.println("initSigfox");
	currentZone = getZone();

	switch (currentZone){
		case RCZ1:
		{
			Serial.println("RCZ1");
			break;
		}
		case RCZ2:
		{
			Serial.println("RCZ2");
			break;
		}
		case RCZ4:
		{
			Serial.println("RCZ4");
			break;
		}
		default:
		{
			Serial.println("No zone");
			clearBuffer();
			return -1;
		}
	}
	clearBuffer();
	return 0;
}

int WISOL::setPublicKey() {
	recvMsg *receivedMsg;
	int receivedResult;
	int ret = 0;

	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));
	receivedResult = sendMessage("ATS410=1", 8, receivedMsg);

	if (receivedResult == -1){
		Serial.println("Cannot set public key.");
		clearBuffer();
		ret = 1;
	} else {
		for (int i=0; i<receivedMsg->len; i++){
			Serial.print(receivedMsg->inData[i]);
		}
		Serial.println("");
	}

	free(receivedMsg);
	return ret;
}

int WISOL::setPrivateKey() {
	recvMsg *receivedMsg;
	int receivedResult;
	int ret = 0;

	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));
	receivedResult = sendMessage("ATS410=0", 8, receivedMsg);

	if (receivedResult == -1){
		Serial.println("Cannot set private key.");
		clearBuffer();
		ret = 1;
	} else {
		for (int i=0; i<receivedMsg->len; i++){
			Serial.print(receivedMsg->inData[i]);
		}
		Serial.println("");
	}

	free(receivedMsg);
	return ret;
}

int WISOL::resetMacroChannel() {
	recvMsg *receivedMsg;
	int receivedResult;
	int ret = 0;

	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));
	receivedResult = sendMessage("AT$RC", 5, receivedMsg);
	if (receivedResult == -1){
		Serial.println("Cannot reset Macro Channel.");
		clearBuffer();
		ret = 1;
	}

	free(receivedMsg);
	return ret;
}

int WISOL::getZone(){
	recvMsg *receivedMsg;
	int receivedResult;
	int ret = 0;
	//Serial.println("getZone");

	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));

	receivedResult = sendMessage("AT$DR?", 6, receivedMsg);
	//Serial.print("inData = ");
	//Serial.println(receivedMsg->inData);

	if (strCmp(receivedMsg->inData, "905200000", 9)) {
		ret = RCZ2;
	} else if (strCmp(receivedMsg->inData, "922300000", 9)) {
		ret = RCZ4;
	} else if (strCmp(receivedMsg->inData, "0869525000", 10)) {
		ret = RCZ1;
	} else if (strCmp(receivedMsg->inData, "869525000", 9)) {
		ret = RCZ1;
	} else if (strCmp(receivedMsg->inData, "922200000", 9)) {
		ret = RCZ3;
	} else {
		ret = 0;
	}

	free(receivedMsg);
	return ret;
}


int WISOL::setZone(){
	return -1;
}

void WISOL::printRecv(char *in, const int len) {
	for (int i=0; i<len; i++){
		Serial.print(in[i]);
	}
	Serial.println("");
	clearBuffer();
}


int WISOL::strCmp(char *in1, char *in2, const int len){
	for (int i = 0; i<len; i++){
		if (in1[i]!=in2[i]){
			return 0;
		}
	}
	return 1;
}

/*
int testComms()
 - Send "AT" and wait for "OK" back
 - Return 0 if testing succesfully (receive "1234" back)
 - Return -1 otherwise
*/
int WISOL::testComms(){
	char testchar[] = "AT";
	recvMsg *receivedMsg;
	int receivedResult;
	int ret = 0;

	Buffer_Init(); // Prepare buffer
	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));
	receivedResult = sendMessage(testchar, 2, receivedMsg); // Send message

	// Read ACK
	if (receivedResult == -1) {
		ret = -1;
	} else {
		if (receivedMsg->inData[0] == 'O' && receivedMsg->inData[1] == 'K') {
			ret = 0;
		} else {
			ret = -1;
		}
	}

	free(receivedMsg);
	clearBuffer();
	return ret;
}

/*
recvMsg sendPayload(char *inData, int len){
	- Input: inData is a pointer to the sending payload.
	         len is the length (in bytes) of the sending payload.
	- Output: recvMsg is received message
Important note: inData is a string of hex (i.e., to send a byte of 0xAB, we need inData = {'A', 'B'})
*/
int WISOL::sendPayload(uint8_t *outData, const uint8_t len, const int downlink, recvMsg *receivedMsg){
	int receivedResult;

	if ((len > 12) || (len <= 0)){
		Serial.println("Payload length must be positive and not be longer than 12 bytes.");
		clearBuffer();
		return -1;
	}

	if (outData == NULL){
		Serial.println("outData is NULL.");
		clearBuffer();
		return -1;
	}

	if (!((downlink == 0) || (downlink == 1))){
		Serial.println("downlink must be 0 or 1.");
		clearBuffer();
		return -1;
	}

	if (receivedMsg == NULL){
		Serial.println("receivedMsg is NULL.");
		clearBuffer();
		return -1;
	}
	int i;
	Serial.println("Pacote = ");
	for(i=0; i<len; i++) {
		Serial.print(outData[i]);
		Serial.print(" ");
	}
	Serial.println();

	receivedResult = sendPayloadProcess(outData, len, downlink, receivedMsg);

	return receivedResult;
}


/*
recvMsg sendPayload(char *inData, int len){
	- Input: inData is a pointer to the sending payload.
	         len is the length (in bytes) of the sending payload.
	- Output: recvMsg is received message
Important note: inData is a string of hex (i.e., to send a byte of 0xAB, we need inData = {'A', 'B'})
*/
int WISOL::sendPayload(uint8_t *outData, const uint8_t len, int downlink){
	int receivedResult;

	// Verifica se o dado está OK pra enviar
	if ((len > 12) || (len <= 0)){
		Serial.println("Payload length must be positive and not be longer than 12 bytes.");
		clearBuffer();
		return -1;
	}

	if (outData == NULL){
		Serial.println("outData is NULL.");
		clearBuffer();
		return -1;
	}

	if (!((downlink == 0) || (downlink == 1))){
		Serial.println("downlink must be 0 or 1.");
		clearBuffer();
		return -1;
	}
	
	// Mostrando o pacote que eu vou enviar
	/*int i;
	Serial.println("Pacote = ");
	for(i=0; i<len; i++) {
		Serial.print(outData[i]);
		Serial.print(" ");
	}
	Serial.println();*/
	
	//Verifica se precisa mudar a zona
	checkChangeZone();
	
	// Envia o pacote
	receivedResult = sendPayloadProcess(outData, len, downlink, NULL);

	return receivedResult;
}


int WISOL::sendPayloadProcess(uint8_t *outData, const uint8_t len, const int downlink, recvMsg *receivedMsg){
	const char header[] = "AT$SF=";
	const uint8_t headerLen = (uint8_t) strlen(header);
	int actualLen;
	int sendLen;
	char* hex_str;
	int receivedResult;

	if ((outData[len] == '\0') || (outData[len] == '\n')){
		actualLen = len - 1;
	} else {
		actualLen = len;
	}

	hex_str = (char*) malloc (2 * actualLen);
	ASCII2Hex(outData, actualLen, hex_str);
	sendLen = 2*actualLen;

	clearBuffer();
	receivedResult = prepareZone();

	if (receivedResult == -1){
		Serial.println("Prepare zone failed");
		clearBuffer();
		return -1;
	}

	delay(20);
	Buffer_Init();
	//Serial.print("sendPayloadProcess = ");
	for (int i=0; i<headerLen; i++){
		Serial2.print(header[i]); // print header first
		//Serial.print(header[i]);
	}
	//Serial.println();

	for (int i=0; i<sendLen; i++){
		Serial2.print(hex_str[i]); // print payload
		//Serial.print(hex_str[i]);
	}
	//Serial.println();

	if (downlink == 1){
		Serial2.print(",1");
	} else {

	}

	Serial2.println('\0'); // send end terminal
	free(hex_str); // free hex_str from the memory

	if (receivedMsg != NULL){
		receivedResult = getRecvMsg(receivedMsg, downlink); // Read ACK
		Serial.println("ReceivedMsg not null");
	} else {
		Serial.println("ReceivedMsg null");
		return 0; // No wait for ack
	}
	return receivedResult;
}



/*
	sendMessage
	- Input: outData is a pointer to the sending message.
	         len is the length of the sending message.
	- Output: recvMsg is received message
*/
int WISOL::sendMessage(char *outData, const uint8_t len, recvMsg *receivedMsg){
	int receivedResult;
	//Serial.println("sendMessage");
	
	if (len <= 0){
		Serial.println("Payload length must be positive.");
		clearBuffer();
		return -1;
	}

	if (outData == NULL){
		Serial.println("outData is NULL.");
		clearBuffer();
		return -1;
	}

	if (receivedMsg == NULL){
		Serial.println("receivedMsg is NULL.");
		clearBuffer();
		return -1;
	}

	clearBuffer();
	Buffer_Init(); // prepare buffer
	
	for (int i=0; i<len; i++){
		Serial2.print(outData[i]); // send message
		//Serial.print(outData[i]);
	}
	Serial2.println('\0'); // send end terminal
	//Serial.println('\0');

	receivedResult = getRecvMsg(receivedMsg, 0); // read ack or return payload

	return receivedResult;
}

/*
recvMsg prepareZone()
	- Set the zone to ZC
*/
int WISOL::prepareZone(){
	recvMsg *receivedMsg;
	int receivedResult;

	receivedMsg = (recvMsg *)malloc(sizeof(recvMsg));
	Serial.print("Prepare zone = ");
	Serial.println(currentZone);
	
	switch (currentZone){
		case RCZ1:
		{
			Serial.println("RCZ1");
			char testchar[] = "ATS302=15";
			receivedResult = sendMessage(testchar, (int) strlen(testchar), receivedMsg);
			break;
		}
		case RCZ2:
		{
			Serial.println("RCZ2");
			checkChangeZone();
			break;
		}
		case RCZ4:
		{
			Serial.println("RCZ4");
			checkChangeZone();
			// const char testchar[] = "AT$RC";
			// receivedResult = sendMessage(testchar, (int) strlen(testchar), receivedMsg);
			break;
		}
		case RCZ3:
		{
			Serial.println("RCZ3");
			char testchar[] = "ATS302=15";
			receivedResult = sendMessage(testchar, (int) strlen(testchar), receivedMsg);
			break;
		}
		default:
		{
			Serial.println("Default");
			receivedResult = -1;
			break;
		}
	}
	free(receivedMsg);
	return receivedResult;
}


// check the need of reset the macro channels
void WISOL::checkChangeZone() {
	recvMsg *RecvMsg;
	char msg[] = "AT$GI?";
	char testchar[] = "AT$RC";
	char X, Y;
	int receivedResult;

	RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
	receivedResult = sendMessage(msg, (int) strlen(msg), RecvMsg);

	X = RecvMsg->inData[0];
	Y = RecvMsg->inData[2];

	for (int i=0; i<RecvMsg->len; i++){
		Serial2.print(RecvMsg->inData[i]);
	}
	Serial2.println("");

	if ((X=='0') || (Y<'3')) {
		receivedResult = sendMessage(testchar, (int) strlen(testchar), RecvMsg);
	}

	free(RecvMsg);
	clearBuffer();
	//return receivedResult;
}


// prepare buffer
void WISOL::Buffer_Init()
{
	//Serial.println("Buffer_Init");
  for (int i = 0; i < BUFFER_SIZE; i++) {
	master_receive[i] = 0xFF;
  }
  //Serial.prinln(master_receive);
  
}


/*
recvMsg getRecvMsg()
	- Output: return receive message from WISOL
*/
int WISOL::getRecvMsg(recvMsg *receivedMsg, const int downlink){
	int count = 1;
	int countMax;
	int receivedResult;
	
	//Serial.println("getRecvMsg");

	if (downlink == 1){
		countMax = 460; // wait 45s + 1 extra second
		//Serial.println("downlink 1");
	} else {
		//Serial.println("downlink else");
		countMax = 100; // wait 10 second
	}

	// Wait for the incomming message
	while ((Serial2.available() == 0) && (count <= countMax)) {
		count++;
		delay(100);
	}
	//if(Serial2.available() == 0) Serial.println("Serial not available");
	receivedResult = getdownlinkMsg(receivedMsg);

	return receivedResult;
}


int WISOL::getdownlinkMsg(recvMsg *receivedMsg){

	// Prepare receive message format
	receivedMsg->len = Serial2.available();
	receivedMsg->inData = master_receive;
	
	//Serial.println("getdownlinkMsg");
	//Serial.println(receivedMsg->len);
	
	if (receivedMsg->len){
		//Serial.print("master_receive = ");
		for (int i=0; i<receivedMsg->len; i++){
			master_receive[i] = Serial2.read(); // Read receive message
			//Serial.print(master_receive[i]);
		}
		//Serial.println();

		if (strCmp(receivedMsg->inData, "OK", 2)==1){
			//Serial.println("OK");
			return 0;
		} else {
			//Serial.println("else OK");
		}

		if (strCmp(receivedMsg->inData, "Er", 2)){
			//Serial.println("Er");
			return -1;
		} else {
			//Serial.println("else Er");
			return 0;
		}

	} else {
		//Serial.println("else len");
		return -1;
	}
	clearBuffer();
}


void WISOL::ASCII2Hex(uint8_t* input, int length, char* buf_str){
    for (int i = 0; i < length; i++)
    {
        buf_str += sprintf(buf_str, "%02X", input[i]);
    }
    sprintf(buf_str,"\n");
}


/* Not used */
recvMsg WISOL::goDeepSleep(){
	Serial.println("AT$P=2");
	Serial.print('\0');
}


/* Not used */
void WISOL::wakeDeepSleep(){
	//EIMSK &= 0xFD; // disable interrupt 1
	pinMode(3, OUTPUT);
	digitalWrite(3, LOW);
	delay(50);
	digitalWrite(3, HIGH);
	pinMode(3, INPUT_PULLUP);
	//EIMSK |= 0x02; // enable interrupt 1
}

void WISOL::clearBuffer(){
	Serial2.print("\0"); // Make sure there is no unfinished message.
	switch (currentZone){
		case RCZ1:
		{
			delay(1000);
			break;
		}
		case RCZ2:
		{
			delay(50);
			break;
		}
		case RCZ4:
		{
			delay(50);
			break;
		}
		default:
		{

		}
	}

	while (Serial2.available()!=0){
		Serial2.read();
		delay(10);
	}
}
