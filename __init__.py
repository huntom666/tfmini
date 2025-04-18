# my_components/tfmini/__init__.py

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

# Declare dependencies for this component (e.g., UART)
DEPENDENCIES = ['uart']

# Define a namespace for the C++ code
tfmini_ns = cg.esphome_ns.namespace('tfmini')

# You might define the C++ class here if needed elsewhere,
# but often it's done directly in sensor.py
# TFminiSensor = tfmini_ns.class_('TFminiSensor', cg.PollingComponent, uart.UARTDevice)

# Basic config schema, can be expanded if component-level config is needed
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(cg.Component) # Basic component ID
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    # This function runs if you have component-level setup in YAML
    # (e.g., under an 'tfmini:' key, not just 'sensor:')
    # Often empty for simple sensor platforms.
    cg.add_global(tfmini_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    # Add component setup code here if needed
