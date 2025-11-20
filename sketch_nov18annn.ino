// Arduino pin assignment
#define PIN_IR A0

// configurable parameters
#define LOOP_INTERVAL 50    // Loop Interval (unit: msec)
#define _EMA_ALPHA    0.7   // EMA weight of new sample (range: 0 to 1)
                            // Setting EMA to 1 effectively disables EMA filter.

// global variables
unsigned long last_sampling_time; // unit: msec
int ema;                          // Voltage values from the IR sensor (0 ~ 1023)

void setup()
{
  Serial.begin(1000000);    // 1,000,000 bps

  last_sampling_time = 0;

  //return;
  while(1) {
    while (Serial.available() == 0)
      ;
    Serial.read();  // wait for <Enter> key
    ir_sensor_filtered(7, 0.5, 1);
  }
}

void loop()
{
  unsigned int raw, filtered;           // Voltage values from the IR sensor (0 ~ 1023)

  // wait until next sampling time.
  if (millis() < (last_sampling_time + LOOP_INTERVAL))
    return;
  last_sampling_time += LOOP_INTERVAL;

  // Take a median value from multiple measurements
  filtered = ir_sensor_filtered(10, 0.5, 2);

  // Take a single measurement
  raw = analogRead(PIN_IR);

  // Calculate EMA
  ema = _EMA_ALPHA * ema + (1.0 - _EMA_ALPHA) * raw;

  // Oupput the raw, filtered, and EMA values for comparison purpose
  Serial.print("MIN:"); Serial.print(0);              Serial.print(",");
  Serial.print("RAW:"); Serial.print(raw);            Serial.print(",");
  Serial.print("EMA:"); Serial.print(ema + 100);      Serial.print(",");
  Serial.print("FLT:"); Serial.print(filtered + 200); Serial.print(",");
  Serial.print("MAX:"); Serial.println(900);
}

int compare(const void *a, const void *b) {
  return (*(unsigned int *)a - *(unsigned int *)b);
}

unsigned int ir_sensor_filtered(unsigned int n, float position, int verbose)
{
    unsigned long start_time = micros();   // 전체 시간 측정 시작

    float prod = (float)n * position;
    unsigned int k = (unsigned int)prod;   
    if (prod > (float)k)
        k = k + 1;
        
    if (k == 0) k = 1;
    if (k > n)  k = n;

    unsigned int buf[k];   // 메모리 k개만 사용

    for (int i = 0; i < k; i++)
        buf[i] = 2000;    // 초기 큰값


    for (int i = 0; i < n; i++) {
        unsigned int x = analogRead(PIN_IR);

        if (verbose == 1) {
            Serial.print(" ");
            Serial.print(x);
        }

        for (int j = 0; j < k; j++) {
            if (x < buf[j]) {
                for (int t = k - 1; t > j; t--)
                    buf[t] = buf[t - 1];
                buf[j] = x;
                break;
            }
        }
    }

    unsigned long dt = micros() - start_time;

    Serial.print("  => ");

    Serial.print(buf[k - 1]);

    Serial.print("   | time(us): ");
    Serial.print(dt);

    Serial.print("   | mem(bytes): ");
    Serial.print(sizeof(buf));

    Serial.println();

    return buf[k - 1];
}
