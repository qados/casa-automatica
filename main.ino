// inclusão das bibliotecas
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <Stepper.h>

#include "site.h"

#define leituraAnalogica A2 //Atribui o pino A5 a variável leituraAnalogica
#define leituraDigital 48 //Atribui o pino 8 a variável leituraDigital 

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

/*
// Teclado numérico
const byte QTD_LINHAS = 4;
const byte QTD_COLUNAS = 4;
char teclas[QTD_LINHAS][QTD_COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};      
byte pinosLinhas[QTD_LINHAS] = {12, 11, 10, 9};
byte pinosColunas[QTD_COLUNAS] = {8, 7, 6, 5};

Keypad teclado = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, QTD_LINHAS, QTD_COLUNAS);
*/

// Pinos
const byte pinos[] = {20, 21, 22, 23, 24, // leds
					  30, 31, 32, 33}; // servos
const byte QTD_PINOS = 9;

Servo servoJanelaAlta, servoGaragem, servoJanelaBaixa, servoVaral;
const byte pinoServo1 = 2;
const byte pinoServo2 = 3;
const byte pinoServo3 = 4;
const byte pinoVaral = 50;

// Motor de passo
const int PASSOS_POR_VOLTA = 2048;
const int PASSOS_FIXOS = 5632; // 2,75 voltas de 2048 passos
Stepper stepperPortao(PASSOS_POR_VOLTA, 40, 44, 42, 46);
boolean posicaoAberta = false;

// Definições de IP, máscara de rede e gateway
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 10);       //Define o endereco IP que pode ser alterado
IPAddress gateway(192, 168, 1, 1);   //Define o gateway
IPAddress subnet(255, 255, 255, 0);  //Define a máscara de rede

// Inicializa o servidor web na porta 80
EthernetServer server(80);

void setup() {
	// Inicia o monitor serial
	Serial.begin(9600);

	// Inicializa a interface de rede
	Ethernet.begin(mac, ip, gateway, subnet);
	server.begin();

	pinMode(leituraAnalogica, INPUT); //Define leituraAnalogica como entrada
	pinMode(leituraDigital, INPUT); //Define leituraDigital como entrada

	// Inicializa a tela
	lcd.init();
	lcd.backlight();
	mostrarMensagemInicial();

	// Inicializa todos os pinos como output e desliga todos
	for (int i = 0; i < QTD_PINOS; i++) {
		pinMode(pinos[i], OUTPUT);
		digitalWrite(pinos[i], LOW);
	}

	// Define os servo motores e motor stepper
	servoJanelaAlta.attach(pinoServo1);
	servoGaragem.attach(pinoServo2);
	servoJanelaBaixa.attach(pinoServo3);
	servoVaral.attach(pinoVaral);

	servoJanelaAlta.write(0);
	servoGaragem.write(165);
	servoJanelaBaixa.write(90);
	servoVaral.write(90);

	stepperPortao.setSpeed(12);
}

void loop() {
	// Caso o sensor de chuva detecte umidade, resete todos os servos
	if (lerSensor()) {
		servoJanelaAlta.write(90);
		servoGaragem.write(0);
		servoJanelaBaixa.write(90);
		servoVaral.write(0);
	} else {
		servoVaral.write(90); // Se não, estenda o varal
	};
	
	mostrarMensagemInicial();

	// Ethernet
	String request;
	//Aguarda conexão do browser
	EthernetClient client = server.available();
	if (client) {
		Serial.println("novo dispositivo conectado");
		boolean currentLineIsBlank = true;
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				Serial.write(c);
				Serial.print("");

				request += c;

				// se a solicitação http terminou então envia a resposta
				if (c == '\n' && currentLineIsBlank) {
					gerarSite(client);
			  		break;
				}
				if (c == '\n') {
					// começando nova linha
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// novo caractere na linha atual
					currentLineIsBlank = false;
				}
			}
		}
		// tempo para o navegador receber as informações
		delay(1);
		// fechamento da conexão
		client.stop();
		Serial.println("cliente fechou conexão");

		// Comando recebido
		if (request.startsWith("POST")) {
			lcd.clear();
			lcd.setCursor(0, 0);
			String room = request.substring(7, 11); // onde será feita a mudança (sala, quarto, etc)
			String changeReq = request.substring(12, 17); // ligar ou desligar

			// Normalizar o pedido
			changeReq.trim();
			bool change;

			if (changeReq == "true") {
				change = true;
			} else if (changeReq == "false") {
				change = false;
			}
    
			// LEDs
			if (room == "sala") {
				digitalWrite(13, change);
				lcd.print("Luz Sala"); 		lcd.setCursor(0, 1); lcd.print(change ? "LIGADO" : "DESLIGADO");
			} else if (room == "qua1") {
				digitalWrite(21, change);
				lcd.print("Luz Quarto 1"); 	lcd.setCursor(0, 1);	lcd.print(change ? "LIGADO" : "DESLIGADO");
			} else if (room == "qua2") {
				digitalWrite(22, change);
				lcd.print("Luz Quarto 2"); 	lcd.setCursor(0, 1);	lcd.print(change ? "LIGADO" : "DESLIGADO");
			} else if (room == "czna") {
				digitalWrite(23, change);
				lcd.print("Luz Cozinha"); 	lcd.setCursor(0, 1);	lcd.print(change ? "LIGADO" : "DESLIGADO");
			} else if (room == "bnho") {
				digitalWrite(24, change);
				lcd.print("Luz Banheiro"); 	lcd.setCursor(0, 1);	lcd.print(change ? "LIGADO" : "DESLIGADO");
			// Servos
			} else if (room == "jan1") {
				servoJanelaAlta.write(change ? 0 : 90);
				lcd.print("Janela 1"); 		lcd.setCursor(0, 1);	lcd.print(change ? "ABERTA" : "FECHADA");
			} else if (room == "jan2") {
				servoJanelaBaixa.write(change ? 0 : 90);
    			lcd.print("Janela 2"); 		lcd.setCursor(0, 1);	lcd.print(change ? "ABERTA" : "FECHADA");
			} else if (room == "grgm") {
				servoGaragem.write(change ? 180 : 0);
    			lcd.print("Garagem"); 		lcd.setCursor(0, 1);	lcd.print(change ? "ABERTA" : "FECHADA");
			// Stepper
			} else if (room == "port") {
				lcd.print("Portao ");
				lcd.setCursor(0, 1);
			
				if (change) {
					if (!posicaoAberta) { // Só avança se estiver fechado
						lcd.print("Abrindo portao...");
						stepperPortao.step(-PASSOS_FIXOS); // Valor positivo = Sentido Horário (Clockwise)
						posicaoAberta = true;          // Atualiza o estado para aberto
					} else {
						lcd.print("Ja esta aberto!");
					}
				} 
				else {
					if (posicaoAberta) { // Só retorna se estiver aberto
						lcd.print("Fechando portao...");
						stepperPortao.step(PASSOS_FIXOS); // Valor negativo = Sentido Anti-Horário (Counter-Clockwise)
						posicaoAberta = false;          // Atualiza o estado para fechado
					} else {
						lcd.print("Ja esta fechado!");
					}
				}

				// Sempre desliga as bobinas após o movimento para economizar energia
				digitalWrite(40, LOW);
				digitalWrite(42, LOW);
				digitalWrite(44, LOW);
				digitalWrite(46, LOW); 
			}

			delay(2000);
			mostrarMensagemInicial();
		}
	}
}

void mostrarMensagemInicial() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Mini Casa Auto");
	lcd.setCursor(0, 1);
	lcd.print("Selecione comp.");
}

bool lerSensor() {
	int valorDigital = digitalRead(leituraDigital); // Realiza a leitura digital do sensor e armazena em valorDigital

	Serial.println(valorDigital);
	if (valorDigital == 0) { //Se esse valor for igual a 0, será mostrado no monitor serial que a chuva foi detectada e o led verde se acende
		Serial.println("Status: Chuva detectada");
		return true;
	}
	else { // Se esse valor for igual a 1, será mostrado no monitor serial que o solo está seco e o led vermelho se acende
		Serial.println("Status: Chuva não detectada");
		return false;
	}
	delay(500); // Atraso de 500ms
}
