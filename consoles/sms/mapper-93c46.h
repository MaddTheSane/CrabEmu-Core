/*
    This file is part of CrabEmu.

    Copyright (C) 2009, 2012 Lawrence Sebald

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

#ifndef MAPPER_93C46_H
#define MAPPER_93C46_H

#include "CrabEmu.h"

CLINKAGE

#include <stdio.h>

extern uint8 sms_mem_93c46_mread(uint16 addr);
extern void sms_mem_93c46_mwrite(uint16 addr, uint8 data);

extern uint16 sms_mem_93c46_mread16(uint16 addr);
extern void sms_mem_93c46_mwrite16(uint16 addr, uint16 data);

extern int sms_mem_93c46_write_context(FILE *fp);
extern int sms_mem_93c46_read_context(const uint8 *buf);
extern int sms_mem_93c46_read_mem(const uint8 *buf);

ENDCLINK

#endif /* !MAPPER_93C46_H */
