#include "shared-bindings/board/__init__.h"

#include "board_busses.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_03) },
  { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_04) },
  { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_P0_28) },
  { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_P0_29) },
  { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_P0_30) },
  { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_P0_31) },

  // 0.71 * VBAT
  { MP_ROM_QSTR(MP_QSTR_VBAT_DET), MP_ROM_PTR(&pin_P0_05) },
  // Digital input: detect usb voltage presence, 0.64 * VUSB
  { MP_ROM_QSTR(MP_QSTR_VUSB_DET), MP_ROM_PTR(&pin_P0_12) },

  { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_P1_01) },
  { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_P1_02) },
  { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_P1_08) },
  { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_P1_10) },
  { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_P1_11) },
  // D7 connected to user LED
  { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_P1_12) },
  { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_P1_03) },

  { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_27) },
  { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_26) },

  { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P1_15) },
  { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P1_13) },
  { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P1_14) },

  { MP_ROM_QSTR(MP_QSTR_TXD), MP_ROM_PTR(&pin_P0_06) },
  { MP_ROM_QSTR(MP_QSTR_RXD), MP_ROM_PTR(&pin_P0_08) },

  { MP_ROM_QSTR(MP_QSTR_MODE), MP_ROM_PTR(&pin_P0_11) },
  { MP_ROM_QSTR(MP_QSTR_LED_R), MP_ROM_PTR(&pin_P0_13) },
  { MP_ROM_QSTR(MP_QSTR_LED_G), MP_ROM_PTR(&pin_P0_14) },
  { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_PTR(&pin_P0_15) },

  // Lipo Charger
  { MP_ROM_QSTR(MP_QSTR__NCHG), MP_ROM_PTR(&pin_P1_09) },
  // BT Antenna Select: VCTL1, VCTL2 on SKY13351-378LF
  // ANT1=0, ANT2=1 -> PCB Antenna
  // ANT1=1, ANT2=0 -> External u.FL
  { MP_ROM_QSTR(MP_QSTR__ANT1), MP_ROM_PTR(&pin_P0_25) },
  { MP_ROM_QSTR(MP_QSTR__ANT2), MP_ROM_PTR(&pin_P0_02) },

  // esp32
  { MP_ROM_QSTR(MP_QSTR__BOOT_W), MP_ROM_PTR(&pin_P0_16) },
  { MP_ROM_QSTR(MP_QSTR__HOST_WAKE), MP_ROM_PTR(&pin_P0_07) },
  { MP_ROM_QSTR(MP_QSTR__B_TX), MP_ROM_PTR(&pin_P1_05) },
  { MP_ROM_QSTR(MP_QSTR__B_RX), MP_ROM_PTR(&pin_P1_04) },
  { MP_ROM_QSTR(MP_QSTR__BT_CTS), MP_ROM_PTR(&pin_P1_07) },
  { MP_ROM_QSTR(MP_QSTR__BT_RTS), MP_ROM_PTR(&pin_P1_06) },
  { MP_ROM_QSTR(MP_QSTR__WIFI_EN), MP_ROM_PTR(&pin_P0_24) },

};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
