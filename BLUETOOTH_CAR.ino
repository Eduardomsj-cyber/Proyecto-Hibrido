#include <AFMotor.h>  // Librería para controlar motores con el shield Adafruit Motor Shield (L293D/L298N compatible)

// Objetos que representan motores conectados a los puertos M1, M2, M3, M4 del shield
AF_DCMotor MotorSupIzq(1);    // M1 - Motor superior izquierdo
AF_DCMotor MotorInferior(2);  // M2 - Motores inferiores (izquierdo y derecho)
AF_DCMotor MotorAgua(3);      // M3 - Motor de bomba de agua
AF_DCMotor MotorSupDer(4);    // M4 - Motor superior derecho

const int buzPin = 8;   // Pin digital 8 → buzzer (PB0 en ATmega328P)
const int ledPin = 10;  // Pin digital 10 → LED (PB2 en ATmega328P)
int valSpeed = 255;     // Velocidad predeterminada de los motores (máximo)

void setup() {
  Serial.begin(9600);  // Inicializa comunicación serial para recibir comandos

  // 🧠 Ensamblador AVR: configurar pines como salida usando SBI (Set Bit in I/O Register)
  // DDRB = Data Direction Register para el puerto B
  // sbi DDRB, 0 → establece PB0 (pin 8) como salida
  // sbi DDRB, 2 → establece PB2 (pin 10) como salida
  asm volatile(
    "sbi %[ddrb], 0\n"
    "sbi %[ddrb], 2\n"
    :
    : [ddrb] "I" (_SFR_IO_ADDR(DDRB))  // Dirección del registro DDRB
  );

  // Establecer velocidad a todos los motores (255 es velocidad máxima)
  MotorSupIzq.setSpeed(valSpeed);
  MotorInferior.setSpeed(valSpeed);
  MotorSupDer.setSpeed(valSpeed);
  MotorAgua.setSpeed(255);  // Bomba de agua siempre a máxima velocidad

  // Dejar los motores en estado RELEASE (sin movimiento al iniciar)
  MotorSupIzq.run(RELEASE);
  MotorInferior.run(RELEASE);
  MotorSupDer.run(RELEASE);
  MotorAgua.run(RELEASE);
}

void loop() {
  while (Serial.available() > 0) {
    char command = Serial.read();     // Leer el carácter enviado por Serial
    Serial.println(command);          // Mostrar el comando recibido (debug)

    switch (command) {
      // 🚗 Comandos de movimiento

      case 'F':   // Avanzar
        SetSpeed(valSpeed);
        MotorSupIzq.run(FORWARD);
        MotorSupDer.run(FORWARD);
        MotorInferior.run(FORWARD);
        break;

      case 'B':   // Retroceder
        SetSpeed(valSpeed);
        MotorSupIzq.run(BACKWARD);
        MotorSupDer.run(BACKWARD);
        MotorInferior.run(BACKWARD);
        break;

      case 'R':   // Girar a la derecha
        SetSpeed(valSpeed);
        MotorSupIzq.run(FORWARD);
        MotorSupDer.run(BACKWARD);
        MotorInferior.run(FORWARD);
        break;

      case 'L':   // Girar a la izquierda
        SetSpeed(valSpeed);
        MotorSupIzq.run(BACKWARD);
        MotorSupDer.run(FORWARD);
        MotorInferior.run(BACKWARD);
        break;

      case 'G':   // Adelante izquierda (marcha más suave de ese lado)
        MotorSupIzq.setSpeed(valSpeed / 4);
        MotorInferior.setSpeed(valSpeed / 4);
        MotorSupIzq.run(FORWARD);
        MotorSupDer.run(FORWARD);
        MotorInferior.run(FORWARD);
        break;

      case 'H':   // Adelante derecha
        MotorSupDer.setSpeed(valSpeed / 4);
        MotorSupIzq.run(FORWARD);
        MotorSupDer.run(FORWARD);
        MotorInferior.run(FORWARD);
        break;

      case 'I':   // Atrás izquierda
        MotorSupIzq.setSpeed(valSpeed / 4);
        MotorInferior.setSpeed(valSpeed / 4);
        MotorSupIzq.run(BACKWARD);
        MotorSupDer.run(BACKWARD);
        MotorInferior.run(BACKWARD);
        break;

      case 'J':   // Atrás derecha
        MotorSupDer.setSpeed(valSpeed / 4);
        MotorSupIzq.run(BACKWARD);
        MotorSupDer.run(BACKWARD);
        MotorInferior.run(BACKWARD);
        break;

      case 'S':   // Detener todos los motores
        MotorSupIzq.run(RELEASE);
        MotorSupDer.run(RELEASE);
        MotorInferior.run(RELEASE);
        break;

      // 🔊 Claxon con asm AVR
      case 'Y':
        // Encender buzzer
        asm volatile("sbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (0));
        delay(200);
        asm volatile("cbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (0));
        delay(80);
        asm volatile("sbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (0));
        delay(300);
        asm volatile("cbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (0));
        break;

      // 💡 Encender luces y motor de agua (ensamblador)
      case 'X':
        // LED ON usando ensamblador
        asm volatile("sbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (2));
        MotorAgua.run(FORWARD);
        break;

      // 🔌 Apagar luces y motor de agua
      case 'x':
        // LED OFF con ensamblador
        asm volatile("cbi %[port], %[pin]" :: [port] "I" (_SFR_IO_ADDR(PORTB)), [pin] "I" (2));
        MotorAgua.run(RELEASE);
        break;

      // 🎚 Cambiar velocidad con teclas 0–9
      case '0': SetSpeed(0); break;
      case '1': SetSpeed(25); break;
      case '2': SetSpeed(50); break;
      case '3': SetSpeed(75); break;
      case '4': SetSpeed(100); break;
      case '5': SetSpeed(125); break;
      case '6': SetSpeed(150); break;
      case '7': SetSpeed(175); break;
      case '8': SetSpeed(200); break;
      case '9': SetSpeed(255); break;
    }
  }
}

// 🎯 Función para aplicar velocidad a todos los motores
void SetSpeed(int val) {
  // Ensamblador opcional: mueve valor a registro r24
  asm volatile(
    "mov r24, %[vel]\n"
    :
    : [vel] "r" (val)
    : "r24"
  );

  valSpeed = val;
  MotorSupIzq.setSpeed(val);
  MotorSupDer.setSpeed(val);
  MotorInferior.setSpeed(val);
}
