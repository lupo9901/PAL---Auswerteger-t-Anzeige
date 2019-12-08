/*  Test zur Multiplex-Ansteuerung mehrerer 7-Segmentanzeigen.

     Segmente a-f + dot auf digitalPins angeschlossen.
     Massen werden vom µC durch LOW-pegel geschalten

    Luca Ponzio 2019

    Testversion/Prototyp für 10-Digit Multiplexing 
    Schulprojekt BS Deggendorf EGS 12

    Jegliche Teile des Codes wurden selbst geschrieben.


*/

//**Pinbelegung der Segmente**//

int _PINa = 4;
int _PINb = 5;
int _PINc = 6;
int _PINd = 7;
int _PINe = 8;
int _PINf = 9;
int _PINg = 10;

//**Pinbelegung der Massen der Anzeigen**//

int _Agnd = 11;
int _Bgnd = 12;
int _Cgnd = 2;
int _Dgnd = 3;

//**Einstellungen**//

static int  numOfPanels = 4;   //Zahl der Anzeigesegmente -> Später 10.
static int  startPin = 2;      //Erster Pin der Reihe am µC
static int  displayTime = 6;   //Anzeige der Zahl in millisekunden

//**Systemeinstellungen**//

volatile int timer = 0;

//Array für Position der Zahlen auf dem Display//

byte digitNumbers[] {
  0b01111110,   //"0"
  0b00001100,   //"1"
  0b10110110,   //"2"
  0b10011110,   //"3"
  0b11001100,   //"4"
  0b11011010,   //"5"
  0b11111010,   //"6"
  0b00001110,   //"7"
  0b11111110,   //"8"
  0b11001110,   //"9"
  0b00000001    //"."
};


bool displayNumbers(int pos1, int pos2, int pos3, int pos4 ) {    // Funktion zur Anzeige auf den Displays. Nimmt alle digits einzeln als Argumente

 static int y = 0;    // Aktuelles Digit

  if (timer >= displayTime) {   // Wenn der Timer-Interrupt-Zähler über einem treshold ist, wird die Anzeige aktualisiert. Da
    
    int val;    //Erhält den Wert der im aktuellen Durchlauf anzuzeigenden Zahl

    if (y == 0) val = pos4;   // Ordnet val je nach Durchlauf ein Digit zu
    if (y == 1) val = pos3;
    if (y == 2) val = pos2;
    if (y == 3) val = pos1;

    digitalWrite(_Agnd, y != 0);    // Schaltet die Massen für das aktuelle Digit auf LOW
    digitalWrite(_Bgnd, y != 1);    // Schaltet später die Transistoren
    digitalWrite(_Cgnd, y != 2);
    digitalWrite(_Dgnd, y != 3);

    for (int i = 0; i <= 6; i++) {
      digitalWrite(i + 4, digitNumbers[val] & 1 << i + 1 ); //Schreibt einen HIGH-Pegel, wenn das Zahlenhild der Abfragematrix entspricht
    }

     y++;   //Erhöhe Zähler für aktuelles Digit

    if(y >= numOfPanels ){    //Setze zurückt wenn alle Digits nacheinander angezeigt wurden
      y = 0;
      }
      
    timer = 0;    // Setze timer für Zeitsteuerung auf 0

  }
}

ISR(TIMER0_COMPA_vect) {   // Bei Überlaufen des CTC wird diese Funktion aufgerufen (Jede ms um 1 erhöht), Timer/Counter0 compare match A vector
  
  timer++;    // Erhöhe timer um 1
  
}


void setup() {

  for (int i = startPin; i <= 8 + numOfPanels; i++) {   //Deklaration der Outputs für 8 Pins + Massen der Segmente
    pinMode(i, OUTPUT);   // Ausgang als Output hernehmen
  }

  Serial.begin(9600);   //Serielle Kommunikation

  TCCR0A = (1 << WGM01);    // Modus auf CTC (Clear Timer on Compare)
  OCR0A = 0xF9;             //ORC0A auf 1ms -> Wenn TCNT0 == OCR0A -> Löse interrupt aus

  TIMSK0 |= (1 << OCIE0A); // Setze Flag für Interrupt 
  sei();                    // Interrupts an

  TCCR0B |= (1 << CS01);    //Prescaleregister auf 1/64 der Clock setzen
  TCCR0B |= (1 << CS00);    //Bit 2 dito

}

void loop() {   //Hauptcode, jetzt nur ein Testprogramm zu testen 

  int val = analogRead(A0);   //Lese Analogwert ein

  int val1 =  val         % 10 ;    // Ermittle die MS Ziffer
  int val2 = (val / 10)   % 10 ;    // zweite Ziffer
  int val3 = (val / 100)  % 10 ;    // dritte Ziffer
  int val4 = (val / 1000) % 10 ;    // Ermittle die LS Ziffer

  displayNumbers(val1, val2, val3, val4);   //Zeige eingelesenen Wert auf Display an.

}
