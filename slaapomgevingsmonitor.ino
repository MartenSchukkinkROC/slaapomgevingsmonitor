#include <Servo.h>  // Standaard lib
#include <DHT.h>

#define DHT_TYPE DHT22
#define DHT_PIN 4     // digital
#define LIGHT_PIN A0  // analog
#define NOISE_PIN A1  // analog

#define THUMB_UP 179
#define THUMB_DOWN 1

DHT dht(DHT_PIN, DHT_TYPE);
Servo servo_light;
Servo servo_hum;
Servo servo_temp;
Servo servo_noise;

const int numReadings = 20;

float lightReadings[numReadings];
float temperatureReadings[numReadings];
float humidityReadings[numReadings];
int noiseReadings[numReadings];
int readIndex = 0;          // the index of the current reading

void  setup()
{
  Serial.begin(9600);
  Serial.println("Initialising...");
  pinMode(LIGHT_PIN, INPUT);
  pinMode(NOISE_PIN, INPUT);
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
    humidityReadings[thisReading] = dht.readTemperature();
    noiseReadings[thisReading] = noiseReading();
    // no need to delay, since noiseReading takes (numNoiseReadings * delayNoiseReadings) ms
  }
}

void loop()
{
// === SENSORS (INPUT) ===
  // 1) LIGHT
  float curLight = analogRead(LIGHT_PIN) / 1024.0 * 100.0; // 0..1023 converted to percentage
  lightReadings[readIndex] = curLight;
  float avgLight = getAvg(lightReadings, numReadings);

  // 2) HUMIDITY
  float curHumidity = dht.readHumidity();
  humidityReadings[readIndex] = curHumidity;
  float avgHumidity = getAvg(humidityReadings, numReadings);

  // 3) TEMPERATURE
  float curTemperature = dht.readTemperature();
  temperatureReadings[readIndex] = curTemperature;
  float avgTemperature = getAvg(temperatureReadings, numReadings); 


  // 4) NOISE
  int curNoise = noiseReading();
  noiseReadings[readIndex] = curNoise;
  int avgNoise = getAvg(noiseReadings, numReadings);

  readIndex = readIndex + 1;

  // if we're at the end of the array wrap around to the beginning
  if (readIndex >= numReadings) {
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
  servo_noise.write(avgNoise < 10 ? THUMB_UP : THUMB_DOWN);

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
  Serial.print("\t");
  Serial.print("curNoise:");
  Serial.print(curNoise);
  Serial.print("\t");
  Serial.print("avgNoise:");
  Serial.println(avgNoise);

  // no need to delay the loop, since noiseReading takes (numNoiseReadings * delayNoiseReadings) ms
}

int noiseReading() {
  const int numNoiseReadings = 20;
  const int delayNoiseReadings = 25;

  int minNoiseReading = 1023;
  int maxNoiseReading = 0;

  for (int i = 0; i < numNoiseReadings; i++) {
    int curNoiseReading = analogRead(NOISE_PIN);
    if (curNoiseReading < minNoiseReading) minNoiseReading = curNoiseReading;
    if (curNoiseReading > maxNoiseReading) maxNoiseReading = curNoiseReading;
    delay(delayNoiseReadings);
  }

  int deltaNoiseReading = maxNoiseReading - minNoiseReading;
  return deltaNoiseReading;
}

// Returns the average of an array of floats
float getAvg(float arr[], int size) {
  float sum = 0.0;
  
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  
  return sum / size;
}

// Returns the average of an array of ints
int getAvg(int arr[], int size) {
  int sum = 0;
  
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  
  return sum / size;
}

// // Returns the biggest of an array of floats
// int getMax(int arr[], int size) {
//   int max = 0;
  
//   for (int i = 0; i < size; i++) {
//     if (arr[i] > max)  max = arr[i];
//   }
  
//   return max;
// }