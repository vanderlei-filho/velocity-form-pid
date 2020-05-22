const int y_pin = A0;
const int u_pin = 6;

const int n = 20;
int y_arr[n];
int k = 0;
int i = 0;
int j = 0;
int y_sum = 0;
int y = 0;

int bias = 75;
int y_bar = 72;

int u = 0;
int u_past = 0;
int yr = 0;
int yr_arr[10];
int yr_index = 0;
int e = 0;
int e_past = 0;
int AUTO = 0;
float Ki = 0.744;
float Kp = 2;

unsigned long loopstart = 0;
unsigned long looptime = 0;

const int Ts = 50000;

int plot_counter;
int yr_counter;


void setup() {
    pinMode(u_pin, OUTPUT);
    Serial.begin(57600);
    for (k = 0; k < n; k++) {
        y_arr[k] = 0;
    }
    yr_arr[0] = y_bar;
    yr_arr[1] = y_bar+5;
    yr_arr[2] = y_bar+10;
    yr_arr[3] = y_bar+5;
    yr_arr[4] = y_bar;
    yr_arr[5] = y_bar-5;
    yr_arr[6] = y_bar-10;
    yr_arr[7] = y_bar+10;
    yr_arr[8] = y_bar+15;
    yr_arr[9] = y_bar;
    Serial.println("yr y e u");
}


void loop() {
    loopstart = micros();

    /* COMPUTE y MOVING AVERAGE */
    for (j=0; j<20; j++) {
        y_sum = y_sum - y_arr[i];
        y_arr[i] = analogRead(y_pin); //100usec
        y_sum = y_sum + y_arr[i];
        i++;
        if (i >= n) {
            i = 0; // Wrap to the beginning
        }
        delayMicroseconds(100); //1300usec*20usec -> 26000usec = 26msec
    }
    y = map(y_sum/n, 0, 1023, 0, 100);

    /* CHECK y REFERENCE */

    if (AUTO == 0){
        yr = y_bar;
        if (yr_counter == 999) {
            AUTO = 1;
        }
    } else {
        if (yr_counter == 1000) {
            yr_index++;
            if (yr_index >= 10) {
                yr_index = 0; // Wrap to the beginning
            }
            yr_counter = 0;
        }
        yr = yr_arr[yr_index]
    }

    /* COMPUTE ERROR */
    e_past = e;
    e = yr - y;

    /* COMPUTE CONTROL ACTION */
    u_past = u;
    if (AUTO == 0){
        u = 0;
    } else {
        u = u_past + 2*e - 1.46*e_past;
    }
  
    /* SATURATION */
    if (u > 25) {
        u = 25;
    } else if (u < -75) {
        u = -75;
    }

    /* APPLY CONTROL */
    analogWrite(u_pin, map(bias+u, 0, 100, 0, 255));

    /* PLOT */
    if (plot_counter == 9){
        Serial.println((String) yr+" "+y+" "+e+" "+u);
        plot_counter = 0;
    }

    /* UPDATE LOOP COUNTERS */
    yr_counter++;
    plot_counter++;

    /* WAIT SAMPLING TIME */
    looptime = micros()-loopstart;
    delayMicroseconds(Ts - looptime);
}
