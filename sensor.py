# my_components/tfmini/sensor.py

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_ACCURACY_DECIMALS,
    CONF_DEVICE_CLASS,
    CONF_STATE_CLASS,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER, # Default unit is meters
)

# Import the namespace defined in __init__.py
from . import tfmini_ns

DEPENDENCIES = ['uart'] # Ensure UART is loaded

# Define the C++ class we'll be creating instances of
# It inherits from Sensor, PollingComponent, and UARTDevice
TFminiSensor = tfmini_ns.class_('TFminiSensor', sensor.Sensor, cg.PollingComponent, uart.UARTDevice)

# Configuration schema for the sensor platform
CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_METER, # Default to meters
    accuracy_decimals=2,            # Default to 2 decimal places
    device_class=DEVICE_CLASS_DISTANCE,
    state_class=STATE_CLASS_MEASUREMENT,
).extend({
    cv.GenerateID(): cv.declare_id(TFminiSensor), # Declare the ID for the C++ class
    cv.Required(CONF_UPDATE_INTERVAL): cv.positive_time_period_milliseconds, # Require update interval
}).extend(uart.UART_DEVICE_SCHEMA) # Include standard UART device options (like uart_id)

async def to_code(config):
    """Generate C++ code for the sensor."""
    # Get the UART parent device
    uart_device = await cg.get_variable(config['uart_id'])

    # Create a new instance of our C++ TFminiSensor class
    var = cg.new_Pvariable(config[CONF_ID]) # Pass the generated ID

    # Register the sensor component
    await sensor.register_sensor(var, config)
    # Register the polling component (handles update_interval)
    await cg.register_component(var, config)
    # Register the UART device, linking it to the parent UART bus
    await uart.register_uart_device(var, config) # Pass the full config for UART settings

    # Set the UART parent (essential for communication)
    cg.add(var.set_uart_parent(uart_device))

    # Example: Set other properties if defined in C++ class
    # if CONF_SOME_OTHER_OPTION in config:
    #    cg.add(var.set_some_other_option(config[CONF_SOME_OTHER_OPTION]))

