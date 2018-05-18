#include "WISOL.h"

#define TAM_PACOTE 12 // Tamanho do pacote a ser enviado para o servidor

// Comunicação Sigfox
uint8_t pacote[TAM_PACOTE];
WISOL wisol;
recvMsg* in_msg;

// Comunicação Serial User
char input = 0;

/*****************SETUP****************/

void setup() {
  // Inicializando os módulos da Sigfox (aqui ele também inicializa o Serial)
  
   Serial.begin(9600);
   wisol.initSigfox();
 
}

/*****************LOOP****************/

void loop() {

  // Definindo o tamanho do pacote
  pacote[0] = 128;
  pacote[1] = 15;
  pacote[2] = 64;
  pacote[3] = 57;
  pacote[4] = 32;
  pacote[5] = 21;
  pacote[6] = 90;
  pacote[7] = 83;
  pacote[8] = 75;
  pacote[9] = 43;
  pacote[10] = 78;
  pacote[11] = 0;

  if (Serial.available() > 0) {    // is a character available?
    input = Serial.read();       // get the character
  
    // Verificando o pedido de envio do usuário
    if (input >= 's') {
      int i;
      for(i = 0; i<5; i++) {
        Serial.println("Pedido para enviar: ");
        wisol.getZone();
        wisol.setZone();
        
        // Acho que um desses manda a mensagem pro backend da sigfox, mas eu ainda n sei qual a diferença entre eles
        wisol.sendPayload(pacote, TAM_PACOTE, 0);
        //wisol.sendMessage(pacote, TAM_PACOTE, in_msg);
        // Aparentemente, nos dois, ele já chama também a função de mostrar a mensagem recebida
        delay(500);
      }
    }
  } 

  delay(100);
}
