#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
// include the library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
// Yeah so this is the class array i made to store the uids of the cards read 
class uidstore{  
  public:
  byte uidb[4];
  bool cardMatch(byte readcarduid[4]){
    for(int h=0;h<4;h++){
      if(uidb[h]!=readcarduid[h]) return false;  
    }       
    return true;
  }
}card[100];
int k=0;
//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:")); //shows in serial that it is ready to read
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

//*****************************************************************************************//
void loop() {
  boolean cardpresent = false;  // Initially No Card is Present 
  int indextochange; // This is to store the index of the card which already exists in the buffer ...we'll remove it from buffer and subtract the item price !
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------
  byte readcarduid[4]; // This will store the UID bytes 
  for(int u=0;u<4;u++){
    readcarduid[u]=mfrc522.uid.uidByte[u];  // Assigning the UID bytes
  }
  
  if(k==0){     // Initial Case when the first card is being scanned
    for(int g=0;g<4;g++){
      card[k].uidb[g]=readcarduid[g];   // Store the UID in class array 
    }
    ++k;    
  }
  else{
        for(int t=0;t<k;t++){     // If a card is already known , for each scanned card, we'll check if its already there
            if(card[t].cardMatch(readcarduid)== true){
              cardpresent = true;
              indextochange=t;    
              break; 
              }
        }
              if(cardpresent==true){
              for(int d=indextochange;d<k;d++){  // Im removing the card UID itself by shifting all card ojects by one index lower 
                   for(int dd=0;dd<4;dd++){
                     card[d].uidb[dd]=card[d+1].uidb[dd];
                   }               
              }
             --k; // I decrement k finally to reduce the total cards known 
             }
                   
             if(cardpresent== false){
             for(int x=0;x<4;x++){  // In case card is new and not scanned before, add it to the card buffer 
             card[k].uidb[x]=readcarduid[x];  //Add the UID data byte by byte 
             }
            ++k;  // Increase the No: of Cards Known Now 
            }
        
  }
  //-------------------------------------------
  
  Serial.print(F("Item Price, Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //--- Get Price Of Item
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //---Print Price Of Item 
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {

      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");

  //---Get Item Name

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  String msg1="";
  //---Print Item Name 
  for (uint8_t i = 0; i < 16; i++) {
    
    Serial.write(buffer2[i] );

  }
  
   msg1 =(char*)buffer1;  // Stores The Price Of The Item 

  Serial.print("Card UID:");    //Prints The UID of The Card on The Serial Monitor ... 
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }  
   Serial.println(" ");
  //----------------------------------------

  delay(1000); //change value if you want to read cards faster
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("Rate : ");
  int rate=0;
  rate = rate + atoi((char*)buffer1);   // Yeah its a lil round the bush but works flawlessly !
  lcd.print(rate);  
  
  static long int sum=0;
       if(cardpresent==true){ // This the magic variable xD .... determines whether a card has been scanned before or not 
      sum = sum - atoi((char*)buffer1); 
     }
     else{
      sum = sum + atoi((char*)buffer1); 
     }
  lcd.setCursor(0,1);
  lcd.print("Total: ");
  lcd.print(sum);
  delay(2000);
  lcd.clear();
  }
//*****************************************************************************************//
