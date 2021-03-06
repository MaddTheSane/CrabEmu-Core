/*
    This file is part of CrabEmu.

    Copyright (C) 2005, 2006, 2007, 2008, 2009, 2012 Lawrence Sebald

    CrabEmu is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 
    as published by the Free Software Foundation.

    CrabEmu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CrabEmu; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>

#include "mapper-sg1000.h"
#include "smsz80.h"

extern uint8 sms_paging_regs[4];
extern uint8 *sms_read_map[256];
extern uint8 *sms_write_map[256];
extern uint8 *sms_cart_rom;
extern uint32 sms_cart_len;
extern uint8 *sms_rom_page0;
extern uint8 *sms_rom_page1;
extern uint8 *sms_rom_page2;
extern uint8 sms_dummy_arear[256];
extern uint8 sms_dummy_areaw[256];
extern uint8 sms_cart_ram[0x8000];

typedef void (*remap_page_func)();
extern remap_page_func sms_mem_remap_page[4];

void sms_mem_remap_page2_castle(void) {
    int i;

    sms_rom_page2 = sms_cart_ram;

    for(i = 0x80; i < 0xA0; ++i) {
        sms_write_map[i] = sms_rom_page2 + ((i & 0x3F) << 8);
        sms_read_map[i] = sms_rom_page2 + ((i & 0x3F) << 8);
    }

    for(; i < 0xC0; ++i) {
        sms_read_map[i] = sms_dummy_arear;
    }

    sms_z80_set_readmap(sms_read_map);
}

void sms_mem_remap_page0_twmsxa(void) {
    int i;

    sms_rom_page0 = sms_cart_rom;

    for(i = 0x00; i < 0x20; ++i) {
        sms_read_map[i] = sms_cart_rom + (i << 8);
        sms_write_map[i] = sms_dummy_areaw;
    }

    for(i = 0x20; i < 0x40; ++i) {
        sms_write_map[i] = sms_cart_ram + ((i & 0x1F) << 8);
        sms_read_map[i] = sms_cart_ram + ((i & 0x1F) << 8);
    }

    sms_z80_set_readmap(sms_read_map);
}

uint8 sms_mem_sg_mread(uint16 addr) {
    return sms_read_map[addr >> 8][addr & 0xFF];
}

void sms_mem_sg_mwrite(uint16 addr, uint8 data) {
    sms_write_map[addr >> 8][addr & 0xFF] = data;
}

uint16 sms_mem_sg_mread16(uint16 addr) {
    int top = addr >> 8, bot = addr & 0xFF;
    uint16 data = sms_read_map[top][bot++];

    if(bot <= 0xFF)
        data |= (sms_read_map[top][bot] << 8);
    else
        data |= (sms_read_map[(uint8)(top + 1)][0] << 8);

    return data;
}

void sms_mem_sg_mwrite16(uint16 addr, uint16 data) {
    int top = addr >> 8, bot = addr & 0xFF;

    sms_write_map[top][bot++] = (uint8)data;

    if(bot <= 0xFF)
        sms_write_map[top][bot] = (uint8)(data >> 8);
    else
        sms_write_map[(uint8)(top + 1)][0] = (uint8)(data >> 8);
}

int sms_mem_8kb_write_context(FILE *fp) {
    uint8 data[4];

    /* Write the Mapper RAM block */
    data[0] = 'M';
    data[1] = 'P';
    data[2] = 'R';
    data[3] = 'M';
    fwrite(data, 1, 4, fp);             /* Block ID */

    UINT32_TO_BUF(8208, data);
    fwrite(data, 1, 4, fp);             /* Length */

    UINT16_TO_BUF(1, data);
    fwrite(data, 1, 2, fp);             /* Version */
    fwrite(data, 1, 2, fp);             /* Flags (Importance = 1) */

    data[0] = data[1] = data[2] = data[3] = 0;
    fwrite(data, 1, 4, fp);             /* Child pointer */

    fwrite(sms_cart_ram, 1, 8192, fp);

    return 0;
}

int sms_mem_8kb_read_mem(const uint8 *buf) {
    uint32 len;
    uint16 ver;

    /* Check the size */
    BUF_TO_UINT32(buf + 4, len);
    if(len != 8208)
        return -1;

    /* Check the version number */
    BUF_TO_UINT16(buf + 8, ver);
    if(ver != 1)
        return -1;

    /* Check the child pointer */
    if(buf[12] != 0 || buf[13] != 0 || buf[14] != 0 || buf[15] != 0)
        return -1;

    memcpy(sms_cart_ram, buf + 16, 8192);
    return 0;
}
