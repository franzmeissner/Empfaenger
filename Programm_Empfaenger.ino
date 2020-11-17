

#include <Joystick.h>
#include <string.h>
#include <stdio.h>

void setup() {
  //Baudraten müssen gleich sein
  Serial.begin(9600);     //  USB
  Serial1.begin(9600);   // XBee
  Joystick.begin(true);
  //Zur Sicherheit alle nicht benutzten Anwendungen der Joystick-Bibliothek auf 0 setzen
  Joystick.setZAxis(0);
  Joystick.setXAxisRotation(0);
  Joystick.setYAxisRotation(0);
  Joystick.setZAxisRotation(0);
  Joystick.setThrottle(0);
  Joystick.setRudder(0);
}

//Variablendeklaration
int zaehler = 0; 
bool Zeilevoll = false;   
char eingang[10];
char zeichen;
int x_semikolon;
int y_semikolon;
char x_hex[3];
char y_hex[3];
char s_hex[2];
int s_0;
int s_1;
int s_2;
int x_dez;
int y_dez;
int s_dez;
byte button;
byte buttonpressed = 0;
bool zeile_nicht_voll = true;

void loop()
{ 
  
  if (Serial1.available())
  {
      
      zeichen = Serial1.read(); //akutelles Zeichen von XBee einlesen
      //Vergleich ob eingelesene Zeile vollständig ist
      if (zeichen != 10)
      {
        eingang[zaehler] = zeichen;
        zaehler++;
      }
      else
      {
        //Es wird erkannt, dass alle Werte einer Zeile vollständig eingelesen sind
        zaehler = 0;

    //Variablendeklaration innerhalb Schleife, damit Variablen wieder zurückgesetzt werden können
    int x_zeiger = 0;
    int y_zeiger = 0;
    bool zwei_xwerte = false;
    bool x_semikolon_gefunden = false;
    bool y_semikolon_gefunden = false;
    int semikolon = 0;
    int i = 0;

    //aktuelle Wert liegen im Hexadezimalenzahlenformat in der Form: X;Y;B vor bzw. in ASCII: XX;YY;B
    //Beispiel: Aktuelle Zeile in HEX: 7F;7F;0 Zeilenumbruch     Aktuelle Zeile in ASCII: 37 46 3B 37 46 3B 30 0A
    //                                                      7 --> 37
    //                                                      F --> 46
    //                                                      ; --> 3B
    //                                                      7 --> 37
    //                                                      F --> 46
    //                                                      ; --> 3B
    //                                                      0 --> 30
    //                                          Zeilenumbruch --> 0A                                   
    //nun müssen zur Weiterverarbeitung die Werte für die X -und Y-Position, sowie Tasterwerte voneinander 
    //extrahiert werden --> in einzelne Arrays geschrieben
    //Damit eine Trennung erfolgen kann, wird nach einem Semikolon in einer aktuellen Zeile gesucht und die 
    //davor befindlichen Werte in das jeweilige Array geschrieben 
    //Die For-Schleife dient zum Auffinden der Position eines Semikolons bei gleichzeitiger Unterscheidung 
    //ob dieses einem X- bzw. Y-Wert zuzuordnen ist
      for (i ; i<strlen(eingang); i++)
        { x_zeiger++;
          y_zeiger++;
          if(eingang[i] == 59)
          {
            semikolon ++;
            if(semikolon == 1 and !x_semikolon_gefunden)
            {
              x_semikolon = x_zeiger-1;
              x_semikolon_gefunden = true;
            }
            if (semikolon == 2 and !y_semikolon_gefunden)
            {
               y_semikolon = y_zeiger-1;
              y_semikolon_gefunden = true;
            }
           
          }
          
        }
        //Index der Semikolons ist bekannt 
        //Prüfung in welcher Form die einzelnen ASCII-Zeichen vorliegen
        //Fall: X;...
        if (x_semikolon == 1)
        {
          x_hex[0] = 48;
          x_hex[1] = eingang[x_semikolon-1];
        }
        //Fall: XX;...
        else
        {
          zwei_xwerte = true;
          x_hex[0] = eingang[x_semikolon-2];
          x_hex[1] = eingang[x_semikolon-1];
        }

        //Fall: X;Y;B
        if (y_semikolon == 3)
        {
          y_hex[0] = 48;
          y_hex[1] = eingang[y_semikolon-1];
          s_hex[0] = eingang[y_semikolon+1];
        }
        else
        {
          //Fall: XX;Y;B
          if (y_semikolon ==4 and zwei_xwerte)
          {
            y_hex[0] = 48;
            y_hex[1] = eingang[y_semikolon-1];
            s_hex[0] = eingang[y_semikolon+1];
          }
          //Fall: X;YY;B
          else
          {
            y_hex[0] = eingang[y_semikolon-2];
            y_hex[1] = eingang[y_semikolon-1];
            s_hex[0] = eingang[y_semikolon+1];
          }
        }
        //Fall: XX;YY;B
        if (y_semikolon == 5)
        {
          y_hex[0] = eingang[y_semikolon-2];
          y_hex[1] = eingang[y_semikolon-1];
          s_hex[0] = eingang[y_semikolon+1];
        }

        //Nachfolgender Zugriff erwartet Werte in Dezimalbereich, also Umrechnung: HEX-->DEZ
        //Dezimalwerte müssen gleichzeitig noch in den richtigen Zahlenbereich gescho´ben werden,
        //damit Joystick-Bibliothek diese positionsgetreu auswerten kann
        x_dez = strtol(x_hex, NULL, 16)-127;
        y_dez = strtol(y_hex, NULL, 16)-127;
        s_dez = strtol(s_hex, NULL, 16);

        //Aktuellen Wert der X- und Y-Position der Joystick-Bibliothek übergeben
        Joystick.setXAxis(x_dez);
        Joystick.setYAxis(y_dez);

        //Fallunterscheidung um zu Prüfen welcher Taster betätigt wird
        if (s_dez == 0)
        {
          s_0 = s_dez;
          buttonpressed = 0;
          Joystick.setButton(s_1, buttonpressed);
          Joystick.setButton(s_2, buttonpressed);
        }
        if (s_dez == 1)
        {
          s_1 = s_dez-1;
          buttonpressed = 1;
          Joystick.setButton(s_1, buttonpressed);
        }

        if (s_dez == 2)
        {
          s_2 = s_dez-1;
          buttonpressed = 1;
          Joystick.setButton(s_2, buttonpressed);
        }
      }
  }
}

 
