#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal.h>

/// Init Digicode
const byte n_Li= 4; //nombre de ligne du digicode
const byte n_Col= 4; //nombre de colonnes du digicode
byte Li_Pins[n_Li] = {9,8,7,6}; //Lignes 0 à 3
byte Col_Pins[n_Col]= {5,4,3,2}; //Colonnes 0 à 3
int compteur=0;
int tps =millis();

// Paterne du digicode
char Paterne[n_Li][n_Col]=
  {
   {'1', '2', '3', 'A'},
   {'4', '5', '6', 'B'},
   {'7', '8', '9', 'C'},
   {'*', '0', '#', 'D'}
 };
 
Keypad digicode = Keypad( makeKeymap(Paterne), Li_Pins, Col_Pins, n_Li, n_Col );
Servo locker;
LiquidCrystal monEcran(12,11,A3,A2,A1,A0); // on crée l'objet écran

String Mdp ="NULL";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et 2 lignes
  
  locker.attach(10); // Pin de controle du Servo
  Locker_fermer();
}


void loop() {
  // put your main code here, to run repeatedly:
  char choix = Affichage_Menu();
  if (choix == 'A'){
    if(Mdp == "NULL"){
      Mdp = Init_Mdp();
    }else{
      monEcran.clear(); // on efface l'écran
      monEcran.print("Saisir votre"); // on écrit Bonjour
      monEcran.setCursor(0,1); //on passe à la ligne suivante
      monEcran.print("Mot de passe"); 
      Serial.println("Pour Ouvrir");
      Serial.print("Saisir votre MDP:");
      String Saisie_1 = Saisie_Code();
      Serial.println(Saisie_1);
      Verif_Mdp(Mdp,Saisie_1);
    }
  }else if(choix =='B'){
    
    if(Mdp == "NULL"){
      monEcran.clear(); // on efface l'écran
      monEcran.print("Vous avez"); // on écrit Bonjour
      monEcran.setCursor(0,1); //on passe à la ligne suivante
      monEcran.print("aucun code"); 
      Mdp = Init_Mdp();
      
    }else{
      monEcran.clear(); // on efface l'écran
      monEcran.print("Saisir votre"); // on écrit Bonjour
      monEcran.setCursor(0,1); //on passe à la ligne suivante
      monEcran.print("Mot de passe"); 
      String Saisie_2 = Saisie_Code();
      int test =Mdp.compareTo(Saisie_2);
      if(test == 0){ // Bon Mdp
      Mdp = Init_Mdp();
      }else{ // Mauvais Mdp
        
        monEcran.clear(); // on efface l'écran
        monEcran.print("Saisie"); 
        monEcran.setCursor(0,1); //on passe à la ligne suivante
        monEcran.print("Incorrecte"); 
        delay(5000);
      }
    }
  }else{
     monEcran.clear(); // on efface l'écran
     monEcran.print("Saisie"); 
     monEcran.setCursor(0,1); //on passe à la ligne suivante
     monEcran.print("Incorrecte"); 
     delay(2500);
  }
  choix=NO_KEY;
}

/// Fonction Locker
void Locker_ouvert(){
  locker.write(168);
  monEcran.clear(); // on efface l'écran
  monEcran.print("Ouverture"); 
  Serial.println("Verrou Ouvert");
  delay(1500);
}
void Locker_fermer(){
  locker.write(60);
  monEcran.clear(); // on efface l'écran
  monEcran.print("Fermeture"); 
  Serial.println("Verrou Fermer");
  delay(1500);
}

/// Fonctions MDP
char Attente_pression(){
  char keypressed = digicode.getKey();
  while (keypressed == NO_KEY){
    keypressed = digicode.getKey();
  }
  return keypressed;
}

String Saisie_Code(){
  String Saisie;
  for(int i=0; i<5; i++){
    char keypressed = Attente_pression();
    Saisie = Saisie +(keypressed);
  }
  return Saisie;
}

String Init_Mdp(){
  //Affichier Veuiller saisir votre nouveau MDP
  Serial.println("Veuiller sasir votre Nouveau MDP");
  monEcran.clear(); // on efface l'écran
  monEcran.print("Saisissez le"); 
  monEcran.setCursor(0,1); //on passe à la ligne suivante
  monEcran.print("nouveau code"); 
  String temp = Saisie_Code();
  monEcran.clear(); // on efface l'écran
  monEcran.print("Votre code est:"); 
  monEcran.setCursor(0,1); //on passe à la ligne suivante
  monEcran.print(temp); 
  Serial.println("Votre Mot de Passe est: "+temp);
  delay(1500);
  return temp ;
}

int Verif_Mdp(String Mdp , String Saisie){
  int temp = Saisie.compareTo(Mdp);
  if(temp == 0){ // Indentique
    monEcran.clear(); // on efface l'écran
    monEcran.print("Mot de Passe :"); 
    monEcran.setCursor(0,1); //on passe à la ligne suivante
    monEcran.print("Correct"); 
    compteur=0;
    Serial.println("Mot de Passe : Correct");
    Locker_ouvert();
    delay(1500);
    Refermeture();
    return 1;
  }else{ // Differant
    monEcran.clear(); // on efface l'écran
    monEcran.print("Mot de Passe :"); 
    monEcran.setCursor(0,1); //on passe à la ligne suivante
    monEcran.print("Incorrect"); 
    Serial.println("Mot de Passe : Incorrect");
    Locker_fermer();
    delay(2500);
    if(compteur!=0){
        if(millis()-tps<10000){
          compteur++;
        }
        else{
          compteur=0;
        }
    }
    if(compteur==0){
      compteur++;
      tps=millis();
    }
    if(compteur==3){
      compteur=0;
      monEcran.clear(); // on efface l'écran
      monEcran.print("Trop de saisies"); 
      monEcran.setCursor(0,1); //on passe à la ligne suivante
      monEcran.print("Incorrect"); 
      Serial.println("incorrectes 5min");
      delay(300000);
      
      
    }

    
    return 0; 
  }
}
void Refermeture(){
  char temp = '0';
  Serial.println("Veuillez saisir A pour fermer");
  monEcran.clear(); // on efface l'écran
  monEcran.print("Veuillez saisir"); 
  monEcran.setCursor(0,1); //on passe à la ligne suivante
  monEcran.print("A pour fermer"); 
  while(temp != 'A'){
    temp = Attente_pression();
  }
  Locker_fermer();
}
char Affichage_Menu(){
  char keypressed = digicode.getKey();
  while (keypressed == NO_KEY){
    keypressed = digicode.getKey();
    
    monEcran.clear(); // on efface l'écran
    monEcran.print("Deverouiller"); // on écrit Bonjour
    monEcran.setCursor(0,1); //on passe à la ligne suivante
    monEcran.print("Entrez : A"); // on finit d'écrire
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    monEcran.clear(); // on efface l'écran
    monEcran.print("Changer le code"); // on écrit Bonjour
    monEcran.setCursor(0,1); //on passe à la ligne suivante
    monEcran.print("Entrez : B"); // on finit d'écrir
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    delay(250);
    if(keypressed != NO_KEY){
      break;
    }
    keypressed = digicode.getKey();
    
  }
  
  Serial.print(keypressed);
  return keypressed;
}
