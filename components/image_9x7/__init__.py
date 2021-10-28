import logging

from esphome import core
from esphome.components import m18st05b, display, font
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_ID,
    CONF_DATA,
    CONF_RAW_DATA_ID,
)
from esphome.core import CORE, HexInt

_LOGGER = logging.getLogger(__name__)

DEPENDENCIES = ["m18st05b"]
MULTI_CONF = True

ImageType = display.display_ns.enum("ImageType")
Image_ = m18st05b.m18st05b_ns.class_("Image9x7")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(Image_),
        cv.Required(CONF_DATA): cv.ensure_list(cv.hex_int),
#        cv.Required(CONF_SIZE): cv.dimensions,
        cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
    }
)

async def to_code(config):
    data = config[CONF_DATA]
    rhs = [HexInt(x) for x in data]
    prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)
    cg.new_Pvariable(
        config[CONF_ID], prog_arr, 9, 8, ImageType.IMAGE_TYPE_BINARY
    )
