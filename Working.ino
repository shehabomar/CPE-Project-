// Include Libraries#
#include <M5Core2.h>
#include <WiFi.h>
#include <EMailSender.h>

// Pin Definitions
#define MQ3 35



const char* ssid = "Room19";
const char* password = "passpass2";

float sensorValue;
float sensor_volt;
float RS_gas;
float R0;
float ratio;
float BAC= 0.0F;
float Per_Alc= 0.0F;
int R2 = 2000;

const char* smtp_server = "smtp.gmail.com";
const int smtp_port = 587;
const char* email_username = "m5stackcore2nyu@gmail.com";
const char* email_password = "arumvwnnhdypafll";
EMailSender emailSend(email_username, email_password);

const char* recipient = "mkassem582@gmail.com";
void start_prog(){
  M5.Lcd.setTextSize(2.5);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.println("Press any Button to start the Program");
  while (true) {
    // wait for push button
    if(M5.BtnA.wasPressed()||M5.BtnB.wasPressed()|| M5.BtnA.wasPressed()) {
      break;
    }
    M5.update();      
  }
}
void setup() {
  M5.begin();
  // Connect to WiFi
  start_prog();
  M5.Lcd.clear();
  
  M5.Lcd.print("Connecting to WiFi: ");
  M5.Lcd.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
    WiFi.begin(ssid, password);
  }
  M5.Lcd.println("Connected to WiFi!");
  M5.Lcd.fillScreen(WHITE);
}

void loop() {
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 10);
  M5.Lcd.printf("M5Stack Alcohol detector");
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setCursor(90, 50);
  M5.Lcd.print("Level ");
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(110, 110);

  sensorValue = analogRead(MQ3);
  sensor_volt = sensorValue / 1024 * 5.0;
  RS_gas = ((5.0 * R2) / sensor_volt) - R2;
  R0 = 16000;
  ratio = RS_gas / R0; // ratio = RS/R0
  double x = 0.4 * ratio;
  BAC = pow(x, -1.431);
  Per_Alc = BAC * 0.0001;
  M5.Lcd.fillRect(110, 110, 100,30, WHITE); 
  M5.Lcd.printf("%5.2f",Per_Alc);
  if (isnan(Per_Alc)) {
    M5.Lcd.setCursor(110, 110);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Waiting");
    M5.Lcd.fillCircle(170, 180, 30, YELLOW); 
  }
   
  if(Per_Alc > 0.15){
    M5.Lcd.fillCircle(170, 180, 30, RED);
    // Send the email
    EMailSender::EMailMessage message;
    message.subject = "A drunk driver is detected !";
    message.message = "I am sending this email to inform you that the driver whose car license plate is C152 is probably drunk";
    EMailSender::Response resp = emailSend.send("mkassem582@gmail.com",message);
    delay(3000);
  }
  else{
    M5.Lcd.fillCircle(170, 180, 30, GREEN);
  }
  
  delay(2000);

}
