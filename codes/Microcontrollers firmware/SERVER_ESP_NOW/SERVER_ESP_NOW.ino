
#include <MFRC522.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Base64.h>
#include <mbedtls/aes.h>

//OUTPUT FOR WIRELESS  ALARM ZONES
#define SECURITY  12
String sec_call;

String  myData;
String room_no;
String id_for_del;
//για AES CRC
const unsigned char key1[] = "aaaaaaaaaaaaaaaa";//{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
const unsigned char key2[] = "bbbbbbbbbbbbbbbb";
const unsigned char key3[] = "cccccccccccccccc";
const unsigned char key4[] = "dddddddddddddddd";
const unsigned char key5[] = "eeeeeeeeeeeeeeee";
const unsigned char key6[] = "ffffffffffffffff";


unsigned char iv1[]  ={0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};
unsigned char iv2[]  ={0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
unsigned char iv3[]  ={0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x4,  0x43, 0x43, 0x43, 0x43, 0x43};
unsigned char iv4[]  ={0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44};
unsigned char iv5[]  ={0x45, 0x45, 0x45 ,0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45};
unsigned char iv6[]  ={0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46};
//SoftwareSerial mySerial(13, 15); // Orizoume mia nea UART (RX pin 2, TX pin 3)

//MAC ADRESS FROM ALL SENDERS
uint8_t MacAddress_sender1[6] ={0x40,0x4C,0xCA,0x40,0xFB,0x84};
uint8_t MacAddress_sender2[6] = {0x40,0x4C,0xCA,0x41,0x2B,0x58};
uint8_t MacAddress_sender3[6] = {0x40,0x4C,0xCA,0x41,0x29,0xB8};
uint8_t MacAddress_sender4[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
uint8_t MacAddress_sender5[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
uint8_t MacAddress_sender6[6] = {0x40,0x4C,0xCA,0x40,0xF6,0x60};
esp_now_peer_info_t peerInfo;


//δηλωση συναρτησεων
void splitString(String str);
void printHex(const char* label, const unsigned char* data, size_t length);
void connectToWiFi();
void aes_cbc_decrypt(const unsigned char * key, unsigned char* iv, const unsigned char* ciphertext, unsigned char* plaintext,int length);
void splitString(String str);
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t* incomingData, int len);
void OnDataSent(const uint8_t *mac_addr,esp_now_send_status_t status);

int outputPin=13;
int enablePin =18;
bool objectDetect;

void setup() {
  
  //esp_sleep_enable_timer_wakeup(1000000);


  Serial.begin(115200);

  while(!Serial);
  //mySerial.begin(9600);
  pinMode( LED_BUILTIN,OUTPUT);
  pinMode( SECURITY,OUTPUT);

  pinMode(outputPin,INPUT);
  pinMode(enablePin,OUTPUT);

  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(SECURITY, HIGH);
  
  WiFi.mode(WIFI_STA);
  connectToWiFi();

  //register callback function
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);


  memcpy(peerInfo.peer_addr, MacAddress_sender1,6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

  //Add peer1
  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.print("Faillled to add peer");
  }

  memcpy(peerInfo.peer_addr, MacAddress_sender2,6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

    //Add peer2
  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.print("Faillled to add peer");
  }

    memcpy(peerInfo.peer_addr, MacAddress_sender3,6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

    //Add peer3
  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.print("Faillled to add peer");
  }

  
}

void loop() {
        
  /*

     digitalWrite( enablePin, HIGH);     // Enable the internal 38kHz signal.
     delayMicroseconds( 210);                   // Wait 210µs (8 pulses of 38kHz).
     if( digitalRead( outputPin))        // If detector Output is HIGH,
     {
         objectDetect = false;           // then no object was detected;
     }
     else                                // but if the Output is LOW,
     {
         delayMicroseconds( 395);               // wait for another 15 pulses.
         if( digitalRead( outputPin))    // If the Output is now HIGH,
         {                               // then first Read was noise
             objectDetect = false;       // and no object was detected;
         }
         else                            // but if the Output is still LOW,
         {
             objectDetect = true;        // then an object was truly detected.
         }
     }
    // digitalWrite( enablePin, LOW);  
     Serial.print("detected  = ");
     Serial.println(objectDetect);
     delayMicroseconds( 263); 

    */
    if (Serial.available() > 0) {
         String   mess = Serial.readString();
         
         if(mess == "1"){
            //digitalWrite(SECURITY, LOW);
            sec_call="1";
            uint8_t sendData[mess.length() + 1];
            mess.getBytes(sendData, mess.length() + 1);
            esp_err_t result = esp_now_send(MacAddress_sender2,(uint8_t *) &sendData,sizeof(sendData));
          
         }
         else if(mess=="0"){
            //digitalWrite(SECURITY, HIGH);
             sec_call="0";
            uint8_t sendData[mess.length() + 1];
            mess.getBytes(sendData, mess.length() + 1);
            esp_err_t result = esp_now_send(MacAddress_sender2,(uint8_t *) &sendData,sizeof(sendData));
           
         }
         else{

           splitString(mess);
           //Serial.println(room_no);
           //Serial.println(id_for_del);

           if(room_no=="room1"){
              uint8_t sendData[id_for_del.length() + 1];
              id_for_del.getBytes(sendData, id_for_del.length() + 1);
              esp_err_t result = esp_now_send(MacAddress_sender1,(uint8_t *) &sendData,sizeof(sendData));
              Serial.println(room_no);
              Serial.println(id_for_del);
           }
           else if(room_no=="room2"){
              uint8_t sendData[id_for_del.length() + 1];
              id_for_del.getBytes(sendData, id_for_del.length() + 1);
              esp_err_t result = esp_now_send(MacAddress_sender2,(uint8_t *) &sendData,sizeof(sendData));
              
           }
           else if(room_no=="room3"){
              uint8_t sendData[id_for_del.length() + 1];
              id_for_del.getBytes(sendData, id_for_del.length() + 1);
              esp_err_t result = esp_now_send(MacAddress_sender3,(uint8_t *) &sendData,sizeof(sendData));
           }
         }

           
  }
  
  
        //String recMessage = client.readStringUntil('\n');
        /*
        int base64Length = recMessage.length();  // Παίρνουμε το μήκος του string Base64

        // Υπολογίζουμε το μέγεθος του πίνακα unsigned char που θα χρειαστεί για να αποθηκευτούν τα δεδομένα
        int decodedLength =3 * (base64Length / 4);

       const char *recm = recMessage.c_str();
       int k = recMessage.length();
       int decoded_length = Base64.decodedLength(const_cast<char *>(recm), k);
       char decoded[decoded_length + 1];
       Base64.decode(decoded, const_cast<char *>(recm), k);
       decoded[decoded_length] = '\0';
       //printHex("Encryption Data from Client : ", reinterpret_cast<const unsigned char*>(decoded), decoded_length);

       //pame gia apokriptografisi
       unsigned char decryptedText[decoded_length];
   
       IPAddress current_ip = client.remoteIP();
       
       
       if(current_ip==ipToCompare_room_1){
         zone =1;
         //Serial.print("room_1 : ");
         aes_cbc_decrypt(key, iv1, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
       } 
       else if (current_ip==ipToCompare_room_2){
         zone=2;
         //Serial.print("room_2 : ");
         aes_cbc_decrypt(key, iv2, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
       }
       else if (current_ip==ipToCompare_room_3){
         zone=3;
         //Serial.print("room_3 : ");
         aes_cbc_decrypt(key, iv3, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
       }
        else if (current_ip==ipToCompare_room_6){
         zone=6;
         //Serial.print("room_6 : ");
         aes_cbc_decrypt(key, iv4, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
       }
      // Μετατροπή του πίνακα σε ένα String
        String myString(reinterpret_cast<char*>(decryptedText),decoded_length);
       splitString(myString,zone);
      // Serial.print("THE PLAINTEXT IS is : ");
       Serial.println(myString);
      //Serial.println(decoded);
       
       
       //Serial.println("Η mmetafora egine .");
       //Serial.println("------------------------------------------------------");
      }
    }
  client.stop();
  /
  /Serial.println("-------------------------------------------------------------------");
  //Serial.println("Η σύνδεση τερματίστηκε.");
 
  } */
}  
void printHex(const char* label, const unsigned char* data, size_t length) {
    Serial.print(label);
    for (size_t i = 0; i < length; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0xF, HEX);
    }
    Serial.println();
}
/////////////////////////////////////////////////////////////////////////////////////
void connectToWiFi() {
  Serial.println("Σύνδεση στο WiFi");
 
  int attempt = 0;

  while (esp_now_init() != ESP_OK && attempt < 10) {
    delay(1000);
    Serial.print(".");
    attempt++;
  }

  if (esp_now_init() == ESP_OK) {
    Serial.println("\nΣυνδέθηκε στο WiFi με επιτυχία");
   neopixelWrite(LED_BUILTIN, 0 ,0,255 );
  } else {
    digitalWrite(LED_BUILTIN, LOW );
    Serial.println("\nΑποτυχία σύνδεσης στο WiFi. Το LED θα παραμείνει σβηστό.");
  }
}
////////////////////////////////////////////////////////////////////////////////////
void aes_cbc_decrypt(const unsigned char * key, unsigned char* iv, const unsigned char* ciphertext, unsigned char* plaintext,int length) {

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    mbedtls_aes_setkey_dec(&aes,key, 128);

     size_t ciphertextLength = strlen(reinterpret_cast<const char*>(ciphertext));
     size_t blockCount = length / 16;
     //size_t blockCount = (ciphertextLength + 15) / 16;

    //Serial.println("");
    //Serial.print("ciphertextLength : "); Serial.println(ciphertextLength);
    //Serial.print("blockCount : "); Serial.println(blockCount);

      unsigned char* decryptedBuffer = new unsigned char[blockCount * 16];
     
     for (size_t i = 0; i < blockCount; ++i) {
        /*Serial.print(" decrypt IV1 block  : ");
        for (int j = 0; j < 16; j++) {
              Serial.print(iv[j], HEX);  // HEX εμφανίζει τον αριθμό σε δεκαεξαδική μορφή.
              Serial.print(" ");
         }Serial.println();*/
       
          size_t blockStart = i * 16;
          mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, 16, iv,
                                ciphertext + blockStart,
                                decryptedBuffer + blockStart);
          //memcpy(iv, ciphertext + blockStart, 16); // Χρησιμοπ το τελευταίο κρυπτογραφημένο μπλοκ 
     }
    // Αντιγραφή του αποκρυπτογραφημένου κειμένου στον προορισμό
    memcpy(plaintext, decryptedBuffer, blockCount * 16);

    // Απελευθέρωση της μνήμης που δεσμεύτηκε
    delete[] decryptedBuffer;
    mbedtls_aes_free(&aes);
}
//////////////////////////////////////////////////////////////////////////////////////
void splitString(String str){
  
    int  lastColonPos = str.lastIndexOf("-");
   
    if (lastColonPos != -1 && lastColonPos != 0) {
        // Εξάγουμε την υποσυμβολοσειρά που βρίσκεται μεταξύ των δύο τελευταίων :
        room_no=str.substring(0,lastColonPos);
        id_for_del = str.substring(lastColonPos+1,str.length());
      
    } else {
        Serial.println("Δεν βρέθηκαν κατάλληλα δεδομένα. προβλημα με την αποκρηπτογραφηση");
    }
 

}
///////////////////////////////////////////////////////////////////////////////////////
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t* incomingData, int len){
    
  String strReceivedData;
  for (int i = 0; i < len-1; i++) {
     strReceivedData += (char)incomingData[i];
     } 

  //Serial.println("ENCRYPTION DATA RECEIVED IN BASE64 ENCODING: "  +strReceivedData);
  int base64Length = strReceivedData.length(); 

  //// Υπολογίζουμε το μέγεθος του πίνακα unsigned char που θα χρειαστεί για να αποθηκευτούν τα δεδομένα
  int decodedLength =3 * (base64Length / 4);
  const char *recm = strReceivedData.c_str();
  int k = strReceivedData.length();
  int decoded_length = Base64.decodedLength(const_cast<char *>(recm), k);
  char decoded[decoded_length + 1];
  Base64.decode(decoded, const_cast<char *>(recm), k);

  decoded[decoded_length] = '\0';

  //printHex("ENCRYPTION DATA RECEIVED IN HEX ENCODING:: ", reinterpret_cast<const unsigned char*>(decoded), decoded_length);
  unsigned char decryptedText[decoded_length];

  uint8_t senderMacAddress[6];
  memcpy(senderMacAddress,info->src_addr,6) ;
  
 
  if(memcmp(senderMacAddress, MacAddress_sender1, 6) == 0){
   
    aes_cbc_decrypt(key1, iv1, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
  }
  else if(memcmp(senderMacAddress, MacAddress_sender2, 6) == 0){
     
    aes_cbc_decrypt(key2, iv2, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
  }
  else if(memcmp(senderMacAddress, MacAddress_sender3, 6) == 0){
    
    aes_cbc_decrypt(key3, iv3, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
  }
   else if(memcmp(senderMacAddress, MacAddress_sender6, 6) == 0){
    
    aes_cbc_decrypt(key6, iv4, reinterpret_cast<const unsigned char*>(decoded), decryptedText,decoded_length);
  }
  else{
    return;
  }

  //printHex("PLAINTEXT IN HEX: ", reinterpret_cast<const unsigned char*>(decryptedText), decoded_length);

  // Μετατροπή του πίνακα σε ένα String
  String myString(reinterpret_cast<char*>(decryptedText),decoded_length);
  //splitString(myString,zone);
   
  // Serial.print("THE PLAINTEXT IS  : ");
   Serial.println(myString); 
  // esp_deep_sleep_start();
   //Serial.println("----------------------------------------------------");
}
void OnDataSent(const uint8_t *mac_addr,esp_now_send_status_t status){
     if (status == ESP_NOW_SEND_SUCCESS){
        if(sec_call=="0"){
            digitalWrite(SECURITY,HIGH);
            Serial.println("sec:0:");
        }
        else if(sec_call=="1") {
            digitalWrite(SECURITY,LOW);
            Serial.println("sec:1:");
        }
        sec_call="2";
     }
    //Serial.print("\r\nLast Packet Send Status:  ");
   // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success": "Delivery Fail");
}


