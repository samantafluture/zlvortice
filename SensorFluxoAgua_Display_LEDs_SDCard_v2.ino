// Carregar bibliotecas
#include <LiquidCrystal.h>
#include <SD.h>

// Pinagem - display LCD
#define Luz_Fundo   A5
LiquidCrystal lcd(8, 9, 5, 6, 3, 7);

// Pinagem - LEDs
int led_verde = A0; // GND com R 220 omhs no GND; VCC no A0
int led_amarelo = A1; // GND com R 220 omhs no GND; VCC no A1
int led_vermelho = A2; // GND com R 220 omhs no GND; VCC no A2

// Pinagem - SD card
// Pins 4 (SD_CS); 11 (SD_DI); 12 (SD_DO); 13 (SD_CLK) e 10
// o CS pin (10 no Arduino Uno) deve ser deixado como output e sem nada
// para que a biblioteca sd funcione
const int chipSelect = 4;
File gFile;
int number = 0;

// Pinagem - fluxo agua
// pin 2 (data); 5v (vcc); gnd (gnd) 
float vazao;          //Variável para armazenar o valor em L/min
float media = 0;      //Variável para fazer a média
int contaPulso;       //Variável para a quantidade de pulsos
int i = 0;            //Variável para segundos
int Min = 00;         //Variável para minutos
float Litros = 0;     //Variável para Quantidade de agua
float MiliLitros = 0; //Variavel para Conversão

void setup()
{ 
  Serial.begin(9600); //Inicia a serial com um baud rate de 9600
  
  pinMode(2, INPUT); // Pinagem - fluxo agua
  attachInterrupt(0, incpulso, RISING); //Configura o pino 2(Interrupção 0) para trabalhar como interrupção
//  Serial.println("\n\nInicio\n\n"); //Imprime Inicio na serial
  
  lcd.begin(16,2); //Inicializa LCD
  lcd.clear(); //Limpa o LCD
  pinMode(Luz_Fundo,OUTPUT); //define o pino como saída
  digitalWrite(Luz_Fundo,HIGH); // Liga a luz do display
  
  pinMode(led_verde,OUTPUT);
  pinMode(led_amarelo,OUTPUT);
  pinMode(led_vermelho,OUTPUT);
  
  Serial.print("Iniciando SD card...");
  pinMode(SS, OUTPUT); // ou pino 10
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("SD card falhou!");    
      lcd.setCursor(0,1);
      lcd.print("SD card falhou!");
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


void loop ()
{
  contaPulso = 0;   //Zera a variável para contar os giros por segundos
  sei();      //Habilita interrupção
  delay (1000); //Aguarda 1 segundo
  cli();      //Desabilita interrupção
  
  vazao = contaPulso / 5.5; //Converte para L/min
  media=media+vazao; //Soma a vazão para o calculo da media
  i++;
  
  Serial.print(vazao); //Imprime na serial o valor da vazão
  Serial.print(" L/min - "); //Imprime L/min
  Serial.print(i); //Imprime a contagem i (segundos)
  Serial.println("s"); //Imprime s indicando que está em segundos
  
  // Coletar dados no sd card
  gFile.print(number);  // numeracao - coletando 1 dado por segundo
  gFile.print(", "); // virgula para facilitar geracao de planilhas
  gFile.print(vazao); // coletando dados de vazao de agua por 
  gFile.print(", "); // virgula para facilitar geracao de planilhas
  gFile.print(Min); // segundos/minutos passados
  gFile.print(", "); // virgula para facilitar geracao de planilhas
  gFile.print(Litros); // quantidade de litros passada pelo sensor
  
  lcd.setCursor(0, 0);
  lcd.print(vazao);        //Escreve no display o valor da vazão
  lcd.print(" L/min ");    //Escreve L/min
  lcd.setCursor(0, 1);
  lcd.print(Min);
  lcd.print(":");         //Escreve :
  lcd.print(i);           //Escreve a contagem i (segundos)
  lcd.print("Min   ");    //Escreve :
  MiliLitros = vazao / 60;
  Litros = Litros + MiliLitros;
  lcd.print(Litros);
  lcd.print("L  ");
  
  // outuputs de acordo com vazao de agua
  if (vazao >20 && vazao <30)
  {
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_vermelho, HIGH);
  }
  if (vazao >10 && vazao <20)
  {
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo, HIGH);
    digitalWrite(led_vermelho, LOW);
  }
  if (vazao > 1 && vazao <10)
  {
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_vermelho, LOW);
  }
  if (vazao > 0 && vazao <1)
  {
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_vermelho, LOW);
  }
  
   if(i==60)
  {
    Min++;
    lcd.print(Min);

    if (Min >= 60)
    {
      Min = 0;
    }
    media = media / 60;                    //faz a média
    Serial.print("\nMedia por minuto = "); //Imprime a frase Media por minuto =
    Serial.print(media);                   //Imprime o valor da media
    Serial.println(" L/min - ");           //Imprime L/min
    media = 0;                             //Zera a variável media para uma nova contagem
    i = 0;                                 //Zera a variável i para uma nova contagem
  }
  
}
 
 
void incpulso ()
{ 
  contaPulso++; //Incrementa a variável de contagem dos pulsos
} 

