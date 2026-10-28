/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-, Freakler
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "main.h"
#include "cheats.h"
#include "mctypes.h"

#ifdef CONFIG

extern func_stat save_config();
extern func_stat load_config(const CDRMenuItem *menu_list, usize menu_max);
extern func_stat create_config(const CDRMenuItem *menu_list, usize menu_max);

extern func_stat load_config_block(const u8string magic, uaddr address);

extern void writeBool(SceUID file, int16 id, bool boolean);
extern void writeValue(SceUID file, int16 id, int value);
extern func_stat writeCategoryHeader(SceUID file, u8string magic);

extern int setValueInConfigFor(short id UNUSED, int value UNUSED);

#endif /* CONFIG */

#endif