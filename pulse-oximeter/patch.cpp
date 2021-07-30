// Tell XOD where it can download the libraries:
#pragma XOD require "https://github.com/sparkfun/SparkFun_Bio_Sensor_Hub_Library"

// Include C++ libraries
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

node {
    meta {
        using Type = SparkFun_Bio_Sensor_Hub*;
    }

    static_assert(isValidDigitalPort(constant_input_Reset), "must be a valid digital port");
    static_assert(isValidDigitalPort(constant_input_MFIO), "must be a valid digital port");

    SparkFun_Bio_Sensor_Hub bioHub = SparkFun_Bio_Sensor_Hub(constant_input_Reset, constant_input_MFIO);

    void evaluate(Context ctx) {
        
        if (isSettingUp()) {
            auto wire = getValue<input_I2C>(ctx);
            wire->begin();
            if (bioHub.begin(*wire)!=0){
                raiseError(ctx);
                return;
            }
            if (bioHub.configBpm(MODE_TWO)!=0){
                raiseError(ctx);
                return;
            }
            // allow time for buffer to be loaded with data
            delay(4000);
        }

        if (isInputDirty<input_UPD>(ctx)) {
            bioData body;
            body = bioHub.readBpm();
            emitValue<output_BPM>(ctx, body.heartRate);
            emitValue<output_SpO2>(ctx, body.oxygen);
            emitValue<output_Conf>(ctx, body.confidence);
            emitValue<output_Status>(ctx, body.status);
            emitValue<output_exStat>(ctx, body.extStatus);
            emitValue<output_Done>(ctx, 1);
        }
    }
}
