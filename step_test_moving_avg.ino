const int y_pin = A0;
const int u_pin = 6;
const int n = 100; // Number of readings for moving avg
int y_arr[n]; // Output array
int i = 0; // Reading index
int y_sum = 0;
int yr = 0; // Output control reference
int y = 0; // Scaled output moving avg. [0-255]
int u = 0; // Input [0-255]

const int Ts = 1; // [ms]
int plt_steps = 0; // Variable for Serial plotting timing
int ref_steps = 0; // Variable for timing change of reference

void setup() {
  pinMode(u_pin, OUTPUT);
  Serial.begin(57600);
  // Initialize array to 0
  for (int k = 0; k < n; k++) {
    y_arr[k] = 0;
  }
  //Serial.println("yr y u");
  Serial.println("y u");
}

void loop() {
  /* Change control output */
  if (ref_steps == 3000) {
    u = 85;
  } else if (ref_steps == 6000) {
    u = 85;
  } else if (ref_steps == 9000) {
    u = 40;
  } else if (ref_steps == 12000) {
    u = 35;
  } else if (ref_steps == 15000) {
    u = 30;
  } else if (ref_steps == 18000) {
    u = 25;
  } else if (ref_steps == 21000) {
    u = 45;
  } else if (ref_steps == 24000) {
    u = 40;
    ref_steps = 0;
  }

  /* Read output from analog pin */
  y_sum = y_sum - y_arr[i]; // Subtract last reading
  y_arr[i] = map(analogRead(y_pin), 0, 1023, 0, 100); // Read sensor
  y_sum = y_sum + y_arr[i]; // Add new reading
  i = i + 1; // Update reading index
  if (i >= n) { // If at the end of the array...
    i = 0; // Wrap to the beginning
  }
  y = y_sum / n;

  /* Print output to Serial plotter */
  if (plt_steps == 50) {
    Serial.println((String) y+" "+u);
    plt_steps = 0;
  }

  /* Apply control output */
  analogWrite(u_pin, map(u, 0, 100, 0, 255));

  /* Update step counters */
  plt_steps = plt_steps + 1;
  ref_steps = ref_steps + 1;

  /* Wait sampling time */
  delay(Ts);
}
