
// Include Libraries#
#include <M5Core2.h>
#include <WiFi.h>
#include <EMailSender.h>

// Pin Definitions
#define MQ3 35
#define TOLERANCE 0.15

//the network to connect to
const char* ssid = "Room19";
const char* password = "passpass2";

//variables that we used to calibrate the values of the MQ3 sensor

float sensorValue;
float sensor_volt;
float RS_gas;
float R0;
float ratio;
float BAC= 0.0F;
float Per_Alc= 0.0F;
int R2 = 2000;

//teh server that we will send and receive email messages through it and its port
const char* smtp_server = "smtp.gmail.com";
const int smtp_port = 587;

//the email of the m5stack and its password
const char* email_username = "your_email";
const char* email_password = "your_password";

//using the library EMailSender to assign email_username as the sender
EMailSender emailSend(email_username, email_password);

//the email of the recipient
const char* recipient = "recepientEmail";

//void function to ask the user to press any button to start the process
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

//the setup of the m5stack
void setup() {

  //begining the process
  M5.begin();
  
  //calling the function start_prog to display the intial 
  start_prog();
  
  //clearing what appears after the user presses a button
  M5.Lcd.clear();
  
  // Connect to WiFi
  M5.Lcd.print("Connecting to WiFi: ");
  M5.Lcd.print(ssid);
  WiFi.begin(ssid, password);
  
  //if the m5stack doesn't connect to wifi it will print dots
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
    Wifi.begin(ssid,password);
  }
  
  M5.Lcd.println("Connected to WiFi!");
  M5.Lcd.fillScreen(WHITE);
}

//the loop of the m5stack
void loop() {
  //the layout of the alcohol detector on the m5stack
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


  //calibration of the sensor and converting the values from ppt to gram/deciliter
  sensorValue = analogRead(MQ3);
  sensor_volt = sensorValue / 1024 * 5.0;
  RS_gas = ((5.0 * R2) / sensor_volt) - R2;
  R0 = 16000;
  ratio = RS_gas / R0; // ratio = RS/R0
  double x = 0.4 * ratio;
  BAC = pow(x, -1.431);
  Per_Alc = BAC * 0.0001;

  //a rectangele that controls the apperance of the level of alcohol 
  M5.Lcd.fillRect(110, 110, 100,30, WHITE); 
  
  M5.Lcd.printf("%5.2f",Per_Alc);

  //checking if the percent of alcohol is not defined by the sensor, the m5stack will show waiting
  if (isnan(Per_Alc)) {
    M5.Lcd.setCursor(110, 110);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Waiting");
    M5.Lcd.fillCircle(170, 180, 30, YELLOW); 
  }

   // if the percent of alcohol is greater than 0.15 then this person is considered drunk
  if(Per_Alc > TOLERANCE){
    M5.Lcd.fillCircle(170, 180, 30, RED);
    // Send the email
    EMailSender::EMailMessage message;
    message.subject = "A drunk driver is detected !";
    message.message = "I am sending this email to inform you that the driver whose car license plate is C152 is probably drunk";
    EMailSender::Response resp = emailSend.send("mkassem582@gmail.com",message);
    delay(3000);
  }
  //if the user is not drunk
  else{
    M5.Lcd.fillCircle(170, 180, 30, GREEN);
  }
  
  delay(2000);

}
