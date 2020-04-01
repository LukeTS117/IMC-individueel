#include <PS2Keyboard.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>


/*
 * Initializes the lcd display
 * first param is the adress of the component
 * the rest of the params are values bound to the I2C chip you are using, in my case this was the PCF8574T
*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#define BACKLIGHT_PIN 13
String command;

// A bitmap for the annimation
const uint8_t charBitmap[][8] = {
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 },
   { 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0 },
   { 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0 },
   { 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0 },
   { 0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0 },
   { 0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0 }
   
};

/*
 * Data class to create a Contact
 * params: String name, name of the contact
 *         String Number, Mobile phone number of the contact
 *         struct Contact* next, pointer to the next struct
 *         struct Contact* prev, pointer to the previous struct
*/
struct Contact{
  String name;
  String Number;
  struct Contact* next;
  struct Contact* prev;
};

// this line is used to give the head of the double linked list a memory adress
Contact* head = NULL;

//sets up necesary params for the project
void setup(){
  //sets the serial port to 9600 baud rate
  Serial.begin(9600);
  //sets the lcd backlight pin on output, and high
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  // starts the initialization of the bitmap, as well as the lcd
  int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
  lcd.begin(16,2);

  for ( int i = 0; i < charBitmapSize; i++ ){
    lcd.createChar ( i, (uint8_t *)charBitmap[i] );
  }
  
  printMenu();
}

void loop(){
  startMenu();
}

//starts the menu for the application
//param command is the input command that is given by the user
void startMenu(){
   if(Serial.available()){
      command = Serial.readStringUntil('\n');

      if(command.equals("a")){
         addContact();
      }else if(command.equals("b")){
         displayContact();
      }else if(command.equals("c")){
         removeContact(&head);
      }else if(command.equals("d")){
        displayAnnimation();
      }else {
        Serial.println("Not a valid command, please enter a valid command!");
      }
     printMenu();
   }
}

//prints the menu
void printMenu(){
  Serial.println("choose item: ");
  Serial.println("a : Add contact");
  Serial.println("b : Display contact");
  Serial.println("c : Remove contact");
}

//adds a contact to the double linked list
void addContact(){
  Serial.print("Name: ");
  while(Serial.available() == 0){}
  String nameContact = Serial.readStringUntil('\n');
  Serial.println(nameContact);
  Serial.print("Phone number: ");
  while(Serial.available() == 0){}
  String phoneNumber = Serial.readStringUntil('\n');
  Serial.println(phoneNumber);

  pushNode(&head, nameContact, phoneNumber);
}

//pushes the contact into the double linked list
// params: head_ref is a refrence to the head node
//         String nameContact is the name of the contact
//         String phoneNumber is the number of the contact
void pushNode(struct Contact** head_ref, String nameContact, String phoneNumber){
  struct Contact* new_contact = (struct Contact*) malloc(sizeof(struct Contact));
  new_contact->name = nameContact;
  new_contact->Number = phoneNumber;

  new_contact->next = (*head_ref);
  new_contact->prev = NULL;

  if((*head_ref) != NULL){
    (*head_ref)->prev = new_contact;
  }

  (*head_ref) = new_contact;
}

//removes a contact from the linked list
//param: head_ref is a refrence to the head node
void removeContact(struct Contact** head_ref){
  Serial.println("Name of contact you want to delete: ");
  while(Serial.available() == 0){};
  String nameContact = Serial.readStringUntil('\n');
  Serial.print(nameContact);
  struct Contact* del = getContact(&head,nameContact);

  if (*head_ref == NULL || del == NULL) 
        return; 

  if (*head_ref == del){
       *head_ref = del->next; 
  }
    if (del->next != NULL){
        del->next->prev = del->prev; 
    }
    if (del->prev != NULL){ 
        del->prev->next = del->next; 
    }
    
    free(del); 
 
}

//gets the contact from a linked list
//params: head_ref is a refrence to the head node
//        contactName is the name of the desired contact
struct Contact* getContact(struct Contact** head_ref, String contactName){
    if(*head_ref == NULL || contactName.equals(""))
      return NULL;
    
    struct Contact* current = *head_ref; 
    int i; 
  
    for (int i = 1; current != NULL && current->name != contactName; i++){ 
        current = current->next;
    }
    
    if (current == NULL){ 
        Serial.println("This Contact does not exist");
        return NULL; 
    }

    return current;
}

//displays the contact on the lcd screen
void displayContact(){
  Serial.println("Name of contact: ");
  while(Serial.available() == 0){};
  String nameContact = Serial.readStringUntil('\n');
  Serial.println(nameContact);
  struct Contact* needed = getContact(&head,nameContact);
  if(needed != 0){
    lcd.clear();
    Serial.println("Cleard LCD");
    lcd.print(needed->name);
    Serial.println(needed->name);
    lcd.setCursor(0,1);
    lcd.print(needed->Number);
    Serial.println(needed->Number);
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("There is no such");
    lcd.setCursor(0,1);
    lcd.print("Contact!");
  }
  
}

//displays the animation to the home screen
void displayAnnimation(){
  bool playing = true;
  int i = 0;
  lcd.clear();
  lcd.home();                   // go home
  lcd.print("You founnd the");  
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("Easter egg!");
  delay ( 1000 );
  lcd.clear();
  while(playing){
    for ( int i = 0; i < 2; i++ ){
      for ( int j = 0; j < 16; j++ ){
         lcd.print (char(random(7)));
      }
      lcd.setCursor ( 0, 1 );
    }
    delay (200);
    i++;
    if(i == 25){
      playing = false;
    }
  }
}
 
