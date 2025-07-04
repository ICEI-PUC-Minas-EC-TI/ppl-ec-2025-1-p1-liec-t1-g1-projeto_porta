#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif
#define BUTTON_ENROLL  12
#define BUTTON_DELETE  14
#define BUZZER_PIN     15
#define SERVO_PIN      18
int valorRecebido;


Servo lockServo;
BluetoothSerial SerialBT;

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


bool enrolling = false;
bool deleting = false;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("porta");
  Serial.println("O dispositivo já pode ser pareado!");


  pinMode(BUTTON_ENROLL, INPUT);
  pinMode(BUTTON_DELETE, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);


  lockServo.attach(SERVO_PIN);
  lockServo.write(0);

  Serial.begin(9600);
  delay(100);

  mySerial.begin(57600, SERIAL_8N1, 16, 17); // RX, TX

  finger.begin(57600);
  delay(100);

  if (finger.verifyPassword()) {
    Serial.println("Leitor de digitais iniciado.");
  } else {
    Serial.println("Erro no leitor de digitais.");
    while (1);
  }


  finger.getTemplateCount();
  Serial.print("Templates cadastrados: ");
  Serial.println(finger.templateCount);
}


void loop() {
  if (SerialBT.available() > 0) {
    valorRecebido =(int)SerialBT.read();
    if(valorRecebido == 2){
    cadastrarDigital();
  }
  if(valorRecebido == 0){
  acessoPermitido();
  }
  if(valorRecebido == 1){
    Fechar();
  }
} 


  if (digitalRead(BUTTON_ENROLL) == 1) {
    cadastrarDigital();
  }


  if (digitalRead(BUTTON_DELETE) == 1) {
    deletarDigital();
  }


  if (finger.getImage() == FINGERPRINT_OK) {
    if (finger.image2Tz() == FINGERPRINT_OK) {
      if (finger.fingerSearch() == FINGERPRINT_OK) {
        acessoPermitido();
      } else {
        acessoNegado();
      }
    }
  }
}


void cadastrarDigital() {
  Serial.println("Cadastro iniciado. Coloque o dedo.");
  delay(1000);
  while (finger.getImage() != FINGERPRINT_OK);


  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Erro ao converter imagem.");
    return;
  }


  Serial.println("Remova e recoloque o dedo.");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Erro ao converter imagem 2.");
    return;
  }


  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Erro ao criar modelo.");
    return;
  }


  int id = finger.templateCount + 1;
  if (finger.storeModel(id) == FINGERPRINT_OK) {
    Serial.print("Digital cadastrada com ID ");
    Serial.println(id);
  } else {
    Serial.println("Falha ao armazenar.");
    
  }
}


void deletarDigital() {
  Serial.println("Deleção iniciada. Coloque o dedo.");
  delay(1000);
  while (finger.getImage() != FINGERPRINT_OK);


  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Erro ao ler digital.");
    return;
  }


  if (finger.fingerSearch() != FINGERPRINT_OK) {
    Serial.println("Digital não encontrada.");
    return;
  }


  int id = finger.fingerID;
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.print("Digital deletada com ID ");
    Serial.println(id);
  } else {
    Serial.println("Erro ao deletar.");
  }
}


void acessoPermitido() {
  Serial.println("Acesso permitido!");
  lockServo.write(90);
  delay(5000);  
}

void Fechar() {
  Serial.println("Fechando porta");
  lockServo.write(0);  
}


void acessoNegado() {
  Serial.println("Acesso negado!");

}