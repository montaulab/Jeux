#include <Adafruit_NeoPixel.h>

const int nbled=79; // nombre de led du ruban

unsigned long t0, t1, t_r, t_b; //les tempos t1  pour fonction dégrade, t_r vitesse balle rouge, t_b vitesse balle bleu

Adafruit_NeoPixel led = Adafruit_NeoPixel(nbled, 6, NEO_GRB + NEO_KHZ800);

int p_c = nbled/2; // position du centre

int l_c_r; // largeur du curseur rouge
int l_c_b; // largeur du curseur bleu

int pos_r; // position de la balle rouge
int pos_b; // position de la balle bleu

int vt_r; // vitesse de la balle rouge
int vt_b; // vitesse de la balle bleu

int dir_r; // direction balle rouge
int dir_b; // direction balle bleu

int m_d_r; // pour un appuis correct sur bp rouge dans les limites
int m_d_b; // pour un appuis correct sur bp bleu dans les limites

bool tg; // autorise la tempo si gangé

int in_r,m_in_r,in_b,m_in_b; // antirebond des boutons

uint32_t coul[]={0x330000 , 0x000033 , 0x002200 , 0x222200 , 0x220022 , 0x002222}; // mémo de la couleur

/*######################  setup  #############################*/
void setup() {
   
  led.begin(); // active le ruban de led

  pinMode(7,OUTPUT); // masse des inter
  pinMode(8,INPUT_PULLUP); // bouton poussoir rouge 
  pinMode(9,INPUT_PULLUP); // bouton poussoir bleu 

  _init(); // initialise les valeurs

 }

/*######################  loop  #############################*/
void loop() {
  t0=millis(); // mémorise le temps  

  if (l_c_r < 4) { // si largeur de raquette rouge < 4 
    gagne(0); // rouge gagne
    return; // évite de lire le reste du programme si un gagnant
  }
  else if ( l_c_b < 4) {// si largeur de raquette bleu < 4 
    gagne(1); // bleu gagne
    return; // évite de lire le reste du programme si un gagnant
  } 

  int c_r = p_c - l_c_r; // curseur de gauche
  int c_b = p_c + l_c_b; // curseur de droite

  led.setPixelColor( nbled/2 , coul[2] ); // point vert au centre

  for(int i = c_r +1 ; i < p_c; i++)  led.setPixelColor( i , 0x010000  ); // raquette rouge
  led.setPixelColor( c_r , coul[0]  ); // curseur rouge

  for(int i = p_c + 1; i < c_b; i++)  led.setPixelColor( i , 0x000001  ); // raquette bleu
  led.setPixelColor( c_b , coul[1] );  //  curseur bleu
  
  led.setPixelColor( pos_r , coul[0] );  // balle rouge
  led.setPixelColor( pos_b , coul[1] );  // balle bleu

  if (!digitalRead(8)){ // bp rouge 
    if (!m_in_r) { // mémoire antirebond 
      in_r = true; // valide l'appuis sur le bouton rouge
      m_in_r = true;// // mémoire antirebond 
    } else in_r = false; // dé-valide l'appuis sur le bouton rouge 
    led.setPixelColor( pos_r , 0xffffff );
  }

  if (!digitalRead(9)){ // bp bleu 
    if (!m_in_b) {// mémoire antirebond 
      in_b = true; // valide l'appuis sur le bouton bleu
      m_in_b = true;// // mémoire antirebond 
    } else in_b = false; // dé-valide l'appuis sur le bouton bleu   
    led.setPixelColor( pos_b , 0xffffff ); 
  }  
  
  
  if (in_r){ // appuis sur le bouton rouge
    if ( pos_r > c_r and pos_r < p_c ){ //si la balle est entre les curseurs
      vt_r--; // baisse la vitesse de la balle rouge
      l_c_r--; // baisse la largeur de détection rouge   
      m_d_r = false; // pour un appuis correct sur bp rouge 
    }  
  } 

  if (in_b){ // appuis sur le bouton bleu
    if ( pos_b > p_c and pos_b < c_b ){ //si la balle est entre les curseurs
      vt_b--; // baisse la vitesse de la balle bleu
      l_c_b--; // baisse la largeur de détection bleu   
      m_d_b = false;// pour un appuis correct  sur bp bleu
    }   
  }   

  if ( t0 > t_r ){ // tempo de vitesse balle rouge
    t_r = t0 + vt_r;   // initialise la tempo 
    pos_r += dir_r; // déplace la balle
    if ( pos_r > nbled-1 or pos_r < 0 ) { // si la balle est sur un bord
      dir_r = -dir_r; // inverce la direction de la balle
      if (vt_r < 20 and m_d_r){ // si en dehors de la raquette
        vt_r++; // ralenti la balle
        l_c_r++;// agrandi la raquette
      }  
      m_d_r = true;  // appuis incorect
      m_in_r = false; // antirebond bp rouge
    }         
  }

  if ( t0 > t_b ){ // tempo de vitesse balle bleu
    t_b = t0 + vt_b;  // initialise la tempo   
    pos_b += dir_b; // déplace la balle
    if ( pos_b > nbled-1 or pos_b < 0 ) {// si la balle est sur un bord
      dir_b = -dir_b;// inverce la direction de la balle
      if (vt_b<20 and m_d_b){ // si en dehors de la raquette
        vt_b++;// ralenti la balle
        l_c_b++;// agrandi la raquette
      }  
      m_d_b = true; // appuis incorect
      m_in_b = false; // antirebond bp bleu
    }         
  }

  led.show(); // affiche les leds
  led.clear(); // efface les leds
} 

/*######################  _init  #############################*/
void _init(){

 tg = true; // autorisation d'un gagnant
 
 l_c_r = 20; // longeur de la raquette rouge
 l_c_b = 20; // longeur de la raquette bleu

 pos_r = 0; // position de la balle rouge
 pos_b = nbled; // position de la balle bleu

 vt_r = 20; // vitesse de la balle rouge
 vt_b = 20; // vitesse de la balle rouge

 dir_r = 1; // direction balle rouge
 dir_b = -1; // direction balle rouge

 m_d_r = dir_r; // mémoire de la direction du pion rouge
 m_d_b = dir_b; // mémoire de la direction du pion bleu

m_in_r = in_r = false; // anti rebond du bouton rouge
m_in_b = in_b = false; // anti rebond du bouton bleu

}

/*######################  gagne  #############################*/
void gagne(int c){
  // ici t_b temps pour afficher le gagnant et t_r vitesse de défilement

  if (tg) t_b = t0 + 5000; // enclanche une tempo de 5s
  tg = false; // empèche l'init de la tempo 
    
  if ( t0 > t_r ){ // si t_r est fini
    t_r = t0 + 20;  // réarme t_r à 20ms vitesse de défilement 
    pos_r += dir_r; // position du spot
    if ( pos_r > nbled-1 or pos_r < 0 ) { // si en buté
      dir_r = -dir_r; // change la direction du spot
    }         
  }
  led.setPixelColor(pos_r,coul[c]); // affiche les leds de la couleur du gagnant
  degrad(30); // éfface les leds en douceur
  
  led.show(); // affiche les leds

  if (t0 > t_b) _init(); // au bout de 5s on recommence le jeu
  
}

/*######################  degrad  #############################*/
void degrad(int t){ // t détermine la vitesse d'extinction des leds
  if (t0>t1){
    for(int i=0; i<nbled; i++) { 
      uint32_t c = led.getPixelColor(i); // extraction des couleurs
      byte r =   c / 0x10000;
      byte v = ( c & 0xFF00)/0x100;
      byte b =   c & 0xFF;
      led.setPixelColor(i,r/1.05,v/1.05, b/1.05); // baisse la luminosité des leds
    }
   t1 = t0 + t;
  }

}
