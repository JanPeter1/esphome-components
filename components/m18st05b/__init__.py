import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins, automation
from esphome.components import display, uart, switch
from esphome.core import coroutine
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_UART_ID,
)

CODEOWNERS = ["@JanPeter1"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["display", "switch"]

m18st05b_ns = cg.esphome_ns.namespace("m18st05b")
M18ST05B = m18st05b_ns.class_(
    "M18ST05B", cg.PollingComponent, display.DisplayBuffer, uart.UARTDevice, switch.Switch
)
M18ST05BRef = M18ST05B.operator("ref")

M18ST05B_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(M18ST05B),
    }
).extend(cv.polling_component_schema("1s")).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA).extend(switch.SWITCH_SCHEMA)

CONFIG_SCHEMA = cv.All(
    M18ST05B_SCHEMA,
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)

async def setup_m18st05b(var, config):
    await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await setup_m18st05b(var, config)
    await uart.register_uart_device(var, config)
    await switch.register_switch(var, config)
