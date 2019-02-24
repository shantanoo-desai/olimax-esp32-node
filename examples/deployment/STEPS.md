## Deployment

1. Calibration needs to be done for each UMG Node.

    - Upload `Calibration_UMG.ino` script to UMG Node and calibrate the BNO055 first

    - For Calibration refer to [BOSCH BNO055 Calibration Video](https://www.youtube.com/watch?v=Bw0WuAyGsnY&ab_channel=BoschSensortec)

    - Once calibrated, the offsets for BNO055 will be stored on the node


2. Changes

    Use the `UMG_CONF.h` to change:
    - IP Address of the Node
    - InfluxDB Tags and Measurement Names e.g. Node Name and Node Placement


3. Upload the `UMG_Node_main.ino` script to UMG  Node and observe if the Calibration Offsets are reacquired from the Node and stored BNO055. 
