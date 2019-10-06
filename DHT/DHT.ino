#include <DHT.h>" // Inclui a biblioteca no seu código

DHT dht(A0, DHT22); // Cria um objeto da classe dht

void setup()
{
  Serial.begin(9600); // Inicializa serial com taxa de transmissão de 9600 bauds
  dht.begin();
}

void loop(){
  Serial.println(dht.readTemperature()); // chama método de leitura da classe dht,
  // com o pino de transmissão de dados ligado no pino A1
  delay(5000);
}
