/**BIBLIOTECAS**/
#include <LiquidCrystal.h>                                                //Biblioteca do display LCD
#include <DS3231.h>                                                       //Biblioteca do RTC
#include "DHT.h"                                                          //Biblioteca do Sensor de Umidade e Temperatura
#include <Servo.h>                                                        //Biblioteca do Servo Motor

/**CONSTANTES**/
#define TEMPO_ATUALIZACAO 2000                                            //Constante que define o tempo do delay entre cada iteração
//Variávieis auxiliares
int porta_rele = 6;                                                      //Porta digital que o relé está plugado
float temperatura;                                                        //Variável que armazena a temperatura em cada loop
float umidade;                                                            //Variável que armazena a umidade em cada loop

/**PARÂMENTROS DO SENSOR DE TEMPERATURA**/
DHT dht(A0, DHT22);                                                 //Variável que define a porta e o modelo do sensor (no caso, as constantes definidas antes)

/**PARÂMETROS DO RTC**/
DS3231 rtc;                                                               //Variável do RTC
RTCDateTime dataehora;                                                    //Variável do objeto a ser instanciado com os valores do RTC

/**PARÂMETROS DO DISPLAY LCD**/
LiquidCrystal lcd (12, 11, 5, 4, 3, 2);                                    //Iniciação do objeto do Display LCD e sua respectiva pinagem digital
int porta_chave = 8;
int cont = 1;                                                             //Variável auxiliar a ser usada no controle de movimento do Servo Motor
int cntrIdaVolta = 0;                                                     //Variável auxiliar a ser usada no controle de movimento do Servo Motor para ida e volta
int contdias = 0;                                                         //Variável que armazena a quantidade de tempo que que se passou desde a inicialização do Arduíno
int dias;                                                                 //Variável auxiliar que armazena o valor do dia atual
Servo myservo;                                                            //Variável que representa o servo motor



//**SETUP**//
void setup () {
  pinMode(porta_chave, INPUT_PULLUP); 
  Serial.begin(9600);                                                     //Inicio do Serial
  lcd.begin(16, 2);                                                       //Inicialização do Display LCD com 16 x 2 caracteres
  dht.begin();                                                            //Inicialização do Sensor de Umidade e Temperatura
  rtc.begin();                                                            //Inicialização do RTC
  rtc.setDateTime(__DATE__, __TIME__);                                    //Configurando os valores inicais para a data e horários atuais do computador
  RTCDateTime dataehora1 = rtc.getDateTime();                             //Instanciando a variável auxiliar da contagem de dias para o valor atual do RTC
  dias = dataehora1.minute;                                               //Usando a variável auxiliar anterior, inicia-se a contagem de dias do Arduíno ligado(MUDAR DE MINUTE PARA day)
  
}

void ida(){
  myservo.attach(7);
  myservo.write(0);
  delay(1600);
  myservo.detach();
}
void volta(){
  myservo.attach(7);
  myservo.write(240);
  delay(1600);
  myservo.detach();
}

//**LOOP**//
void loop () {
 if(digitalRead(porta_chave) == LOW && millis() < 10000){ //SE A LEITURA DO PINO FOR IGUAL A LOW, FAZ
      ida();
  }
  dataehora = rtc.getDateTime();                                         //Atribuindo valores instantâneos de data e hora à instância dataehora
  temperatura = dht.readTemperature();                                   //Armazenando a temperatura do loop atual
  umidade = dht.readHumidity();                                          //Armazenando a umidade do loop atual
  lcd.clear();                                                           //Apaga o conteúdo exibido no display
  lcd.setCursor(0, 0);                                                   //posiciona o cursor na coordenada especificada
  lcd.print("T=");                                                       //exibir no LCD
  lcd.print(temperatura);                                                //exibir no LCD (Valor da temperatura)
  lcd.print("C");                                                        //exibir no LCD
  lcd.setCursor(0, 1);                                                   //posiciona o cursor na coordenada especificada
  lcd.print("U=");                                                       //exibir no LCD
  lcd.print(umidade);                                                    //exibir no LCD (Valor da umidade)
  lcd.print("%");                                                        //exibir no LCD
  lcd.setCursor(12, 0);                                                  //posiciona o cursor na coordenada especificada
  lcd.print("D=");                                                       //exibir no LCD
  lcd.print(contdias);                                                   //exibir no LCD (Quantidade de dias desde o inicio)
  
  //Condicional de incremento dos dias
  if (dataehora.day != dias) {
    contdias++;                                                          //Incrementando a unidade tempo monitorada(Quantidade de Dias desde o início do funcionamento)
    dias = dataehora.day;                                                //A variável que armazena o 'controle' da unidade de tempo é atualizada no instante da mudança
  }

  /*
    Condicional de monitoramento de acionamento do Servo Motor.
    Como o motor é pra ser acionado a cada 2 horas, verifica se a quantidade de horas é divisível por 2,
    se for E a variável de controle estiver disponível, o motor é acionado e a variável de controle é alterada
    para que isso só seja feito uma vez por ciclo de tempo
  */
  if (dataehora.hour % 3 == 0 && cont == 1 && contdias <= 19 && cntrIdaVolta == 0 && contdias > 2) {
    volta();                                                               //A função que aciona o motor é chamada para ir
    cont++;                                                              //A variável de controle é alterada para que no proximo loop, não entre nessa condição
    cntrIdaVolta = 1;
  }
  /*
    Condicional de monitoramento de acionamento do Servo Motor.
    Como o motor é pra ser acionado a cada 2 horas, verifica se a quantidade de horas é divisível por 2,
    se for E a variável de controle estiver disponível, o motor é acionado e a variável de controle é alterada
    para que isso só seja feito uma vez por ciclo de tempo
  */
  if (dataehora.hour % 3 == 0 && cont == 1 && contdias <= 19 && cntrIdaVolta == 1 && contdias > 2) {
    ida();                                                               //A função que aciona o motor é chamada para ir
    cont++;                                                              //A variável de controle é alterada para que no proximo loop, não entre nessa condição7
    cntrIdaVolta = 0;
  }
  /*
    Condicional de monitoramento de acionamento do Servo Motor.
    Quando a hora for ímpar, a variável de controle volta ao estado de permissão da estrutura anterior
  */
  if (dataehora.hour % 3 != 0) {                                         //O valor inteiro representa o tempo que o motor vai voltar poder ser utilizada
    cont = 1;                                                            //A variável de controle é alterada para que seja permissinado novamente o acionamento do motor
  }
  /*
    Condicional que faz o relé desligar a lâmpada caso a temperatura seja maior que a estipulada
  */
  if (temperatura >= 37.8) {
    digitalWrite(porta_rele, LOW);                                       //Comando que desliga a porta do relé
  }
  /*
    Condicional que faz o relé ligar a lâmpada caso a temperatura seja menor que a estipulada
  */
  else if (temperatura <= 37.3) {
    digitalWrite(porta_rele, HIGH);                                      //Comando que liga a porta do relé
  }
  delay(TEMPO_ATUALIZACAO);                                              //Tempo de atualização do Loop

}
