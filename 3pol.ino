#define PIN_IR A0

float dist_list[7] = {0, 50, 100, 150, 200, 250, 300};
float volt_data[7];
int idx = 0;

unsigned int ir_sensor_filtered(unsigned int n, float position, int verbose);
void poly_fit_3(float *coef);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("엔터 시 거리 측정");
  Serial.print("거리: ");
  Serial.println(dist_list[idx]);
}

void loop() {
  if (!Serial.available()) return;

  String s = Serial.readStringUntil('\n');

  unsigned int v = ir_sensor_filtered(100, 0.5, 0);
  volt_data[idx] = (float)v;

  Serial.print("거리 ");
  Serial.print(dist_list[idx]);
  Serial.print(" cm → 센서값 = ");
  Serial.println(v);

  idx++;

  if (idx < 7) {
    Serial.print("거리: ");
    Serial.println(dist_list[idx]);
    return;
  }


  float coef[4];     // a0, a1, a2, a3 저장
  poly_fit_3(coef);

  // 식 출력
  Serial.println("\n---3차식---");
  Serial.print(coef[0], 6); Serial.print(" + ");
  Serial.print(coef[1], 6); Serial.print("*a_value + ");
  Serial.print(coef[2], 6); Serial.print("*a_value*a_value + ");
  Serial.print(coef[3], 6); Serial.println("*a_value*a_value*a_value");

  while(1);
}


void poly_fit_3(float *coef)
{
  const int N = 7;
  const int M = 4;

  float X[4][4] = {0};
  float Y[4]    = {0};

  // Normal Equation 생성
  for (int r = 0; r < M; r++) {
    for (int c = 0; c < M; c++) {
      for (int i = 0; i < N; i++)
        X[r][c] += pow(volt_data[i], r + c);
    }
    for (int i = 0; i < N; i++)
      Y[r] += dist_list[i] * pow(volt_data[i], r);
  }

  // 가우스 소거법
  for (int i = 0; i < M; i++) {
    float pivot = X[i][i];
    for (int j = 0; j < M; j++) X[i][j] /= pivot;
    Y[i] /= pivot;

    for (int k = 0; k < M; k++) {
      if (k == i) continue;
      float t = X[k][i];
      for (int j = 0; j < M; j++) X[k][j] -= t * X[i][j];
      Y[k] -= t * Y[i];
    }
  }

  for (int i = 0; i < M; i++)
    coef[i] = Y[i];
}



int compare(const void *a, const void *b) {
  return (*(unsigned int *)a - *(unsigned int *)b);
}

unsigned int ir_sensor_filtered(unsigned int n, float position, int verbose)
{
  unsigned int *ir_val, ret_val;

  if (n == 0 || position < 0 || position > 1) return 0;

  ir_val = (unsigned int*)malloc(sizeof(unsigned int) * n);
  if (!ir_val) return 0;

  for (int i = 0; i < n; i++)
    ir_val[i] = analogRead(PIN_IR);

  qsort(ir_val, n, sizeof(unsigned int), compare);
  ret_val = ir_val[(unsigned int)(n * position)];

  free(ir_val);
  return ret_val;
}
