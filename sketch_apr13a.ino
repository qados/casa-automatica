#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// definições para o keypad e o servo motor
LiquidCrystal_I2C lcd(0x26, 16, 2);
Servo meuServo;

const byte numRows = 4;
const byte numCols = 4;

char keymap[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {12, 11, 10, 9};
byte colPins[numCols] = {8, 7, 6, 5};

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins,
                         numRows, numCols);

// definições para o sensor de chuva
int pino_a = A5;
int pino_saida = 13;
int limite = 200;

void setup()
{
  // setup do lcd e servo
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Casa");
  delay(1500);
  lcd.clear();
  
  meuServo.attach(3);
  meuServo.write(0);
  
  lcd.print("1: Abrir");
  lcd.setCursor(0, 1);
  lcd.print("2: Fechar");

  // setup do sensor de chuva
  pinMode(pino_saida, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  char tecla = myKeypad.getKey();
  int val_a = analogRead(pino_a);

  Serial.print("Valor analogico: ");
  Serial.println(val_a);
  
  if (tecla != NO_KEY) {
    lcd.clear();
    lcd.print("Tecla: ");
    lcd.print(tecla);
    
    if (tecla == '1') {
      meuServo.write(180); // abre
      lcd.setCursor(0, 1);
      lcd.print("Abrindo Portao");
    } else if (tecla == '2') {
      meuServo.write(0); // fecha
      lcd.setCursor(0, 1);
      lcd.print("Fechando...");
    }
  }

  if (val_a < limite) {
    digitalWrite(pino_saida, HIGH);
    Serial.println("Chuva suficiente - LIGADO");
  } else {
    digitalWrite(pino_saida, LOW);
    Serial.println("Sem chuva suficiente");
  }
}
