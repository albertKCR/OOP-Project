#include <Wire.h>
#include <RTClib.h>
#include <Thermistor.h>
#include <LiquidCrystal.h>

class luz{ // criação da classe luz que será responsável por ligar a led de diferentes maneiras
  private:
    int r,g,bl;
    public:
      luz(int a, int b, int c){ //no construtor será definido o pino de cada cor da led e também serão definidor como output
      r = a;
      g = b;
      bl = c;
      pinMode(r, OUTPUT);
      pinMode(g, OUTPUT);
      pinMode(bl, OUTPUT);
      digitalWrite(r, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(bl, HIGH);
      }
      void cor(int v, int ve, int az){
      digitalWrite(r, v);
      digitalWrite(g, ve);
      digitalWrite(bl, az);
      }
  };
class sensorSom: public luz{ //criação da classe sensorSom que herda luz, nessa clase haverá funções que farão a led interagir com o som
  private:
    int sensors;
    bool ledSom;
  public:
    sensorSom(int a, int b, int c, int pinS): luz(a,b,c){
      pinMode(16, INPUT);
      sensors=16;
    }
    void piscarRoxo(){
      if(digitalRead(sensors)==HIGH){
        ledSom=!ledSom; 
      }
      cor(ledSom,HIGH, ledSom);
    }
    void piscarAzul(){
      if(digitalRead(sensors)==HIGH){
        cor(HIGH,HIGH,LOW);  
      }
      else cor(HIGH, HIGH, HIGH);
    }
    void piscarVermelho(){
      if(digitalRead(sensors)==HIGH){
        cor(LOW,HIGH,HIGH);  
      }
      else cor(HIGH, HIGH, HIGH);
    }
  };

class sensorLuz: public luz{//criação da classe sensorLuz que herda luz, nessa classe faz o uso do sensor de movimento para acender a led
  private:
    int pinPIR;
  public:
      sensorLuz(int a,int  b,int c, int pin): luz(a,b,c){
      pinPIR = pin;
      pinMode(pinPIR, INPUT);
    }  
  void led(){
    if (digitalRead(pinPIR) == HIGH){
      cor(0,0,0);
      delay(7000);
    }
    else cor(255,255,255);
    }
};

class temperatura: public luz{ //classe temperatura herda luz, nessa classe terá as funções de retornar a temperatura (obtida atraves do termistor) e também um modo de led que acomapnha a temperatura
  private:
    int t, pino;
    Thermistor sensor = Thermistor(pino);
  public:
    temperatura(int a, int b, int c, int pin) : luz (a, b, c){
      pino=pin;
      sensor = Thermistor(pino);
      pinMode(pin, INPUT);
    }
  int getTemperatura(){
    t= sensor.getTemp();
  return t;
  } 
    void temperaturaCor(){
      t= sensor.getTemp();
      
      if (t>=5 && t<10){
       cor(HIGH, HIGH, LOW);
       Serial.println(t);
      }
      else if (t>=10 && t<20){
       cor(LOW, HIGH, LOW);
       Serial.println(t);
      }
      else if (t>=20 && t<25){
       cor(LOW, LOW, HIGH);
       Serial.println(t);
      }
      else if (t>=25){
       cor(LOW, HIGH, HIGH);
       Serial.println(t);
      }     
    }
};
//classe tela que herda sensorLuz, temperatura e sensorSom, nessa classe tem o procedimento menu que será basicamente a exibição do programa na tela lcd
//na tela lcd será possivel definir o alarme atraves do procedimento setAlarme(), definir a hora atual através do procedimento setHora() e também escolher o modo de led desejável
class tela: public sensorLuz, temperatura, sensorSom{
private:
    luz l1;
    int c1;
    int c2;
    RTC_DS1307 rtc;
    LiquidCrystal lcd = LiquidCrystal(tela1, tela2, tela3, tela4, tela5, tela6);
    int tela1, tela2, tela3, tela4, tela5, tela6;
    int botao1, botao2, botao3, buzzerpin;
    int hhours;
    int hmins;
    int ahours = 0;
    int amins = 0;
  public:
  // no construtor é recebido como parâmetro todos os pinos digitais e analogicos a serem usados para os botoes, buzzerm, led e outros componentes
  tela(int a, int b, int c, int pin,int t1,int t2,int t3,int t4,int t5,int t6, int pinT, int pinSom, int b1, int b2, int b3, int buzz):l1(a,b,c),sensorLuz(a,b,c,pin), temperatura(a,b,c,pinT), sensorSom(a,b,c,pinSom){
  botao1=b1;
  botao2=b2;
  botao3=b3;
  buzzerpin=buzz;
  tela1=t1;
  tela2=t2;
  tela3=t3;
  tela4=t4;
  tela5=t5;
  tela6=t6;
  lcd = LiquidCrystal(tela1, tela2, tela3, tela4, tela5, tela6);
  if (! rtc.begin()) {
   Serial.println("RTC NAO INICIALIZADO");
    while (1); 
 }
  c1=0;
  c2=0;
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(buzz, OUTPUT);
  }
  // c1 e c2 são contadores que definem o que aparece na tela lcd
  void menu(){
  if((hhours==ahours) && (hmins==amins)) buzzer();
  if (c1==0){
  lcd.setCursor(6, 0);
  horaAtual();
  lcd.setCursor(6, 1);
  lcd.print(getTemperatura());
  lcd.setCursor(8,1);
  lcd.print((char)223);
  lcd.print("C");
  if (c2==3){led();}
  delay(200);
    if(digitalRead(botao1)==HIGH) {
      c1++;
      lcd.clear();
      delay(200);
    }
  }
  else if(c1==1){
  setHora();
   if(digitalRead(botao1)==HIGH){
      c1++;
      lcd.clear();
    }
  }
  else if(c1==2){
    setAlarme();
    if (c2==3){led();}
    delay(200);
    if(digitalRead(botao1)==HIGH){
      c1++;
      lcd.clear();
      delay(200);
    }
  }
  else if(c1==3){
  lcd.setCursor(2, 0);
  lcd.print("Modos de Led");
  l1.cor(HIGH,HIGH,HIGH);
    if((digitalRead(botao2)==HIGH) && (c2==0)){
    c2++;
      lcd.clear();
      delay(200);
    }
    if((c2==1) && (c1==3)){
      lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Sensor de Som");
        piscarRoxo();
       // piscarAzul();
       // piscarVermelho();
      if(digitalRead(botao2)==HIGH){
        c2++;
        lcd.clear();
        delay(200);
      }}
      else if((c2==2) && (c1==3)){
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Luz Branca");
        l1.cor(LOW,LOW,LOW);
        delay(200);
        if(digitalRead(botao2)==HIGH){
        c2++;
          lcd.clear();
          delay(200);
        }
      }
      else if((c2==3) && (c1==3)){
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Sensor PIR");
        led();
        delay(200);
        if(digitalRead(botao2)==HIGH){
        c2++;
          lcd.clear();
          delay(200);
        }
        }
      else if((c2==4) && (c1==3)){
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Temperatura");
        temperaturaCor();
        delay(200);
        if(digitalRead(botao2)==HIGH){
        c2=0;
          lcd.clear();
          delay(200);
        }
    }
    if(digitalRead(botao1)==HIGH){
      c1=0;
      lcd.clear();
    }}
  }
  void horaAtual(){
    DateTime agora = rtc.now();
    hhours = agora.hour();
    hmins = agora.minute();
    lcd.setCursor(5,0);
    if(hhours<10){
      lcd.print("0");
      lcd.print(hhours,DEC);
    }else lcd.print(hhours,DEC);
    lcd.print(":");
    if(hmins<10){
      lcd.print("0");
      lcd.print(hmins, DEC);
     }else lcd.print(hmins, DEC);
 
  }
  void setHora(){
     DateTime agora = rtc.now();
   lcd.setCursor(6,0);
    lcd.print("Hora");
    lcd.setCursor(6,1);
    if(digitalRead(botao3)==HIGH)
    {
     hmins++;
     if(hmins>59){
      hhours++;
      hmins =0;
      }
    }
    if(digitalRead(botao2)==HIGH)
    {
     hhours++;
     if(hhours>23){
      hhours=0;
      }
    }
    lcd.setCursor(6,1);
    if(hhours < 10)
  {
    lcd.print("0");
    lcd.print(hhours);
  }
  else
  {
    lcd.print(hhours);
  }
   lcd.print(":");
    
  if (hmins < 10)
  {
    lcd.print("0");
      lcd.print(hmins);
  }
  else
  {
      lcd.print(hmins);
  }
  rtc.adjust(DateTime(2021, 1, 1, hhours, hmins, 00));
  delay(100);
  }
   void setAlarme()
  {
    lcd.setCursor(6,0);
    lcd.print("Alarme");
    lcd.setCursor(6,1);
    if(digitalRead(botao3)==HIGH)
    {
     amins++; 
    }
    if(digitalRead(botao2)==HIGH)
    {
     ahours++; 
    }
    lcd.setCursor(6,1);
    if(ahours < 10)
  {
    lcd.print("0");
    lcd.print(ahours);
  }
  else
  {
    lcd.print(ahours);
  }
   lcd.print(":");
    
  if (amins < 10)
  {
    lcd.print("0");
      lcd.print(amins);
  }
  else
  {
      lcd.print(amins);
  } 
   if(amins > 59)
     {
      ahours++;
      amins = 0;
     }
    if(ahours > 23)
     {
      ahours = 0; 
     }
  }
  void buzzer(){
        l1.cor(LOW,LOW,LOW);
        digitalWrite(buzzerpin, HIGH);
        delay(500);
        digitalWrite(buzzerpin, LOW);
        delay(500);
        digitalWrite(buzzerpin, HIGH);
        delay(500);
        digitalWrite(buzzerpin, LOW);
        delay(5000);
        digitalWrite(buzzerpin, LOW);
        l1.cor(HIGH,HIGH,HIGH);
  }
};

// definição do objeto t1 da classe tela, que passa por parametro todas as entradas e saidas digitais do programa
tela t1(2, 3, 4, 5, 11, 10, 9, 8, 7, 6, 1, 16, 12, 13, 0, 14);

void setup()
{
 Serial.begin(9600);
  delay(500);
}

void loop()
{
  t1.menu();
  
  
  delay(30);
}
