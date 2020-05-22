/* Process variable PIN (Analog) at Arduino board*/
const int y_pin = A0;
/* Control output PIN (PWM) at Arduino board*/
const int u_pin = 6;
/* Number of readings for moving average calculation */
const int n = 100;
/* Process variable output array */
int y_arr[n];
/* Reading index for PV output array */
int i = 0;
/* Process variable summation */
int y_sum = 0;
/* Process variable desired value (reference) */
int yr = 0;
/* Process variable (moving average) */
int y = 0;
/* Control output [0-255] */
int u = 0;
/* Reference tracking error (y-yr) */
int e = 0;
/* Last control output u(k-1) */
int u_anterior = 0;
/* Last reference tracking error e(k-1) */
int e_anterior = 0;
/* Controller operation mode (0|1) */
int AUTO = 0;
/* Loop start timestamp millis() */
unsigned long loopstart = 0;
/* Loop start timestamp millis() */
unsigned long looptime = 0;
/* Controller sampling time [ms] */
const int Ts = 1;
/* Loop counter for Serial plotting time scaling */
int plt_steps = 0;
/* Loop counter for reference signal changing */
int ref_steps = 0;
/* PI integral action gain */
float Ki = 0.744;
/* PI proportional action gain */
float Kp = 2;

void setup() {
  /* Set pinMode for PWM output */
  pinMode(u_pin, OUTPUT);
  /* Set BAUD Rate */
  Serial.begin(57600);
  /* Initialize PV array to 0 */
  for (int k = 0; k < n; k++) {
    y_arr[k] = 0;
  }
  /* Set serial plotter plot labels */
  Serial.println("yr y e u");
}

void loop() {
  /* Save loop start timestamp */
  loopstart = micros();
  /* Change PV reference based on number of loops */
  if (ref_steps == 3000) {
    yr = 68;
  } else if (ref_steps == 6000) {
    /* Turn on AUTO controller after 6000 steps (6 seconds) */
    AUTO = 1;
    u_anterior = u;
    e_anterior = e;
  } else if (ref_steps == 7500) {
    yr = 78;
  } else if (ref_steps == 9000) {
    yr = 83;
    //ref_steps = 0;
  }

  /* Subtract last reading from the PV array */
  y_sum = y_sum - y_arr[i];
  /* Read output from analog pin */
  y_arr[i] = map(analogRead(y_pin), 0, 1023, 0, 100);
  /* Add new reading to the PV array */
  y_sum = y_sum + y_arr[i];
  /* Update PV array reading index */
  i = i + 1;
  /* If at the end of the array... */
  if (i >= n) {
    i = 0; // Wrap to the beginning
  }
  /* Compute PV average value */
  y = y_sum / n;

  /* If 50 loops are completed passed (50ms)... */
  if (plt_steps == 50) {
    /* Print variables to serial plotter */
    Serial.println((String) yr+" "+y+" "+e+" "+u);
    plt_steps = 0;
  }

  /* Calculate reference tracking error */
  e = yr - y;
  e_anterior = e;

  /* Calculate control output */
  if (AUTO == 0){
    u = 75;
    u_anterior = u;
  } else {
    u = u_anterior + 2*e - 1.999*e_anterior;
    u_anterior = u;
  }
  
  /* Saturate control output */
  if (u > 100) {
    u = 100;
  } else if (u < 0) {
    u = 0;
  }

  /* Apply control output (write [0-255] to PWM pin)*/
  analogWrite(u_pin, map(u, 0, 100, 0, 255));

  /* Update loop counters */
  plt_steps = plt_steps + 1;
  ref_steps = ref_steps + 1;
  
  /* Wait sampling time - loop computing time [1ms] */
  looptime = micros()-loopstart
  delayMicroseconds(Ts*1000 - looptime);
}
