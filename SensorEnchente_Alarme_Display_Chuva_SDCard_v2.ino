// Carregar bibliotecas
#include <Ultrasonic.h>
#include <LiquidCrystal.h>
#include <SD.h>
#include <Wire.h> 

// Pinagem - ultrassonico
#define pino_trigger A1
#define pino_echo A2

// Pinagem - alarme (potenciometro entre GND do speaker e SIGNAL do pot; VCC e GND do pot normais)
#define pino_alarme1 A0
//#define pino_alarme2 A3

// Pinagem - chuva
int pino_chuva = A5; //Pino ligado ao A5 do sensor
int Porcento = 0;
int chuva; // Variavel do sensor

// Pinagem - display LCD
#define Luz_Fundo   7
LiquidCrystal lcd(8, 9, 5, 6, 3, 2);

// Pinagem - SD card
// D4: SD_CS; D11: SD_DI; D12: SD_DO; D13: SD_CLK
// o CS pin (10 no Arduino Uno) deve ser deixado como output e sem nada
// para que a biblioteca sd funcione
const int chipSelect = 4;
File gFile;
int number = 0;

// Definir limite da distancia entre sensor e agua
// A partir deste valor, o alarme sera acionado
const int threshold = 50;

// Inicializar o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

void setup()
{
  Serial.begin(9600); //Inicia a serial com um baud rate de 9600
  
//  pinMode(pino_chuva, INPUT);
  
  lcd.begin(16,2); //Inicializa LCD
  lcd.clear(); //Limpa o LCD
  pinMode(Luz_Fundo,OUTPUT); //define o pino como saída
  digitalWrite(Luz_Fundo,HIGH); // Liga a luz do display
  
  Serial.print("Iniciando SDcard...");
  pinMode(SS, OUTPUT); // ou pino 10
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("SDcard falhou!");    
      lcd.setCursor(0,1);
      lcd.print("(SDcard falhou!)");
    while (1) ;
  }
    gFile = SD.open("dados.txt", FILE_WRITE);
  if (! gFile) 
  {
    Serial.println("Erro no arquivo de dados!"); 
      lcd.setCursor(0,1);
      lcd.print("(Arquivo falhou!)"); 
     while (1) ;
  }
    Serial.println("Coleta de dados iniciada.");
   
}

void loop()
{
  // Ler as informacoes do sensor ultrassonico em cm
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  // Acionar alarme dependendo da distancia medida
  int sensorReading = cmMsec;
  int thisPitch = map(sensorReading, 00, 4000, 120, 1500);
   if (cmMsec > threshold) {
     
     // Toque a nota correspondente
       tone(pino_alarme1, thisPitch, 500);
       //tone(pino_alarme2, thisPitch, 500);
     }

  // Ler valores de chuva
  chuva = analogRead(pino_chuva);
  Porcento = map(chuva, 1023, 0, 0, 100);
   
  // Exibir informacoes no serial monitor
  Serial.print("Distancia (cm): ");
  Serial.print(cmMsec);
  Serial.print("Chuva (%): ");
  Serial.print(Porcento);
  
  // Exibir informacoes no display lcd
  lcd.setCursor(0,0);
  
  lcd.print("Dist:");
  lcd.print(" ");
  lcd.setCursor(6,0);
  lcd.print(cmMsec);
  lcd.setCursor(14,0);
  lcd.print("cm");
  
  lcd.setCursor(0,1);
  lcd.print("Chuva: ");
  lcd.print(" ");
  lcd.setCursor(7,1);
  lcd.print(Porcento);
  lcd.println("%");
  
  if (Porcento >= 20)
  {
    lcd.setCursor(10, 1);
    lcd.println("sim!");
    delay(1000);
    lcd.clear();
  }
 
  else
  {
    lcd.setCursor(10, 1);
    lcd.println("nao!");
    delay(1000);
    lcd.clear();
  }
  
  /*
  //Verifica qual é o valor e
        //imprime na Serial o valor e Status 
        if(chuva > 800){
          //Seco
          Serial.println("seco");
          lcd.setCursor(11,1);
          lcd.print(" seco");
          
        }else if(chuva <=800 && chuva > 500 ){
          //Leve Chuva
          Serial.println("fina");
          lcd.setCursor(11,1);
          lcd.print(" fina");//--
        
        }else if (chuva <= 500){
               //Chuva Forte
                Serial.println("forte");
                lcd.setCursor(11,1);
                lcd.print(" forte");//--
        } 
        */
        
    // Coletar dados no sd card
    gFile.print(number);  // numeracao - coletando 1 dado por segundo
    gFile.print(", "); // virgula para facilitar geracao de planilhas
    gFile.print(cmMsec); // coletando dados de distancia
    gFile.print(", "); // virgula para facilitar geracao de planilhas
    gFile.print(Porcento); // coletando dados de chuva 

  gFile.flush();
  delay(1000);
  number++; 
}
