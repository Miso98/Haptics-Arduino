//At a k value of .5 , the handle does not return the handle back to the 0 position completely. A K
//value of .6 is my minimum spring constant in which the system still acts well.
//At a k value of 1.25 the system acted normally, at 1.5 the system will oscillate without any sign
//of stopping or the system would apply the force too quickly and the sensor would lose track of
//the proper 0 position and slam into one side. I’d say the max k value for my spring would be
//around 1.25.
//Min = .6; max = 1.25;
//--------------------------------------------------------------------------
// Code to render on Hapkit
//--------------------------------------------------------------------------
// Parameters that define what environment to render
#define ENABLE_VIRTUAL_WALL
#define ENABLE_LINEAR_DAMPING
#define ENABLE_NONLINEAR_FRICTION
#define ENABLE_HARD_SURFACE
#define ENABLE_BUMP_VALLEY
#define ENABLE_TEXTURE
// Includes
#include <math.h>
// Pin declares

int pwrPin = 5; // PWM output pin for motor 1
int dirPin = 8; // direction output pin for motor 1
int sensorPosPin = A2; // input pin for MR sensor
int fsrPin = A3; // input pin for FSR sensor

// Position tracking variables
int updatedPos = 0; // keeps track of the latest updated value of the MR sensor reading
int rawPos = 0; // current raw reading from MR sensor
int lastRawPos = 0; // last raw reading from MR sensor
int lastLastRawPos = 0; // last last raw reading from MR sensor
int flipNumber = 0; // keeps track of the number of flips over the 180deg mark
int tempOffset = 0;
int rawDiff = 0;
int lastRawDiff = 0;
int rawOffset = 0;
int lastRawOffset = 0;
const int flipThresh = 700; // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;

// Kinematics variables
double xh = 0; // position of the handle [m]
double theta_s = 0; // Angle of the sector pulley in deg
double xh_prev; // Distance of the handle at previous time step
double xh_prev2;
double dxh; // Velocity of the handle
double dxh_prev;
double dxh_prev2;
double dxh_filt; // Filtered velocity of the handle
double dxh_filt_prev;
double dxh_filt_prev2;
double x_wall = .005; // [m] wall

// Force output variables
double force = 0; // force at the handle
double Tp = 0; // torque of the motor pulley
double duty = 0; // duty cylce (between 0 and 255)
unsigned int output = 0; // output command to the motor

// --------------------------------------------------------------
// Setup function -- NO NEED TO EDIT
// --------------------------------------------------------------
void setup() { // Set up serial communication
Serial.begin(9600); // Set PWM frequency
setPwmFrequency(pwmPin,1); // Input pins
pinMode(sensorPosPin, INPUT); // set MR sensor pin to be an input
pinMode(fsrPin, INPUT); // set FSR sensor pin to be an input

// Output pins
pinMode(pwmPin, OUTPUT); // PWM pin for motor A
pinMode(dirPin, OUTPUT); // dir pin for motor A

// Initialize motor
analogWrite(pwmPin, 0); // set to not be spinning (0/255)
digitalWrite(dirPin, LOW); // set direction

// Initialize position valiables
lastLastRawPos = analogRead(sensorPosPin);
lastRawPos = analogRead(sensorPosPin);
}
// --------------------------------------------------------------
// Main Loop
// --------------------------------------------------------------
void loop() {

#define ENABLE_HARD_SURFACE
#define ENABLE_BUMP_VALLEY
#define ENABLE_TEXTURE
// Includes
#include <math.h>
// Pin declares
int pwmPin = 5; // PWM output pin for motor 1
int dirPin = 8; // direction output pin for motor 1
int sensorPosPin = A2; // input pin for MR sensor
int fsrPin = A3; // input pin for FSR sensor
// Position tracking variables
int updatedPos = 0; // keeps track of the latest updated value of the MR sensor reading
int rawPos = 0; // current raw reading from MR sensor
int lastRawPos = 0; // last raw reading from MR sensor
int lastLastRawPos = 0; // last last raw reading from MR sensor
int flipNumber = 0; // keeps track of the number of flips over the 180deg mark
int tempOffset = 0;
int rawDiff = 0;
int lastRawDiff = 0;
int rawOffset = 0;
int lastRawOffset = 0;
const int flipThresh = 700; // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;// Kinematics variables
double xh = 0; // position of the handle [m]
double theta_s = 0; // Angle of the sector pulley in deg
double xh_prev; // Distance of the handle at previous time step
double xh_prev2;
double dxh; // Velocity of the handle
double dxh_prev;
double dxh_prev2;
double dxh_filt; // Filtered velocity of the handle
double dxh_filt_prev;
double dxh_filt_prev2;
double x_wall = .005; // [m] wall

// Force output variables
double force = 0; // force at the handle
double Tp = 0; // torque of the motor pulley
double duty = 0; // duty cylce (between 0 and 255)
unsigned int output = 0; // output command to the motor
}
// --------------------------------------------------------------
// Setup function -- NO NEED TO EDIT
// --------------------------------------------------------------
void setup()
{
// Set up serial communication
Serial.begin(9600); // Set PWM frequency
setPwmFrequency(pwmPin,1); // Input pins
pinMode(sensorPosPin, INPUT); // set MR sensor pin to be an input
pinMode(fsrPin, INPUT); // set FSR sensor pin to be an input

// Output pins
pinMode(pwmPin, OUTPUT); // PWM pin for motor A
pinMode(dirPin, OUTPUT); // dir pin for motor A

// Initialize motor
analogWrite(pwmPin, 0); // set to not be spinning (0/255)
digitalWrite(dirPin, LOW); // set direction

// Initialize position valiables
lastLastRawPos = analogRead(sensorPosPin);
lastRawPos = analogRead(sensorPosPin);
}
// --------------------------------------------------------------
// Main Loop
// --------------------------------------------------------------
void loop()
{
//*************************************************************
//*** Section 1. Compute position in counts (do not change) ***
//*************************************************************
// Get voltage output by MR sensor
rawPos = analogRead(sensorPosPin); //current raw position from MR sensor
// Calculate differences between subsequent MR sensor readings
rawDiff = rawPos - lastRawPos; //difference btwn current raw position and last raw position
lastRawDiff = rawPos - lastLastRawPos; //difference btwn current raw position and last last raw position
rawOffset = abs(rawDiff);
lastRawOffset = abs(lastRawDiff);

// Update position record-keeping vairables
lastLastRawPos = lastRawPos;
lastRawPos = rawPos;

// Keep track of flips over 180 degrees
if((lastRawOffset > flipThresh) && (!flipped)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
    if(lastRawDiff > 0) { // check to see which direction the drive wheel was turning
        flipNumber--; // cw rotation
    } else { // if(rawDiff < 0)
        flipNumber++; // ccw rotation
}
if(rawOffset > flipThresh) { // check to see if the data was good and the most current offset is above the threshold
updatedPos = rawPos + flipNumber*rawOffset; // update the pos value to account for flips over 180deg using the most current offset
tempOffset = rawOffset;
} else { // in this case there was a blip in the data and we want to use lastactualOffset instead
    updatedPos = rawPos + flipNumber*lastRawOffset; // update the pos value to account for any flips over 180deg using the LAST offset
    tempOffset = lastRawOffset;
}
flipped = true; // set boolean so that the next time through the loop won't trigger a flip
} else { // anytime no flip has occurred
    updatedPos = rawPos + flipNumber*tempOffset; // need to update pos based on what most recent offset is
    flipped = false;
}

//*************************************************************
//*** Section 2. Compute position in meters *******************
//*************************************************************
// ADD YOUR CODE HERE (Use your code from Problems 1 and 2)
// Define kinematic parameters you may need
//double rh = ?; //[m]
double r_handle = .09; //[m] radius of handle
// Step B.1: print updatedPos via serial monitor
// Step B.6: double ts = ?; // Compute the angle of the sector pulley (ts) in degrees based on updatedPos
// Step B.7: xh = ?; // Compute the position of the handle (in meters) based on ts (in radians)
// Step B.8: print xh via serial monitor
double ts = .0185*updatedPos + 74.684-90;//substract 90 here because i set straight up as 90 isntead of 0
double ts_rad = ts*3.1415/180;
xh = r_handle * ts_rad;
Serial.println(xh,5);
// Calculate velocity with loop time estimation
dxh = (double)(xh - xh_prev) / 0.001;
// Calculate the filtered velocity of the handle using an infinite impulse response filter
dxh_filt = .9*dxh + 0.1*dxh_prev;
// Record the position and velocity
xh_prev2 = xh_prev;
xh_prev = xh;
dxh_prev2 = dxh_prev;
dxh_prev = dxh;
dxh_filt_prev2 = dxh_filt_prev;
dxh_filt_prev = dxh_filt;
//*************************************************************
//*** Section 3. Assign a motor output force in Newtons *******
//*************************************************************
//*************************************************************
//******************* Rendering Algorithms ********************
//*************************************************************
#ifdef ENABLE_VIRTUAL_WALL
double k = 1.25;
if (xh > x_wall){
force = -1*k*(xh - x_wall);
}
#endif
#ifdef ENABLE_LINEAR_DAMPING
double k = 1.25;
double b = 0.10;
if (xh > x_wall){
force = -1*k*(xh - x_wall) + b*dxh; //wall spring and damping equation
}
#endif
#ifdef ENABLE_BUMP_VALLEY
if (xh > 0 && xh < .025 && dxh < 0){ // if the position is to the right of the 0 and the velocity is backward they're still in the valley and the force goes in the same direction as velocity
    force = .5;
} if (xh > 0 && xh < .025 && dxh > 0) { //if the position is to the right of the 0 and the velocity is forward they're still in the valley and the force goes in the same direction as velocity
    force = -.5;
} if (xh < 0 && xh > -.025 && dxh < 0) { //if the position is to the left of the 0 and the velocity is backward they're still in the hill and the force goes in the opposite direction as velocity
    force =.5;
}if (xh < 0 && xh > -.025 && dxh > 0) { //if the position is to the left of the 0 and the velocity is forward they're still in the hill and the force goes in the opposite direction as velocity
    force =-.5;
}
#endif
#ifdef ENABLE_HARD_SURFACE
#endif
#ifdef ENABLE_NONLINEAR_FRICTION
#endif
#ifdef ENABLE_TEXTURE
#endif
// ADD YOUR CODE HERE (Use your previous code)
// Define kinematic parameters you may need
//double rp = ?; //[m]
//double rs = ?; //[m]
double r_sector = .075; //[m] radius of sector
double r_pulley = .005; //[m] radius pulley
// force = 2.5;
// if (xh > x_wall){
// force = -1*k*(xh - x_wall);
// }
Tp = force*r_handle*r_pulley/ r_sector;
// Step C.1: force = ?; // You can generate a force by assigning this to a constant number (in Newtons) or use a haptic rendering / virtual environment
// Step C.2: Tp = ?; // Compute the require motor pulley torque (Tp) to generate that force using kinematics
//*************************************************************
//*** Section 4. Force output (do not change) *****************
//*************************************************************
// Determine correct direction for motor torque
if(force > 0) {
digitalWrite(dirPin, HIGH);
} else {
digitalWrite(dirPin, LOW);
}
// Compute the duty cycle required to generate Tp (torque at the motor pulley)
duty = sqrt(abs(Tp)/0.03);
// Make sure the duty cycle is between 0 and 100%
if (duty > 1) {
duty = 1;
} else if (duty < 0) {
duty = 0;
}
output = (int)(duty* 255); // convert duty cycle to output signal
analogWrite(pwmPin,output); // output the signal
}
// --------------------------------------------------------------
// Function to set PWM Freq -- DO NOT EDIT
// --------------------------------------------------------------
void setPwmFrequency(int pin, int divisor) {
    byte mode;
    if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
        switch(divisor) {
            case 1: mode = 0x01; break;
            case 8: mode = 0x02; break;
            case 64: mode = 0x03; break;
            case 256: mode = 0x04; break;
            case 1024: mode = 0x05; break;
        default: return;
    }
    if(pin == 5 || pin == 6) {
        TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
        TCCR1B = TCCR1B & 0b11111000 | mode;
    }
    } else if(pin == 3 || pin == 11) {
        switch(divisor) {
            case 1: mode = 0x01; break;
            case 8: mode = 0x02; break;
            case 32: mode = 0x03; break;
            case 64: mode = 0x04; break;
            case 128: mode = 0x05; break;
            case 256: mode = 0x06; break;
            case 1024: mode = 0x7; break;
            default: return;
        }
            TCCR2B = TCCR2B & 0b11111000 | mode;
    }
}