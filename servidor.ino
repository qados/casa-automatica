// inclusão das bibliotecas

#include <SPI.h>
#include <Ethernet.h>

#include "site.h"

//Definições de IP, máscara de rede e gateway
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 10);       //Define o endereco IP que pode ser alterado
IPAddress gateway(192, 168, 1, 1);   //Define o gateway
IPAddress subnet(255, 255, 255, 0);  //Define a máscara de rede

//Inicializa o servidor web na porta 80
EthernetServer server(80);

void setup() {
	//Inicia o monitor serial
	Serial.begin(9600);

	//Inicializa a interface de rede
	Ethernet.begin(mac, ip, gateway, subnet);
	server.begin();
}

void loop() {
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
	}
}
