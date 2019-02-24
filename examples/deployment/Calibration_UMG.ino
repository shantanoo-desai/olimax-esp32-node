#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Preferences.h>


/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (50)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

Preferences Calibration;

/**************************************************************************/
/*
    Display some basic info about the sensor status
    */
/**************************************************************************/
void displaySensorStatus(void)
{
    /* Get the system status values (mostly for debugging purposes) */
    uint8_t system_status, self_test_results, system_error;
    system_status = self_test_results = system_error = 0;
    bno.getSystemStatus(&system_status, &self_test_results, &system_error);

    /* Display the results in the Serial Monitor */
    Serial.println("");
    Serial.print("System Status: 0x");
    Serial.println(system_status, HEX);
    Serial.print("Self Test:     0x");
    Serial.println(self_test_results, HEX);
    Serial.print("System Error:  0x");
    Serial.println(system_error, HEX);
    Serial.println("");
    delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
    */
/**************************************************************************/
void displayCalStatus(void)
{
    /* Get the four calibration values (0..3) */
    /* Any sensor data reporting 0 should be ignored, */
    /* 3 means 'fully calibrated" */
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    /* The data should be ignored until the system calibration is > 0 */
    Serial.print("\t");
    if (!system)
    {
        Serial.print("! ");
    }

    /* Display the individual values */
    Serial.print("Sys:");
    Serial.print(system, DEC);
    Serial.print(" G:");
    Serial.print(gyro, DEC);
    Serial.print(" A:");
    Serial.print(accel, DEC);
    Serial.print(" M:");
    Serial.print(mag, DEC);
}

/**************************************************************************/
/*
    Display the raw calibration offset and radius data
    */
/**************************************************************************/
void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
    Serial.print("Accelerometer: ");
    Serial.print(calibData.accel_offset_x); Serial.print(" ");
    Serial.print(calibData.accel_offset_y); Serial.print(" ");
    Serial.print(calibData.accel_offset_z); Serial.print(" ");

    Serial.print("\nGyro: ");
    Serial.print(calibData.gyro_offset_x); Serial.print(" ");
    Serial.print(calibData.gyro_offset_y); Serial.print(" ");
    Serial.print(calibData.gyro_offset_z); Serial.print(" ");

    Serial.print("\nMag: ");
    Serial.print(calibData.mag_offset_x); Serial.print(" ");
    Serial.print(calibData.mag_offset_y); Serial.print(" ");
    Serial.print(calibData.mag_offset_z); Serial.print(" ");

    Serial.print("\nAccel Radius: ");
    Serial.print(calibData.accel_radius);

    Serial.print("\nMag Radius: ");
    Serial.print(calibData.mag_radius);
}


/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
    */
/**************************************************************************/
void setup(void)
{
    Wire.begin(13, 16, 400000);
    Serial.begin(115200);
    delay(1000);
    Serial.println("Orientation Sensor Test"); Serial.println("");

    /* Initialise the sensor */
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }
    Calibration.remove("offsets");
    Calibration.begin("offsets", false);
    Calibration.clear();
    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    int32_t bnoID = Calibration.getInt("bnoID");
     bool foundCalib = false;
    /*
    *  Look for the sensor's unique ID at the beginning oF EEPROM.
    *  This isn't foolproof, but it's better than nothing.
    */
    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id)
    {
        Serial.println("\nNo Calibration Data for this sensor exists in NVS");
        delay(500);
    }
    else
    {
        Serial.println("\nFound Calibration for this sensor in NVS.");

        // Accelerometer offsets
        calibrationData.accel_offset_x = Calibration.getInt("acc_off_x");
        calibrationData.accel_offset_y = Calibration.getInt("acc_off_y");
        calibrationData.accel_offset_z = Calibration.getInt("acc_off_z");

        // Gyroscrope offsets
        calibrationData.gyro_offset_x = Calibration.getInt("gyr_off_x");
        calibrationData.gyro_offset_y = Calibration.getInt("gyr_off_y");
        calibrationData.gyro_offset_z = Calibration.getInt("gyr_off_z");

        // Magnetometer offsets
        calibrationData.mag_offset_x = Calibration.getInt("mag_off_x");
        calibrationData.mag_offset_y = Calibration.getInt("mag_off_y");
        calibrationData.mag_offset_z = Calibration.getInt("mag_off_z");

        displaySensorOffsets(calibrationData);

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        foundCalib = true;
    }

    delay(1000);

    /* Display some basic information on this sensor */
//    displaySensorDetails();

    /* Optional: Display current status */
    displaySensorStatus();

   //Crystal must be configured AFTER loading calibration data into BNO055.
    bno.setExtCrystalUse(true);

    sensors_event_t event;
    bno.getEvent(&event);
    if (foundCalib){
        Serial.println("Move sensor slightly to calibrate magnetometers");
        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);
            imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
            imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
            imu::Vector<3> mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);


              // Print the sensor data
              Serial.print("Raw:");
              Serial.print((int)round(accel.x()*100));  // meters/sec/sec * 100
              Serial.print(',');
              Serial.print((int)round(accel.y()*100));
              Serial.print(',');
              Serial.print((int)round(accel.z()*100));
              Serial.print(',');
              Serial.print((int)round(gyro.x()*900));  // radians/sec * 900
              Serial.print(',');
              Serial.print((int)round(gyro.y()*900));
              Serial.print(',');
              Serial.print((int)round(gyro.z()*900));
              Serial.print(',');
              Serial.print((int)round(mag.x()*16));  // microteslas * 16
              Serial.print(',');
              Serial.print((int)round(mag.y()*16));
              Serial.print(',');
              Serial.print((int)round(mag.z()*16));
              Serial.println();
//            displayCalStatus();
//            Serial.println("");
            delay(BNO055_SAMPLERATE_DELAY_MS);
        }
    }
    else
    {
        Serial.println("Please Calibrate Sensor: ");
        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);

            Serial.print("X: ");
            Serial.print(event.orientation.x, 4);
            Serial.print("\tY: ");
            Serial.print(event.orientation.y, 4);
            Serial.print("\tZ: ");
            Serial.print(event.orientation.z, 4);

            /* Optional: Display calibration status */
            displayCalStatus();

            /* New line for the next sample */
            Serial.println("");

            /* Wait the specified delay before requesting new data */
            delay(BNO055_SAMPLERATE_DELAY_MS);
        }
    }

    Serial.println("\nFully calibrated!");
    Serial.println("--------------------------------");
    Serial.println("Calibration Results: ");
    adafruit_bno055_offsets_t newCalib;
    bno.getSensorOffsets(newCalib);
    displaySensorOffsets(newCalib);

    Serial.println("\n\nStoring calibration data to NVS...");

    bno.getSensor(&sensor);
    bnoID = sensor.sensor_id;
    // enter the Sensor ID
    Calibration.putInt("bnoID", bnoID);

    // store new Accelerometer offsets
    Calibration.putInt("acc_off_x", newCalib.accel_offset_x);
    Calibration.putInt("acc_off_y", newCalib.accel_offset_y);
    Calibration.putInt("acc_off_z", newCalib.accel_offset_z);

    // store new Gyroscope offsets
    Calibration.putInt("gyr_off_x", newCalib.gyro_offset_x);
    Calibration.putInt("gyr_off_y", newCalib.gyro_offset_y);
    Calibration.putInt("gyr_off_z", newCalib.gyro_offset_z);

    // store new Magnetometer offsets
    Calibration.putInt("mag_off_x", newCalib.mag_offset_x);
    Calibration.putInt("mag_off_y", newCalib.mag_offset_y);
    Calibration.putInt("mag_off_z", newCalib.mag_offset_z);
    Serial.println("Data stored to NVS..");

    Serial.println("\n--------------------------------\n");
    delay(500);
}

void loop() {
    /* Get a new sensor event */
    sensors_event_t event;
    bno.getEvent(&event);

    /* Display the floating point data */
    Serial.print("X: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tY: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: ");
    Serial.print(event.orientation.z, 4);

    /* Optional: Display calibration status */
    displayCalStatus();

    /* Optional: Display sensor status (debug only) */
    displaySensorStatus();

    /* New line for the next sample */
    Serial.println("");

    /* Wait the specified delay before requesting new data */
    delay(BNO055_SAMPLERATE_DELAY_MS);
}
