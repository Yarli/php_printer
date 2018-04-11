/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank M. Kromann    <frank@kromann.info>                    |
   |          Daniel Beulshausen  <daniel@php4win.de>                     |
   | Contribution:                                                        |
   |		  Philippe MAES       <luckyluke@dlfp.org>                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_PRINTER_H
#define PHP_PRINTER_H

#if HAVE_PRINTER
#include <Winspool.h>

extern zend_module_entry printer_module_entry;
#define printer_module_ptr &printer_module_entry

#define PHP_PRINTER_VERSION "0.1.0-dev"

PHP_MINIT_FUNCTION(printer);
PHP_MINFO_FUNCTION(printer);
PHP_MSHUTDOWN_FUNCTION(printer);

PHP_FUNCTION(printer_open);
PHP_FUNCTION(printer_close);
PHP_FUNCTION(printer_write);
PHP_FUNCTION(printer_list);
PHP_FUNCTION(printer_set_option);
PHP_FUNCTION(printer_get_option);
PHP_FUNCTION(printer_create_dc);
PHP_FUNCTION(printer_delete_dc);
PHP_FUNCTION(printer_start_doc);
PHP_FUNCTION(printer_end_doc);
PHP_FUNCTION(printer_start_page);
PHP_FUNCTION(printer_end_page);
PHP_FUNCTION(printer_create_pen);
PHP_FUNCTION(printer_delete_pen);
PHP_FUNCTION(printer_select_pen);
PHP_FUNCTION(printer_create_brush);
PHP_FUNCTION(printer_delete_brush);
PHP_FUNCTION(printer_select_brush);
PHP_FUNCTION(printer_create_font);
PHP_FUNCTION(printer_delete_font);
PHP_FUNCTION(printer_select_font);
PHP_FUNCTION(printer_logical_fontheight);
PHP_FUNCTION(printer_draw_roundrect);
PHP_FUNCTION(printer_draw_rectangle);
PHP_FUNCTION(printer_draw_text);
PHP_FUNCTION(printer_draw_elipse);
PHP_FUNCTION(printer_draw_line);
PHP_FUNCTION(printer_draw_chord);
PHP_FUNCTION(printer_draw_pie);
PHP_FUNCTION(printer_draw_bmp);
PHP_FUNCTION(printer_abort);

typedef struct {
	HANDLE handle;
	LPTSTR name;
	DOCINFO info;
	HDC dc;
	PRINTER_INFO_2 *pi2;
	DWORD dmModifiedFields;
} printer;

ZEND_BEGIN_MODULE_GLOBALS(printer)
	char *default_printer;
ZEND_END_MODULE_GLOBALS(printer)

#ifdef ZTS
#define PRINTERG(v) TSRMG(printer_globals_id, zend_printer_globals *, v)
#else
#define PRINTERG(v) (printer_globals.v)
#endif

#else

#define printer_module_ptr NULL

#endif

#define phpext_printer_ptr printer_module_ptr

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */