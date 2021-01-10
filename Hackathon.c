//Autor: Yuri Cristhian da Cruz

//Sistema de monitoramento de presença, queda e frequência cardíaca desenvolvido durante o Hackathon Manuel,
//onde a proposta era utilizar sensores e um arduíno para criar uma aplicação para integrar IA, IOT e assistentes
//digitais para construir um healthcare para pessoas de 3° idade.

//Sensores: DYP-ME003, SM-S4303R, SW-18015.

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <SoftwareSerial.h>


const int PulseWire = 2; 							// Pino do sensor de batimento ligado a porta A2
const int LED13 = 13; 								// Led on-board do Arduíno
int Threshold = 520; 								// Valor limite do sinal
int queda_D6 = 6; 									// Pino do sensor de queda ligado a porta D0
int presenca_D7 = 7; 								// Pino do sensor de presença ligado a porta D7
int presenca_data, queda_dataD, i; 					
int queda_aux = 0, presenca_aux = 0, BPM_aux = 0; 	
unsigned long int timer = 0, pulse = 0;
PulseSensorPlayground pulseSensor; 					

void setup() {
	Serial.begin(9600); 							// Inicia a serial e configura o baud rate
	pulseSensor.analogInput(PulseWire); 			// Define o pino analógico a receber o sinal do sensor de pulso
	pulseSensor.blinkOnPulse(LED13); 				// Blinka o led do pino 13 de acordo com o batimento cardíaco
	pulseSensor.setThreshold(Threshold); 			// Seta o threshold (valor limite)
	pinMode(queda_D6, INPUT); 						// Define o pino D6 como entrada do sensor de queda
	pinMode(presenca_D7, INPUT); 					// Define o pino D7 como entrada do sensor de presença
	pulseSensor.begin();
}

void loop() {
	int BPM = pulseSensor.getBeatsPerMinute(); 		// Faz leitura do sensor de batimento (em batimentos por minuto)

	if (BPM != 0) {
		BPM_aux = (BPM_aux + BPM) / 2; 				// Filtro Moving Average
	}

	presenca_data = digitalRead(presenca_D7); 		
	queda_dataD = digitalRead(queda_D6); 			
	if (presenca_data == 1) {
		presenca_aux = 1; 							// Guarda o valor para não haver perda devido ao delay
	}
	if (queda_dataD == 0) {
		queda_aux = 0;
	}
	// Troca o valor do sensor de queda (1 para queda, 0 para sem atividade) devido a lógica invertida
	if (queda_aux == 1) {
		queda_aux -= 1;
	} else queda_aux += 1;

	// O primeiro dado é '@' para identificar inicio da sequência e o último '#' para identificar fim da sequência
	// Os números 1, 2 e 3 presentes no vetor identificam o sensor
	char vet[] = {'@', 1, presenca_data, 2, queda_dataD , 3, (BPM/100)%10, (BPM/10)%10, BPM%10, '#'}; //Armazena os valores em um vetor padronizado para transmissão

	
	if(millis() > (timer + 1000)) {
		for (i = 0; i < 10; i++) {
			Serial.write(vet[i]);
		}
		timer = millis();
		presenca_aux = 0; 
		queda_aux = 1; 
	}
}