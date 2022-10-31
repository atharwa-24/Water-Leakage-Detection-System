byte statusLed    = 13;
int led = 7;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
byte sensorInterrupt1 = 1;  // 1 = digital pin 3
byte sensorPin1       = 3;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  
volatile byte pulseCount1;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float flowRate1;
unsigned int flowMilliLitres1;
unsigned long totalMilliLitres1;
float difference;

unsigned long oldTime;
unsigned long oldTime1;

void setup()
{
  
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
   
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  pinMode(led, OUTPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pinMode(sensorPin1, INPUT);
  digitalWrite(sensorPin1, HIGH);

  pulseCount         = 0;
  flowRate           = 0.0;
  flowMilliLitres    = 0;
  totalMilliLitres   = 0;
  oldTime            = 0;
  pulseCount1        = 0;
  flowRate1          = 0.0;
  flowMilliLitres1   = 0;
  totalMilliLitres1  = 0;
  oldTime1           = 0;
  difference         = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  attachInterrupt(sensorInterrupt1, pulseCounter1, FALLING);
}

/**
 * Main program loop
 */
void loop()
{
 
delay(100);
   
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
    detachInterrupt(sensorInterrupt1);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    flowRate1 = ((1000.0 / (millis() - oldTime)) * pulseCount1) / calibrationFactor;
    difference = flowRate - flowRate1;
    if(difference>2)
    {digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW); }
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate1));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\n");
    Serial.print("Difference: ");
    Serial.print(int(difference));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\n");

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    pulseCount1 = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    attachInterrupt(sensorInterrupt1, pulseCounter1, FALLING);
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
void pulseCounter1()
{
  // Increment the pulse counter
  pulseCount1++;
}

#

void setup() {
  Serial.begin(9600);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); //CPHA = CPOL = 0    MODE = 0
  delay(1000);

  pinMode(CS_PIN, OUTPUT);
  pinMode(ADXL_INT1_PIN, INPUT);

  adxl372_Get_DevID(&adxl372, &devId);

  Serial.print("Device id: ");
  Serial.println(devId, HEX);

  adxl372_Set_Op_mode(&adxl372, FULL_BW_MEASUREMENT);
  Set_Impact_Detection();
}
