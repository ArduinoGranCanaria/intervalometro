/*
   Intervalometro con optoacoplador 4N35 y LCD 1602 + teclado
   Permite configurar numero de fotos, tiempo de exposicion y pausa.
   
   2014 Victor R. Ruiz <rvr@linotipo.es>
*/
#include <Teclas.h>
#include <LiquidCrystal.h>

byte lcd_arrow_up[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
};

byte lcd_arrow_down[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
};

byte lcd_arrow_left[8] = {
  B00000,
  B00100,
  B01100,
  B11111,
  B01100,
  B00100,
  B00000,
};

byte lcd_arrow_right[8] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
};

const int pinFoto = 13;   // D13
const int pinTeclado = 0; // A0

const int min_fotos = 1;
const int max_fotos = 500;

const int min_exposicion = 1;
const int max_exposicion = 500;

const int min_pausa = 1;
const int max_pausa = 10000;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int pantalla = 0;   // 0: Info. 1: Leer num_fotos. 2: Tiempo exposicion. 3: Leer pausa. 4: Tomar fotos.
int num_fotos = 1;  // Numero de fotos a realizar
int exposicion = 1; //
int pausa = 1;      // Pausa (segundos) entre foto y foto

// Inicializacion del sistema
void setup() {
  // Configuracion de pin
  pinMode(pinFoto, OUTPUT);

  // Inicializacion del monitor
  Serial.begin(9600);

  // Inicializacion de la pantalla
  lcd.createChar(0, lcd_arrow_up);
  lcd.createChar(1, lcd_arrow_down);
  lcd.createChar(2, lcd_arrow_left);
  lcd.createChar(3, lcd_arrow_right);
  lcd.begin(16,2);

  // Mostrar pantalla principal
  mostrar_pantalla_principal();
}

// Bucle principal
void loop() {
  Teclas teclaLeida = leer_teclado(true);
  switch(pantalla) {
  case 0:
    // Pantalla de informacion
    switch(teclaLeida) {
    case LEFT:
      cambiar_pantalla(pantalla - 1);
      break;

    case RIGHT:
      cambiar_pantalla(pantalla + 1);
      break;
    }
    break;

  case 1:
    // Pantalla de numero de fotos
    switch(teclaLeida) {
    case LEFT:
      // Pantalla anterior
      cambiar_pantalla(pantalla - 1);
      break;

    case RIGHT:
      // Pantalla siguiente
      cambiar_pantalla(pantalla + 1);
      break;

    case UP:
      // Aumentar numero de fotos
      actualizar_numero_fotos(num_fotos + 1);
      break;

    case DOWN:
      // Disminuir numero de fotos
      actualizar_numero_fotos(num_fotos - 1);
      break;
    }
    break;

  case 2:
    // Pantalla de exposicion
    switch(teclaLeida) {
    case LEFT:
      cambiar_pantalla(pantalla - 1);
      break;

    case RIGHT:
      cambiar_pantalla(pantalla + 1);
      break;

    case UP:
      // Aumentar exposicion
      actualizar_exposicion(exposicion + 1);
      break;

    case DOWN:
      // Disminuir exposicion
      actualizar_exposicion(exposicion - 1);
      break;
    }
    break;

  case 3:
    // Pantalla de pausa
    switch(teclaLeida) {
    case LEFT:
      cambiar_pantalla(pantalla - 1);
      break;

    case RIGHT:
      cambiar_pantalla(pantalla + 1);
      break;

    case UP:
      // Aumentar pausa
      actualizar_pausa(pausa + 1);
      break;

    case DOWN:
      // Disminuir pausa
      actualizar_pausa(pausa - 1);
      break;
    }
    break;

  case 4:
    // Pantalla para tomar fotos
    switch(teclaLeida) {
    case LEFT:
      cambiar_pantalla(pantalla - 1);
      break;
      
    case RIGHT:
      cambiar_pantalla(pantalla + 1);
      break;
      
    case SELECT:
      tomar_fotos(num_fotos, pausa);
      break;
    }
    break;
  }
}

Teclas leer_teclado(boolean pausar) {
  Teclas teclaLeida = NONE;
  int codigo = -1;

  // Leer A0 para conocer que tecla se ha pulsado
  codigo = analogRead(pinTeclado);

  // Convertir lectura resistencia en codigo de tecla
  if (codigo < 100) {
    teclaLeida = RIGHT;
  } 
  else if (codigo < 200) {
    teclaLeida = UP;
  } 
  else if (codigo < 400) {
    teclaLeida = DOWN;
  } 
  else if (codigo < 600) {
    teclaLeida = LEFT;
  } 
  else if (codigo < 800) {
    teclaLeida = SELECT;
  }

  if (teclaLeida != NONE && pausar == true) {
    delay(250);
  }
  return teclaLeida;
}

void cambiar_pantalla(int nueva_pantalla) {
  if (nueva_pantalla < 0) {
    nueva_pantalla = 4;
  } 
  else if (nueva_pantalla > 4) {
    nueva_pantalla = 0;
  }
  
  Serial.println(pantalla);
  Serial.println(nueva_pantalla);

  pantalla = nueva_pantalla;

  switch(pantalla) {
  case 0:
    mostrar_pantalla_principal();
    break;

  case 1:
    mostrar_pantalla_num_fotos();
    break;
    
  case 2:
    mostrar_pantalla_exposicion();
    break;

  case 3:
    mostrar_pantalla_pausa();
    break;

  case 4:
    mostrar_pantalla_fotos();
    break;
  }
}

void mostrar_pantalla_principal() {
  // Mensaje de bienvenida
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Intervalometro ");
  lcd.setCursor(0,1);
  mostrar_flechas_izq_der();
}

void mostrar_pantalla_num_fotos() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fotos: ");
  lcd.print(num_fotos);
  lcd.setCursor(0,1);
  mostrar_flechas();
}

void mostrar_pantalla_exposicion() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Exp.: ");
  lcd.print(exposicion);
  lcd.print("s");
  lcd.setCursor(0,1);
  mostrar_flechas();
}

void mostrar_pantalla_pausa() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pausa: ");
  lcd.print(pausa);
  lcd.print("s");
  lcd.setCursor(0,1);
  mostrar_flechas();
}

void mostrar_pantalla_fotos() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select=Iniciar");
  lcd.setCursor(0,1);
  lcd.print(0);
  lcd.print("/");
  lcd.print(num_fotos);
  lcd.setCursor(0,1);
  mostrar_flechas_izq_der();
}

void mostrar_flechas() {
  // Mostrar flechas menu en la esquina inferior derecha
  // de la pantalla
  lcd.setCursor(12,1);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
}

void mostrar_flechas_izq_der() {
  // Mostrar flechas menu (solo izquierda y derecha)
  // en esquina inferior derecha de la pantalla
  lcd.setCursor(14,1);
  lcd.write(byte(2));
  lcd.write(byte(3));
}

void actualizar_numero_fotos(int nuevo_numero) {
  // Revisar limites
  if (nuevo_numero >= min_fotos && nuevo_numero <= max_fotos) {
    // Actualizar valor
    num_fotos = nuevo_numero;
    // Refrescar pantalla con nuevo valor
    mostrar_pantalla_num_fotos();
  }
}

void actualizar_exposicion(int nueva_exposicion) {
  // Revisar limites
  if (nueva_exposicion >= min_pausa && nueva_exposicion <= max_pausa) {
    // Actualizar valor
    exposicion = nueva_exposicion;
    // Refrescar pantalla con nuevo valor
    mostrar_pantalla_exposicion();
  }
}

void actualizar_pausa(int nueva_pausa) {
  // Revisar limites
  if (nueva_pausa >= min_pausa && nueva_pausa <= max_pausa) {
    // Actualizar valor
    pausa = nueva_pausa;
    // Refrescar pantalla con nuevo valor
    mostrar_pantalla_pausa();
  }
}

void tomar_fotos(int num_fotos, int pausa) {
  // Bucle para tomar 'n' fotos con pausa entre ellas
  boolean cancelar = false;
  int i = 0;
  
  lcd.clear();
  for (i = 0; i < num_fotos; i++) {
    // Mostrar paso actual
    lcd.setCursor(0,0);
    lcd.print("Foto: ");
    lcd.print(i + 1);
    lcd.print("/");
    lcd.print(num_fotos);
    // Activar el rele del optoacoplador
    digitalWrite(pinFoto, HIGH);
    // Dejar activado el disparador durante el
    // tiempo de exposicion indicado
    for (int n = 1; n <= exposicion; n++) {
        lcd.setCursor(0,1);
        lcd.print("Exp:             ");
        lcd.setCursor(5,1);
        lcd.print(exposicion - n + 1);
        lcd.print("s");
        delay(1000);
    }
    // Desactivar el rele del optoacoplador
    digitalWrite(pinFoto, LOW);
    // Hacer Pausa
    if (num_fotos > 0) {
      for (int n = 1; n <= pausa; n++) {
        lcd.setCursor(0,1);
        lcd.print("Pausa:           ");
        lcd.setCursor(7,1);
        lcd.print(pausa - n + 1);
        lcd.print("s");
        delay(1000);
      }
      lcd.setCursor(0,1);
      lcd.print("                 ");
    }
  }
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Fin   Sel=Ini");
  mostrar_flechas_izq_der();
}


