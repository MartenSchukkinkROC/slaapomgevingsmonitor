#include <Servo.h>  // Standaard lib
#include <DHT.h>

#define DHT_TYPE DHT22
#define DHT_PIN 4     // digital
#define LIGHT_PIN A0  // analog

// 
#define THUMB_UP 179
#define THUMB_DOWN 1

DHT dht(DHT_PIN, DHT_TYPE);
Servo servo_light;
Servo servo_hum;
Servo servo_temp;
Servo servo_noise;

const int numReadings = 10;

float lightReadings[numReadings];
float temperatureReadings[numReadings];
float humidityReadings[numReadings];
int readIndex = 0;          // the index of the current reading

void  setup()
{
  Serial.begin(9600);
  pinMode(LIGHT_PIN, INPUT);
  dht.begin();         // DHT-Sensor initialiseren
  delay(1000);         // Sensor is langzaam, even wachten
  servo_light.attach(9);
  servo_hum.attach(10);
  servo_temp.attach(11);
  servo_noise.attach(12);

  // Initialise by reading first <numReadings> values
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    lightReadings[thisReading] = analogRead(LIGHT_PIN) / 1024.0 * 100.0;
    temperatureReadings[thisReading] = dht.readHumidity();
    humidityReadings[thisReading] = dht.readTemperature();;
    delay(500);
  }
}

void loop()
{
// === SENSORS (INPUT) ===
  // 1) LIGHT
  float curLight = analogRead(LIGHT_PIN) / 1024.0 * 100.0; // 0..1023 converted to percentage
  lightReadings[readIndex] = curLight;
  float avgLight = avg(lightReadings, numReadings);

  // 2) HUMIDITY
  float curHumidity = dht.readHumidity();
  humidityReadings[readIndex] = curHumidity;
  float avgHumidity = avg(humidityReadings, numReadings);

  // 3) TEMPERATURE
  float curTemperature = dht.readTemperature();
  temperatureReadings[readIndex] = curTemperature;
  float avgTemperature = avg(temperatureReadings, numReadings); 

  // 4) NOISE
  // *** TODO ***

  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // === ACTUATORS (OUTPUT) ===
  // 1) LIGHT SERVO
  servo_light.write(avgLight < 2.0 ? THUMB_UP : THUMB_DOWN);
  
  // 2) HUMIDITY SERVO
  servo_hum.write(avgHumidity > 40.0 && avgHumidity < 60.0 ? THUMB_UP : THUMB_DOWN);

  // 3) TEMPERATURE SERVO
  servo_temp.write(avgTemperature < 18.0 ? THUMB_UP: THUMB_DOWN);
  
  // NOISE SERVO
  servo_noise.write(THUMB_DOWN);

  // === SEND DEBUG INFO TO SERIAL ===
  Serial.print("curHumidity:");
  Serial.print(curHumidity, 2);
  Serial.print("\t");
  Serial.print("avgHumidity:");
  Serial.print(avgHumidity, 2);
  Serial.print("\t");
  Serial.print("curTemperature:");
  Serial.print(curTemperature, 2);
  Serial.print("\t");
  Serial.print("avgTemperature:");
  Serial.print(avgTemperature, 2);
  Serial.print("\t");
  Serial.print("curLight:");
  Serial.print(curLight, 2);
  Serial.print("\t");
  Serial.print("avgLight:");
  Serial.print(avgLight, 2);
  Serial.println();


  delay(500);
}

// int avg(int arr[], int size) {
//   int sum = 0;
  
//   for (int i = 0; i < size; i++) {
//     sum += arr[i];
//   }
  
//   return sum / size;
// }

float avg(float arr[], int size) {
  float sum = 0.0;
  
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  
  return sum / size;
}