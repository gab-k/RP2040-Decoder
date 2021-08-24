#define CV_1 5      //Adresse                -   Die Basisadresse enthält die Lokadresse. Ist CV#1 gleich 0, verlässt der Decoder den Digitalbetrieb und wechselt in den Alternativ-Betrieb (Analog-Betrieb). Normalerweise wird beim Schreiben der CV1 im Decoder automatisch CV19 (Mehrfachtraktionsadresse) und in CV29 das Bit 5 (Verwendung der erweiterten Adresse; Bit 5 bei Zählung ab 0) gelöscht.
#define CV_2 0      //U_min                  -   Definiert die Fahrspannung, die bei niedrigster Fahrstufe am Motor anliegt.
#define CV_3 5      //Beschleunigungsrate    -   Definiert die Länge der Wartezeit, die beim Beschleunigen jeweils vor dem Hochschalten zur nächst höheren Fahrstufe vergeht.
#define CV_4 5      //Bremsrate              -   Definiert die Länge der Wartezeit, die beim Bremsen jeweils vor dem Herunterschalten zur nächst niedrigen Fahrstufe vergeht.
#define CV_5 255    //U_max                  -   Definiert die Fahrspannung, die bei höchster Fahrstufe am Motor anliegt.
#define CV_6 127    //U_mid                  -   Definiert die Fahrspannung, die bei mittlerer Fahrstufe am Motor anliegt.
#define CV_8 1      //Versionsnummer         -   Gibt die Versionsnummer der Decoder-Software an.
#define CV_9 5      //PWM-Periode            -   Definiert die Frequenz der Pulsweitenmodulation.