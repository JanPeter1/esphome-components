import logging

from esphome import core
from esphome.components import display, font
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_ID,
    CONF_DATA,
    CONF_RAW_DATA_ID,
)
from esphome.core import CORE, HexInt

_LOGGER = logging.getLogger(__name__)

DEPENDENCIES = ["display"]
MULTI_CONF = True

ImageType = display.display_ns.enum("ImageType")
Image_ = display.display_ns.class_("Image")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(Image_),
        cv.Required(CONF_DATA): cv.ensure_list(cv.hex_int),
        cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
    }
)

async def to_code(config):
    img = config[CONF_DATA]
    width = 9
    height = 8
    width8 = ((width + 7) // 8) * 8
    data = [0 for _ in range(height * width8 // 8)]
    for y in range(height):
        for x in range(width):
            bit = img[width - x - 1] & (1 << y)
            if bit == 0:
                continue
            pos = x + y * width8
            data[pos // 8] |= 0x80 >> (pos % 8)
    rhs = [HexInt(x) for x in data]
    prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)
    cg.new_Pvariable(
        config[CONF_ID], prog_arr, width, height, ImageType.IMAGE_TYPE_BINARY
    )
