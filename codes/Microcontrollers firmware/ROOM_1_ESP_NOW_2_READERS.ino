#include <MFRC522.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Base64.h>
#include <mbedtls/aes.h>
#include <DHT.h>
//#include <cstring>
//#include <cctype>

// Μέγιστος αριθμός καρτών που μπορεί να αποθηκεύσει ο πίνακας
#define DEFAULT_UID {0x00, 0x00, 0x00, 0x00}
#define MAX_CARDS 10 
#define ROOM 1
#define RST_PIN_1         12        
#define RST_PIN_2         15  
#define SS_PIN_1        18 
#define SS_PIN_2        13 
#define DHT_data         5


int  numStoredCards = 0;
#define alarm_sensor  14
#define smoke_sensor  6
#define beem_sensor  7
#define door_lock 23
#define buzzer 22
#define bypass_beem 3

//esp now
uint8_t broadcastAddress[] = {0x40,0x4C,0xCA,0x44,0xBD,0x48};
esp_now_peer_info_t peerInfo;

//Καθε πόσο χρόνο θα στέλνω μύνημα αν δεν αλλαζει το περιεχομενο
const  long interval=20000;
unsigned long previousMillis = 0; 

//Για τον μετρητη που θα ειναι για το φωτοκυταρο 
unsigned long previousMillisTimer=0;

int escape = 0;
int allow = 0;
bool timer_on=0;
int timer=0;
int beem_on_one_time=0;
bool fire = false;
bool reader_on = false;
int enablePin = 1;

byte storedCards[MAX_CARDS][4];
byte allowedCrards[MAX_CARDS][4]={
    {0x73, 0xFE, 0x1C, 0x0E} ,
    {0x83, 0x03, 0xDF, 0xAA} ,
    {0x03, 0x69, 0x3F, 0x9A} ,
    {0xC4, 0xE4, 0x0E, 0x72} ,
    {0x03, 0x5D, 0x86, 0x9A} ,
    {0x74, 0xED, 0x92, 0x72} ,
    {0x23, 0xB6, 0x34, 0x1B} ,
    {0x63, 0xDD, 0x2A, 0x1B} ,
};

//οριζω τους καρταναγνωστες
MFRC522 mfrc522_entry(SS_PIN_1, RST_PIN_1); 
MFRC522 mfrc522_exit(SS_PIN_2, RST_PIN_2);  // Create MFRC522 instance


DHT dht(DHT_data,DHT11);

 //για AES CRC
const unsigned char key[] = "aaaaaaaaaaaaaaaa";//{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
unsigned char iv[]  ={0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};
//unsigned char iv[]  ={0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
//unsigned char iv[]  ={0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x4,  0x43, 0x43, 0x43, 0x43, 0x43};
//unsigned char iv[]  ={0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44};
//unsigned char iv[]  ={0x45, 0x45, 0x45 ,0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45};
//unsigned char iv[]  ={0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46};

unsigned char prev_iv[] ={0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};

String prevMsg="";


//ορισμος συναρτησεων για επεξεργασια πινακων αποθηκευσης
void initializeStoredCards(byte storedCards[MAX_CARDS][4]);
bool isUidStored(MFRC522::Uid uidToCheck,byte storedCards[MAX_CARDS][4],int numStoredCards);
void storeUid(MFRC522::Uid uidToStore,byte storedCards[MAX_CARDS][4],int *numStoredCards);
void deleteUid(byte storedCards[MAX_CARDS][4],int indx,int *numStoredCards);
int  returnIndex(MFRC522::Uid uidToCheck,byte storedCards[MAX_CARDS][4],int numStoredCards);
void printStoredCards(byte storedCards[MAX_CARDS][4],int numStoredCards);
void printUid(byte *uid);
String makeMessage(byte storedCards[MAX_CARDS][4],int numStoredCards_r1,String  room);
void aes_cbc_encrypt(const unsigned char* key, unsigned char* iv, const char* plaintext, unsigned char* ciphertext,int length);
String addZero(String inputString);
int  sendToServer(String msg);
void printHex(const char* label, const unsigned char* data, size_t length);
void connectToWiFi();
void OnDataSent(const uint8_t *mac_addr,esp_now_send_status_t status);
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t* incomingData, int len);

float prevTemp = 0;
float prevHum = 0;

int first_time_buzer=0;

//String  do_encrypt(String msg, String key_str, String iv_str);΄΄

//βοηθανε αν δεν ποιασει το φωτολυταρο  να ακυρωση την εισοδο εξοδο
MFRC522::Uid temp_currentUid;
uint8_t entry=0;

void setup() {
  Serial.begin(115200);	
  ////////////////////////////////////////////////////////////
  Serial.print("Διαθέσιμη μνήμη RAM: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println("Έκδοση πυρήνα:");
  Serial.println(ESP.getCoreVersion());

  // Εκτύπωση πληροφοριών για τον χώρο αποθήκευσης
  /*
  Serial.println("Πληροφορίες χώρου αποθήκευσης:");
  Serial.print("Συνολικός χώρος: ");
  Serial.print(ESP.getFlashChipSize());
  Serial.println(" bytes");
  Serial.print("Διαθέσιμος χώρος: ");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
  */
 /////////////////////////////////////////////////////////////
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(alarm_sensor,INPUT_PULLUP);
  pinMode(smoke_sensor,INPUT_PULLUP);
  pinMode(beem_sensor,INPUT_PULLUP);
  pinMode(door_lock,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(bypass_beem,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);

  pinMode(enablePin,OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(door_lock,LOW);
 
  	// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  //mySerial.begin(9600);
  SPI.begin();
  dht.begin();

  //try to connect wifi
  WiFi.mode(WIFI_STA);
  connectToWiFi();
 
  //Register the send callback
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
 
  //register perr
  memcpy(peerInfo.peer_addr,broadcastAddress,6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

  //Add peer
  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.print("Faillled to add peer");
  }
  
  neopixelWrite(LED_BUILTIN, 51 ,0,25 );
  
  
  //αρχικοποιω τουσ πίνακες που αποοθηκ τις καρτες
  initializeStoredCards(storedCards);
 
  
  //αρχικοποιω τα RFID module
	mfrc522_entry.PCD_Init();	
  mfrc522_exit.PCD_Init();	

	delay(40);		

  // Show details of PCD - MFRC522 Card Reader details
	mfrc522_entry.PCD_DumpVersionToSerial();	
  mfrc522_exit.PCD_DumpVersionToSerial();
	Serial.println(F("RFID_1:Scan PICC to see UID, SAK, type, and data blocks..."));
 
}


void loop() {
 
  //αν δωσει μια φορα fire oi portes menioyn monhma anoixtes
 if(digitalRead(smoke_sensor)==0){
    digitalWrite(door_lock,HIGH);
    fire = true;
  } 

 //μεατα απο διακοπτη bypass επανερζ
  if(digitalRead(9)==0){
    fire = false;
  }

 
 
 if(!fire){ 
       //το ρελε για μαπζερ και κλειδωμα πορτας παει αναποδα με την επιτρεψη
      //Αν ομως εχουμε φωτια απαελευθερώνονται οι πορτες απο την προηγούμενη  for και δεν ξαναμαινουμε εδω 
      //μονο μετα απο reset
       if(allow==1){
         digitalWrite(door_lock,HIGH);
         if(first_time_buzer==1){
             //tone(buzzer,1000);
             first_time_buzer=0;
          }
       }
       else{
         digitalWrite(door_lock,LOW);
         //noTone(buzzer);
         //digitalWrite(buzzer, LOW);
       }
 }
  
  //μετρητης μετραει δευτε4ρολεπτα και αν περασουν τα 3 τοτε δινει σημα για οτι τεειωσε ο χρονος χαρητος
  if(timer_on==1){
        unsigned long currentMillisTimer = millis();
       if(currentMillisTimer -  previousMillisTimer <1000){
       
       }
       else{
            previousMillisTimer = currentMillisTimer;
            timer++;
            if(timer>4){
              allow = 0;
              timer = 0;
              timer_on = 0;
              if(entry){
                      
                     if (isUidStored(temp_currentUid,storedCards, numStoredCards)) {
                        Serial.println("Ακυρώνετε η είσοδος γιατι δεν εποιασε το φωτοκυταρο  !!!");
                        int indx = returnIndex(temp_currentUid,storedCards, numStoredCards);
                        deleteUid(storedCards,indx,&numStoredCards);
                        printStoredCards(storedCards,numStoredCards);
                      }
              }
              else{
                      if (!isUidStored(temp_currentUid,storedCards, numStoredCards)) {
                        Serial.println("Ακυρώνετε η εξοδος γιατι  δεν εποισε το φωτοκυταρο  !!!");
                        storeUid(temp_currentUid,storedCards,&numStoredCards);
                        printStoredCards(storedCards,numStoredCards);
                      }
                 }
            }
        }  
  }       
  else{
    timer=0;
    allow=0;
  }

  
  if(escape == 1 ) escape =0;

  
  digitalWrite( enablePin, HIGH);     // Enable the internal 38kHz signal.
  delayMicroseconds( 210);  
  if(digitalRead(beem_sensor)==0){
    delayMicroseconds(395);
  }
  //ελενχος αν περασε καποιος απο το φωτοκυταρο νομιμα
  if(digitalRead(beem_sensor)==0 && digitalRead(bypass_beem)==1 ){
    if(!allow)
      escape = 1 ;
    else
      beem_on_one_time=1;
  }
  else if(beem_on_one_time==1){
      timer_on = 0;
      beem_on_one_time=0;
  }

  digitalWrite( enablePin, LOW);     // Enable the internal 38kHz signal.
  delayMicroseconds( 263);  
  
 

   if(fire) escape =0;

  //ΕΛΕΝΧΟ ΣΥΝΕΧΕΙΑ ΤΟ WIFI
  if(esp_now_init() == ESP_OK){
        neopixelWrite(LED_BUILTIN, 51 ,0,25 );
     }else{
        digitalWrite(LED_BUILTIN,LOW);
        Serial.println("anamoni");
        connectToWiFi();
  }

  unsigned long currentMillis = millis();
  
  String msg = makeMessage(storedCards,numStoredCards,"ROOM");
  //αν δεν αλλαζουν τα δεδομενα δεν στελνω κατι εκτοσ αν περασουν καποια sec
  if( prevMsg.equals(msg)&& (currentMillis - previousMillis < interval)){
        //Serial.println(" S A M E");
  }
  else{
     prevMsg = msg;
     String newMsg= addZero(msg);
     int length = newMsg.length();

     Serial.print("THE PLAINTEXT IS: ");
     Serial.println(newMsg);
     unsigned char ciphertext[length];
     const  char *plaintext = newMsg.c_str();

     memcpy(prev_iv,iv,sizeof(iv));
     
     printHex("PLAINTEXT  IN HEX: ", reinterpret_cast<const unsigned char*>(plaintext), length);
     aes_cbc_encrypt(key, iv, plaintext, ciphertext,length);
     
     printHex("ENCRYPTED TEXT IN HEX: ", reinterpret_cast<const unsigned char*>(ciphertext), length);


     // Κωδικοποίηση Base64
     String fromServer = base64::encode( ciphertext,length);
     Serial.println("ENCRYPT DATA TO BE SENT IN BASE64 ENCODING: " + fromServer);

     if(sendToServer(fromServer)==0){
         memcpy(iv,prev_iv,sizeof(iv));
     }
     Serial.println("----------------------------------------------------");
  previousMillis  = currentMillis; 
  }


  
   if(reader_on){
    noTone(buzzer);
    reader_on = false;
  }
    

	// An den yparxei karta jekinav apo thn arxh
	if ( ! mfrc522_entry.PICC_IsNewCardPresent()  && !mfrc522_exit.PICC_IsNewCardPresent()) {
    return;
	}
  
  //οταν διαβασει καρτα ο πρωτος καρταναγνώστης που ειναι για είσοδο
	if (mfrc522_entry.PICC_ReadCardSerial()  ) {

    Serial.println("--------------------------------------------");
		Serial.print("----------RFID ENTRY  is enable-----ROOM  ");
    Serial.print(ROOM); 
    Serial.println(" --------------");
   
    MFRC522::Uid currentUid = mfrc522_entry.uid;
    temp_currentUid =  mfrc522_entry.uid;

    //an den ypoarxei ayti i karta
    if(!isUidStored(currentUid,allowedCrards,8)){
      return;
    }
    tone(buzzer,1000);
    reader_on= true;
    timer_on=1;
    timer = 0;
    allow =1 ;
    entry = 1;
    first_time_buzer=1;

    // Έλεγχος αν το UID είναι ήδη αποθηκευμένο ηδη στον πινακα και αν δεν  ειναι το βαζω μεσα
    if (!isUidStored(currentUid,storedCards, numStoredCards)) {
         
         Serial.println("Νέο UID αναγνωρίστηκε και αποθηκεύτηκε!");
         storeUid(currentUid,storedCards,&numStoredCards);
         printStoredCards(storedCards,numStoredCards);

    }else{ //ειναι αποθηκευμενο και δεν κανω τιποτα

         Serial.println("Το UID είναι ήδη αποθηκευμένο και δεν κανω τιποτα !!.");
         //int indx = returnIndex(currentUid,storedCards, numStoredCards);
         //deleteUid(storedCards,indx,&numStoredCards);
         printStoredCards(storedCards,numStoredCards);
    }
   Serial.println("----------------------------------------------------");


	}
  
  //οταν διαβασει καρτα ο πρωτος καρταναγνώστης που ειναι για εξοδο
  if(mfrc522_exit.PICC_ReadCardSerial()){
    Serial.println("--------------------------------------------");
		Serial.print("----------RFID EXIT  is enable-----ROOM  ");
    Serial.print(ROOM); 
    Serial.println(" --------------");
   
    MFRC522::Uid currentUid = mfrc522_exit.uid;
    temp_currentUid = mfrc522_exit.uid;

    //an den ypoarxei ayti i karta
    if(!isUidStored(currentUid,allowedCrards,8)){
      return;
    }
    tone(buzzer,1000);
    reader_on= true;
    timer_on=1;
    timer = 0;
    allow =1 ;
    entry = 0;
    first_time_buzer=1;

       // Έλεγχος αν το UID είναι ήδη αποθηκευμένο  στον πινακα το διαγράφω
    if (isUidStored(currentUid,storedCards, numStoredCards)) {
         

         Serial.println("Το UID είναι ήδη αποθηκευμένο και διαγραφω  !!!");
         int indx = returnIndex(currentUid,storedCards, numStoredCards);
         deleteUid(storedCards,indx,&numStoredCards);
         printStoredCards(storedCards,numStoredCards);


         

    }else{ //ειναι δεν  αποθηκευμενο  δεν κανω τιποτα
         Serial.println("Το  UID δεν υπάρχει και δεν εκανα ακατι!");
         //storeUid(currentUid,storedCards,&numStoredCards);
         printStoredCards(storedCards,numStoredCards);
        
    }
   Serial.println("----------------------------------------------------");
  }
 

   mfrc522_entry.PICC_HaltA();
   mfrc522_exit.PICC_HaltA();
   mfrc522_entry.PCD_StopCrypto1();  
   mfrc522_exit.PCD_StopCrypto1();  

}


//////////////////////////////////////////////////////////////
void initializeStoredCards(byte storedCards[MAX_CARDS][4]) {
  // Αρχικοποίηση του storedCards (για παράδειγμα)
  for (int i = 0; i < MAX_CARDS; i++) {
    for (int j = 0; j < 4; j++) {
      storedCards[i][j] = 0;
    }
  }
}
//////////////////////////////////////////////////////////////
void storeUid(MFRC522::Uid uidToStore,byte storedCards[MAX_CARDS][4],int *numStoredCards) {
  if (*numStoredCards < MAX_CARDS) {
    // Αν ο πίνακας δεν είναι γεμάτος, αποθηκεύουμε το UID
    memcpy(storedCards[*numStoredCards], uidToStore.uidByte, 4);
    (*numStoredCards)++;
  } else {
    Serial.println("Ο πίνακας UID είναι γεμάτος!");
  }
}
/////////////////////////////////////////////////////////////
bool isUidStored(MFRC522::Uid uidToCheck,byte storedCards[MAX_CARDS][4],int numStoredCards) {
  for (int i = 0; i < numStoredCards; i++) {
    if (memcmp(storedCards[i], uidToCheck.uidByte, 4) == 0) {
      return true; // Το UID είναι ήδη αποθηκευμένο
    }
  }
  return false; // Το UID δεν είναι αποθηκευμένο
}
///////////////////////////////////////////////////////////////////////
void deleteUid(byte storedCards[MAX_CARDS][4],int indx,int *numStoredCards){
    //διαγραφω την καρτα που ηταν στην θεση 
    for(int i = indx; i<*numStoredCards; i++){
       for (int j = 0; j < 4; j++) {
          storedCards[i][j] =storedCards[i+1][j] ;
       }
    }
    (*numStoredCards)--;
}
///////////////////////////////////////////////////////////////////////
int returnIndex(MFRC522::Uid uidToCheck,byte storedCards[MAX_CARDS][4],int numStoredCards) {
  for (int i = 0; i < numStoredCards; i++) {
    if (memcmp(storedCards[i], uidToCheck.uidByte, 4) == 0) {
      return i; // Το UID είναι ήδη αποθηκευμένο
    }
  }
  return 0; // Το UID δεν είναι αποθηκευμένο
}
//////////////////////////////////////////////////////////////////////
void printStoredCards(byte storedCards[MAX_CARDS][4],int numStoredCards) {
  Serial.println("Αποθηκευμένα UID:");

  for (int i = 0; i < numStoredCards; i++) {
    Serial.print("Κάρτα ");
    Serial.print(i + 1);
    Serial.print(": ");
    printUid(storedCards[i]);
  }
}
///////////////////////////////////////////////////////////////////////////////
void printUid(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    Serial.print(uid[i], HEX);
  }
  Serial.println();
}
/////////////////////////////////////////////////////////////////////////////////
String  makeMessage(byte storedCards[MAX_CARDS][4],int numStoredCards,String room){

     //char t;
     String temp="";
     float CurentHum = dht.readHumidity();
     float CurentTemp= dht.readTemperature();

     //elenxo an exei allaxi kata ena bathmo i igrasioa
     if( abs(CurentHum - prevHum) > 1 )
         prevHum = CurentHum;

      //elenxo an exei allaxi kata ena bathmo 0.5  i thermokrasia 
     if( abs(CurentHum - prevTemp) > 0.5 )
         prevTemp = CurentTemp;


     
     for(int i=0; i<numStoredCards; i++){
        for(int j=0; j<4; j++){
           //t = char(storedCards_r1[i][j]);
           if(storedCards[i][j] < 0x10 )  temp +="0";
           String str = String(storedCards[i][j],HEX);
           temp +=str;
           //if(j !=3) temp +=',';
         }
     if(i !=numStoredCards-1) temp +="|";
     }
    temp = String(ROOM) + ":" + temp  + ":" + String(prevTemp) + ":" + String(prevHum) + ":" +String(escape) + ":" +String(!digitalRead(smoke_sensor))  + ":" +String(digitalRead(alarm_sensor)) + ":" ;
    
     return temp;
}
/////////////////////////////////////////////////////////////////////////////////////
int   sendToServer(String msg){
    
  
    //stelno ta data se byte gia apodotikotita
    uint8_t sendData[msg.length() + 1];
    msg.getBytes(sendData, msg.length() + 1);

    esp_err_t result =esp_now_send(broadcastAddress,(uint8_t *) &sendData,sizeof(sendData));
    if(result == ESP_OK){
      Serial.println("Sending confirmed");
      return 1;
    }
    else{
      Serial.println("Sending error");
      return 0;
    }

}
/////////////////////////////////////////////////////////////////////////////////////
void aes_cbc_encrypt(const unsigned char* key, unsigned char* iv, const char* plaintext, unsigned char* ciphertext,int length) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    
    mbedtls_aes_setkey_enc(&aes, key, 128);

    size_t plaintextLength =strlen(reinterpret_cast<const char*>(plaintext));
    size_t blockCount = (plaintextLength + 15) / 16;

    //Serial.print("plaintextLength : "); Serial.println(plaintextLength);
    //Serial.print("blockCount : "); Serial.println(blockCount);
    
    // Δέσμευση μνήμης για το κρυπτογραφημένο κείμενο
    unsigned char* encryptedBuffer = new unsigned char[blockCount * 16];
    
     
       // Κάθε επανάληψη κρυπτογράφησης ενός μπλοκ
    for (size_t i = 0; i < blockCount; ++i) {
        
        Serial.print(" encrypt IV block  : ");
        for (int j = 0; j < 16; j++) {
              Serial.print(iv[j], HEX);  // HEX εμφανίζει τον αριθμό σε δεκαεξαδική μορφή.
              Serial.print(" ");
         }Serial.println();

        size_t blockStart = i * 16;
        //size_t blockSize = std::min<size_t>(16, plaintextLength - blockStart);
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, 16, iv,
                               reinterpret_cast<const unsigned char*>(plaintext + blockStart),
                               encryptedBuffer + blockStart);
        mbedtls_aes_setkey_enc(&aes, key, 128);  // Επαναφορά του κλειδιού για τον επόμενο γύρο
        //memcpy(iv, ciphertext + blockStart, 16); // Χρησιμοποι το τελευταίο κρυπτογραφημένο μπλοκ ως IV για τον επόμενο γύρο
    }

    memcpy(ciphertext, encryptedBuffer, blockCount * 16);

  
    // Απελευθέρωση της μνήμης που δεσμεύτηκε
    delete[] encryptedBuffer;

    mbedtls_aes_free(&aes);
 
}
///////////////////////////////////////////////////////////////////////////////////

String addZero(String inputString) {
  // Elegxos an to mikos tou string einai pollaplasio tou 16
  if (inputString.length() % 16 != 0) {
    // Ypologismos posa midenika xreiazontai gia na ginei to mikos pollaplasio tou 16
    int zerosToAdd = 16 - (inputString.length() % 16);
    
    // Prosthiki midenikwn sto telos tou string
    for (int i = 0; i < zerosToAdd; i++) {
      inputString += '0';
    }
  }
  
  return inputString;
}
////////////////////////////////////////////////////////////////////////////////////////
void printHex(const char* label, const unsigned char* data, size_t length) {
    Serial.print(label);
    for (size_t i = 0; i < length; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0xF, HEX);
    }
    Serial.println();
}
///////////////////////////////////////////////////////////////////////////////////////////
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
  neopixelWrite(LED_BUILTIN, 51 ,0,25 );
  } else {
    Serial.println("\nΑποτυχία σύνδεσης στο WiFi. Το LED θα παραμείνει σβηστό.");
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////
void OnDataSent(const uint8_t *mac_addr,esp_now_send_status_t status){
    Serial.print("\r\nLast Packet Send Status:  ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success": "Delivery Fail");
}
////////////////////////////////////////////////////////////////////////////////////////////////
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t* incomingData, int len){
    Serial.println("sasaddaddasdda");
  String strReceivedData;
  for (int i = 0; i < len-1; i++) {
     strReceivedData += (char)incomingData[i];
     } 

   uint8_t senderMacAddress[6];
  memcpy(senderMacAddress,info->src_addr,6) ;
  if(memcmp(senderMacAddress, broadcastAddress, 6) == 0){
    
       
       MFRC522::Uid uid_for_del;
        for (int i = 0; i < strReceivedData.length(); i += 2) {
              byte byteValue = strtol(strReceivedData.substring(i, i + 2).c_str(), NULL, 16);
              uid_for_del.uidByte[i / 2] = byteValue;
       }

    
      if (isUidStored(uid_for_del,storedCards, numStoredCards)) {
             int indx = returnIndex(uid_for_del,storedCards, numStoredCards);
             deleteUid(storedCards,indx,&numStoredCards);     
      }
    
  }
  
  
}
