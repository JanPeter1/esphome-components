##
# @brief Simple motion detection sensor that uses the WiFi signal strength
#        signal (RSSI) to detect motions.
#
# @author Jan Peter Riegel <JanPeter1@familie-riegel.de>
# Copyright (c) 2022
##

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import binary_sensor
from esphome.const import (
    CONF_TIMING,
    CONF_HYSTERESIS,
    CONF_BUFFER_SIZE,
)

CODEOWNERS = ["@JanPeter1"]
wifi_csi_ns = cg.esphome_ns.namespace("wifi_csi")


CsiSensor = wifi_csi_ns.class_(
    "CsiSensor", binary_sensor.BinarySensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(CsiSensor)
    .extend(cv.COMPONENT_SCHEMA)
    .extend({
        cv.Optional(CONF_TIMING): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_HYSTERESIS): cv.float_,
        cv.Optional(CONF_BUFFER_SIZE): cv.int_,
    })
)

async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    if CONF_TIMING in config:
        cg.add(var.set_timing(config[CONF_TIMING]))
    if CONF_HYSTERESIS in config:
        cg.add(var.set_sensitivity(config[CONF_HYSTERESIS]))
    if CONF_BUFFER_SIZE in config:
        cg.add(var.set_buffer_size(config[CONF_BUFFER_SIZE]))
    await cg.register_component(var, config)
