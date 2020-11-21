#include <Ethernet.h>
#include <Ultrasonic.h>


#define trigPin 7 // Declara constante trigPin
#define echoPin 8 // Declara constante como echoPin
#define tempo 10 // Declara constante de tempo
 
int frequencia = 0; // Inicializa variável de frequência em 0
int pinbuzzer = 9; // Inicializa buzzer no pino 9
int situacao;

Ultrasonic ultrasonic(trigPin,echoPin); // Inicializa os pinos do sensor

int duration, distance; // Inicializa variável de distância e tempo



//Definicoes de IP, mascara de rede e gateway
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,88);          //Define o endereco IP
IPAddress gateway(192,168,0,1);     //Define o gateway
IPAddress subnet(255,255,255,0); //Define a máscara de rede
 
//Inicializa o servidor web na porta 80
  EthernetServer server(80);


void setup() {

  //Inicializa a interface de rede
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pinbuzzer, OUTPUT);
}
 
void loop() {

  //Aguarda conexao do browser
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // uma solicitação http termina com uma linha em branco
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        // se você chegou ao fim da linha (recebeu uma nova linha
        // caractere) e a linha está em branco, a solicitação http terminou,
        // então você pode enviar uma resposta
        
        if (c == 'n' && currentLineIsBlank) {
          // enviar um cabeçalho de resposta http padrão
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 2"); //Recarrega a pagina a cada 2seg
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          //Configura o texto e imprime o titulo no browser
          client.print("<font color=#0000FF><b><u>");
          client.print("Envio de informacoes pela rede utilizando Arduino e um Sensor Ultrasonico !!");
          client.print("</u></b></font>");
          client.println("<br />");
          client.println("<br />");
          
         
          client.print("Status do alarme : ");
          client.print("<b>");
          client.print(situacao);
          client.println("</b>");
          client.print("  (0 = Alarme desligado, 1 = Alarme ligado)");
          client.println("<br />");
          
          //Mostra as informacoes lidas pelo sensor ultrasonico
          client.print("Distancia sendo capturada pelo sensor ultrasonico : ");
          client.print("<b>");
          client.print(distance);
          client.print(" cm");
          client.println("</b></html>");
          break;
        }
        if (c == 'n') {
          // você está começando uma nova linha
          currentLineIsBlank = true;
        } 
        else if (c != 'r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // dar tempo ao navegador para receber os dados
    
    delay(1);
    // fecha conexao
    
    client.stop();
    }

  // logica do projeto
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (ultrasonic.Ranging(CM)); // Variavel global recebe o valor da distancia medida

  
  char comando = Serial.read();
   if (comando == 'a') {
      situacao = 1;
      Serial.println("Alarme ativado!");
    }
   if (comando == 'd') {
      situacao = 0;
      Serial.println("Alarme desativado!");
    }

     if(situacao == 1){
      if (distance >= 40){
        Serial.println("Local protegido!");
        digitalWrite(pinbuzzer, LOW);
      }else{
        Serial.println("Pessoa Detectada!!");
        Serial.print("Distância= ");
        Serial.print(distance);
      
          for (frequencia = 150; frequencia < 1800; frequencia += 1) { // Tone que produz sirene de polícia
            tone(pinbuzzer, frequencia, tempo);
            delay(3);
          }
        
          for (frequencia = 1800; frequencia > 150; frequencia -= 1) { // Tone que produz sirene de polícia
            tone(pinbuzzer, frequencia, tempo);
            delay(3);
          }
       }
     }
}
