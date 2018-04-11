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
   |		  Adam French         <support@quebyte.com>                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/php_math.h"

#ifdef HAVE_PRINTER

static int le_printer, le_brush, le_pen, le_font;

#ifdef PHP_WIN64
#include <windows.h>
#include <wingdi.h>
#include <winspool.h>

#include "php_printer.h"

COLORREF hex_to_rgb(char * hex);
char *rgb_to_hex(COLORREF rgb);
static void printer_close(zend_rsrc_list_entry *resource TSRMLS_DC);
static void object_close(zend_rsrc_list_entry *resource TSRMLS_DC);
char *get_default_printer(void);

zend_function_entry printer_functions[] = {
	PHP_FE(printer_open,				NULL)
	PHP_FE(printer_close,				NULL)
	PHP_FE(printer_write,				NULL)
	PHP_FE(printer_list,				NULL)
	PHP_FE(printer_set_option,			NULL)
	PHP_FE(printer_get_option,			NULL)
	PHP_FE(printer_create_dc,			NULL)
	PHP_FE(printer_delete_dc,			NULL)
	PHP_FE(printer_start_doc,			NULL)
	PHP_FE(printer_end_doc,				NULL)
	PHP_FE(printer_start_page,			NULL)
	PHP_FE(printer_end_page,			NULL)
	PHP_FE(printer_create_pen,			NULL)
	PHP_FE(printer_delete_pen,			NULL)
	PHP_FE(printer_select_pen,			NULL)
	PHP_FE(printer_create_brush,		NULL)
	PHP_FE(printer_delete_brush,		NULL)
	PHP_FE(printer_select_brush,		NULL)
	PHP_FE(printer_create_font,			NULL)
	PHP_FE(printer_delete_font,			NULL)
	PHP_FE(printer_select_font,			NULL)
	PHP_FE(printer_logical_fontheight,	NULL)
	PHP_FE(printer_draw_roundrect,		NULL)
	PHP_FE(printer_draw_rectangle,		NULL)
	PHP_FE(printer_draw_text,			NULL)
	PHP_FE(printer_draw_elipse,			NULL)
	PHP_FE(printer_draw_line,			NULL)
	PHP_FE(printer_draw_chord,			NULL)
	PHP_FE(printer_draw_pie,			NULL)
	PHP_FE(printer_draw_bmp,			NULL)
	PHP_FE(printer_abort,				NULL)
	{NULL, NULL, NULL}
};

zend_module_entry printer_module_entry = {
	STANDARD_MODULE_HEADER,
	"printer",
	printer_functions,
	PHP_MINIT(printer),
	PHP_MSHUTDOWN(printer),
	NULL,
	NULL,
	PHP_MINFO(printer),
	PHP_PRINTER_VERSION,
	STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(printer)

#ifdef COMPILE_DL_PRINTER
ZEND_GET_MODULE(printer)
#endif


PHP_MINFO_FUNCTION(printer)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Printer Support", "enabled");
	php_info_print_table_row(2, "Version", PHP_PRINTER_VERSION);
	php_info_print_table_row(2, "Default printing device", PRINTERG(default_printer) ? PRINTERG(default_printer) : "<b>not detected</b>");
	php_info_print_table_row(2, "Module state", "working");
	php_info_print_table_row(2, "RCS Version", "$Id$");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

static PHP_INI_MH(OnUpdatePrinter)
{
	if (new_value != NULL && new_value_length > 0) {
		if (PRINTERG(default_printer)) {
			pefree(PRINTERG(default_printer), 1);
		}
		PRINTERG(default_printer) = pestrdup(new_value, 1);
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY("printer.default_printer", "", PHP_INI_ALL, OnUpdatePrinter)
PHP_INI_END()

#define COPIES			0
#define MODE			1
#define TITLE			2
#define ORIENTATION		3
#define YRESOLUTION		4
#define XRESOLUTION		5
#define PAPER_FORMAT	6
#define PAPER_LENGTH	7
#define PAPER_WIDTH		8
#define SCALE			9
#define BG_COLOR		10
#define TEXT_COLOR		11
#define TEXT_ALIGN		12
#define DEVICENAME		13
#define DRIVER_VERSION	14
#define OUTPUT_FILE		15
#define VALID_OPTIONS	16
#define BRUSH_SOLID		-1
#define BRUSH_CUSTOM	-2

#define REGP_CONSTANT(a,b)	REGISTER_LONG_CONSTANT(a, b, CONST_CS | CONST_PERSISTENT);

static void php_printer_init(zend_printer_globals *printer_globals TSRMLS_DC) {
	printer_globals->default_printer = get_default_printer();
}

static void php_printer_shutdown(zend_printer_globals *printer_globals TSRMLS_DC) {
	if (printer_globals->default_printer) {
		pefree(printer_globals->default_printer, 1);
	}
}

PHP_MINIT_FUNCTION(printer)
{
    ZEND_INIT_MODULE_GLOBALS(printer, php_printer_init, php_printer_shutdown);
	REGISTER_INI_ENTRIES();

	le_printer	= zend_register_list_destructors_ex(printer_close, NULL, "printer", module_number);
	le_pen		= zend_register_list_destructors_ex(object_close, NULL, "printer pen", module_number);
	le_font		= zend_register_list_destructors_ex(object_close, NULL, "printer font", module_number);
	le_brush	= zend_register_list_destructors_ex(object_close, NULL, "printer brush", module_number);

	REGP_CONSTANT("PRINTER_COPIES",				COPIES);
	REGP_CONSTANT("PRINTER_MODE",				MODE);
	REGP_CONSTANT("PRINTER_TITLE",				TITLE);
	REGP_CONSTANT("PRINTER_DEVICENAME",			DEVICENAME);
	REGP_CONSTANT("PRINTER_DRIVERVERSION",		DRIVER_VERSION);
	REGP_CONSTANT("PRINTER_OUTPUT_FILE",		OUTPUT_FILE);
	REGP_CONSTANT("PRINTER_RESOLUTION_Y",		YRESOLUTION);
	REGP_CONSTANT("PRINTER_RESOLUTION_X",		XRESOLUTION);
	REGP_CONSTANT("PRINTER_SCALE",				SCALE);
	REGP_CONSTANT("PRINTER_BACKGROUND_COLOR",	BG_COLOR);
	REGP_CONSTANT("PRINTER_PAPER_LENGTH",		PAPER_LENGTH);
	REGP_CONSTANT("PRINTER_PAPER_WIDTH",		PAPER_WIDTH);

	REGP_CONSTANT("PRINTER_PAPER_FORMAT",		PAPER_FORMAT);
	REGP_CONSTANT("PRINTER_FORMAT_CUSTOM",		0);
	REGP_CONSTANT("PRINTER_FORMAT_LETTER",		DMPAPER_LETTER);
	REGP_CONSTANT("PRINTER_FORMAT_LEGAL",		DMPAPER_LEGAL);
	REGP_CONSTANT("PRINTER_FORMAT_A3",			DMPAPER_A3);
	REGP_CONSTANT("PRINTER_FORMAT_A4",			DMPAPER_A4);
	REGP_CONSTANT("PRINTER_FORMAT_A5",			DMPAPER_A5);
	REGP_CONSTANT("PRINTER_FORMAT_B4",			DMPAPER_B4);
	REGP_CONSTANT("PRINTER_FORMAT_B5",			DMPAPER_B5);
	REGP_CONSTANT("PRINTER_FORMAT_FOLIO",		DMPAPER_FOLIO);

	REGP_CONSTANT("PRINTER_ORIENTATION",			ORIENTATION);
	REGP_CONSTANT("PRINTER_ORIENTATION_PORTRAIT",	DMORIENT_PORTRAIT);
	REGP_CONSTANT("PRINTER_ORIENTATION_LANDSCAPE",	DMORIENT_LANDSCAPE);

	REGP_CONSTANT("PRINTER_TEXT_COLOR",			TEXT_COLOR);
	REGP_CONSTANT("PRINTER_TEXT_ALIGN",			TEXT_ALIGN);
	REGP_CONSTANT("PRINTER_TA_BASELINE",		TA_BASELINE);
	REGP_CONSTANT("PRINTER_TA_BOTTOM",			TA_BOTTOM);
	REGP_CONSTANT("PRINTER_TA_TOP",				TA_TOP);
	REGP_CONSTANT("PRINTER_TA_CENTER",			TA_CENTER);
	REGP_CONSTANT("PRINTER_TA_LEFT",			TA_LEFT);
	REGP_CONSTANT("PRINTER_TA_RIGHT",			TA_RIGHT);
	
	REGP_CONSTANT("PRINTER_PEN_SOLID",			PS_SOLID);
	REGP_CONSTANT("PRINTER_PEN_DASH",			PS_DASH);
	REGP_CONSTANT("PRINTER_PEN_DOT",			PS_DOT);
	REGP_CONSTANT("PRINTER_PEN_DASHDOT",		PS_DASHDOT);
	REGP_CONSTANT("PRINTER_PEN_DASHDOTDOT",		PS_DASHDOTDOT);
	REGP_CONSTANT("PRINTER_PEN_INVISIBLE",		PS_NULL);

	REGP_CONSTANT("PRINTER_BRUSH_SOLID",		BRUSH_SOLID);
	REGP_CONSTANT("PRINTER_BRUSH_CUSTOM",		BRUSH_CUSTOM);
	REGP_CONSTANT("PRINTER_BRUSH_DIAGONAL",		HS_BDIAGONAL);
	REGP_CONSTANT("PRINTER_BRUSH_CROSS",		HS_CROSS);
	REGP_CONSTANT("PRINTER_BRUSH_DIAGCROSS",	HS_DIAGCROSS);
	REGP_CONSTANT("PRINTER_BRUSH_FDIAGONAL",	HS_FDIAGONAL);
	REGP_CONSTANT("PRINTER_BRUSH_HORIZONTAL",	HS_HORIZONTAL);
	REGP_CONSTANT("PRINTER_BRUSH_VERTICAL",		HS_VERTICAL);

	REGP_CONSTANT("PRINTER_FW_THIN",			FW_THIN);
	REGP_CONSTANT("PRINTER_FW_ULTRALIGHT",		FW_ULTRALIGHT);
	REGP_CONSTANT("PRINTER_FW_LIGHT",			FW_LIGHT);
	REGP_CONSTANT("PRINTER_FW_NORMAL",			FW_NORMAL);
	REGP_CONSTANT("PRINTER_FW_MEDIUM",			FW_MEDIUM);
	REGP_CONSTANT("PRINTER_FW_BOLD",			FW_BOLD);
	REGP_CONSTANT("PRINTER_FW_ULTRABOLD",		FW_ULTRABOLD);
	REGP_CONSTANT("PRINTER_FW_HEAVY",			FW_HEAVY);

	REGP_CONSTANT("PRINTER_ENUM_LOCAL",			PRINTER_ENUM_LOCAL);
	REGP_CONSTANT("PRINTER_ENUM_NAME",			PRINTER_ENUM_NAME);
	REGP_CONSTANT("PRINTER_ENUM_SHARED",		PRINTER_ENUM_SHARED);
	REGP_CONSTANT("PRINTER_ENUM_DEFAULT",		PRINTER_ENUM_DEFAULT);
	REGP_CONSTANT("PRINTER_ENUM_CONNECTIONS",	PRINTER_ENUM_CONNECTIONS);
	REGP_CONSTANT("PRINTER_ENUM_NETWORK",		PRINTER_ENUM_NETWORK);
	REGP_CONSTANT("PRINTER_ENUM_REMOTE",		PRINTER_ENUM_REMOTE);

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(printer)
{
#ifdef ZTS
	ts_free_id(printer_globals_id);
#else
	php_printer_shutdown(&printer_globals TSRMLS_CC);
#endif
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}



/* {{{ proto mixed printer_open([string printername])
   Return a handle to the printer or false if connection failed */
PHP_FUNCTION(printer_open)
{
	zval **arg1;
	printer *resource;

	int argc = ZEND_NUM_ARGS();

	resource = (printer *)emalloc(sizeof(printer));
	resource->dmModifiedFields = 0;

	if( argc == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE ) {
      	convert_to_string_ex(arg1);
      	resource->name = Z_STRVAL_PP(arg1);
	}
	else if( argc == 0 ) {
		resource->name = PRINTERG(default_printer);
	}
	else {
		WRONG_PARAM_COUNT;
	}

	if (OpenPrinter(resource->name, &resource->handle, NULL) != 0) {
		resource->pi2 = (PRINTER_INFO_2 *)emalloc(sizeof(PRINTER_INFO_2));
		resource->pi2->pDevMode = (DEVMODE *)emalloc(DocumentProperties(NULL, NULL, resource->name, NULL, NULL, 0));
		if (DocumentProperties(NULL, resource->handle, resource->name, resource->pi2->pDevMode, NULL, DM_OUT_BUFFER) == IDOK) {
			resource->info.lpszDocName	= estrdup("PHP generated Document");
			resource->info.lpszOutput	= NULL;
			resource->info.lpszDatatype = estrdup("TEXT");
			resource->info.fwType		= 0;
			resource->info.cbSize		= sizeof(resource->info);
			resource->dc = CreateDC(NULL, resource->name, NULL, resource->pi2->pDevMode);
			ZEND_REGISTER_RESOURCE(return_value, resource, le_printer);
		}
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't connect to the printer [%s]", resource->name);
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto void printer_close(resource connection)
   Close the printer connection */
PHP_FUNCTION(printer_close)
{
	zval **arg1;
	printer *resource;

	if ( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto bool printer_write(resource connection,string content)
   Write directly to the printer */
PHP_FUNCTION(printer_write)
{
	zval **arg1, **arg2;
	printer *resource;
	DOC_INFO_1 docinfo;
	int sd, sp = 0, recieved;

	if ( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}


	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_string_ex(arg2);

	docinfo.pDocName	= (LPTSTR)resource->info.lpszDocName;
	docinfo.pOutputFile	= (LPTSTR)resource->info.lpszOutput;
	docinfo.pDatatype	= (LPTSTR)resource->info.lpszDatatype;

	sd = StartDocPrinter(resource->handle, 1, (LPBYTE)&docinfo);
	sp = StartPagePrinter(resource->handle);

	if( sd && sp ) {
		WritePrinter(resource->handle, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2), &recieved);
		EndPagePrinter(resource->handle);
		EndDocPrinter(resource->handle);
		RETURN_TRUE;
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't allocate the printerjob [%d]", GetLastError());
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto array printer_list(int EnumType [, string Name [, int Level]])
   Return an array of printers attached to the server */
PHP_FUNCTION(printer_list)
{
	zval *Printer, *Printer_ptr, **arg1, **arg2, **arg3;
	char InfoBuffer[8192], *Name;

	PRINTER_INFO_1 *P1;
	PRINTER_INFO_2 *P2;
	PRINTER_INFO_4 *P4;
	PRINTER_INFO_5 *P5;
	DWORD bNeeded = sizeof(InfoBuffer), cReturned, i;
	int argc = ZEND_NUM_ARGS(), Level;
	int LevvelsAllowed[] = {0, 1, 1, 0, 1, 1};

	if( argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	Level = 1;
	Name = NULL;

	switch(argc) {
		case 3:
			convert_to_long_ex(arg3);
			Level = (int)Z_LVAL_PP(arg3);
			if(!LevvelsAllowed[Level]) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Level not allowed");
				RETURN_FALSE;
			}
		case 2:
			convert_to_string_ex(arg2);
			Name = Z_STRVAL_PP(arg2);
		case 1:
			convert_to_long_ex(arg1);
			break;
	}

	
	if(array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}


	EnumPrinters(Z_LVAL_PP(arg1), Name, Level, (LPBYTE)InfoBuffer, sizeof(InfoBuffer), &bNeeded, &cReturned);

	P1 = (PRINTER_INFO_1 *)InfoBuffer;
	P2 = (PRINTER_INFO_2 *)InfoBuffer;
	P4 = (PRINTER_INFO_4 *)InfoBuffer;
	P5 = (PRINTER_INFO_5 *)InfoBuffer;


	for(i = 0; i < cReturned; i++) {
		Printer = (zval *) emalloc(sizeof(zval));
		array_init(Printer);
		
		switch (Level) {
			case 1:
				add_assoc_string(Printer, "NAME", P1->pName, 1);
				add_assoc_string(Printer, "DESCRIPTION", P1->pDescription, 1);
				add_assoc_string(Printer, "COMMENT", P1->pComment, 1);
				P1++;
				break;

			case 2:
				if (P2->pServerName) add_assoc_string(Printer, "SERVERNAME", P2->pServerName, 1);
				if (P2->pPrinterName) add_assoc_string(Printer, "PRINTERNAME", P2->pPrinterName, 1);
				if (P2->pShareName) add_assoc_string(Printer, "SHARENAME", P2->pShareName, 1);
				if (P2->pPortName) add_assoc_string(Printer, "PORTNAME", P2->pPortName, 1);
				if (P2->pDriverName) add_assoc_string(Printer, "DRIVERNAME", P2->pDriverName, 1);
				if (P2->pComment) add_assoc_string(Printer, "COMMENT", P2->pComment, 1);
				if (P2->pLocation) add_assoc_string(Printer, "LOCATION", P2->pLocation, 1);
				if (P2->pSepFile) add_assoc_string(Printer, "SEPFILE", P2->pSepFile, 1);
				if (P2->pPrintProcessor) add_assoc_string(Printer, "PRINTPROCESSOR", P2->pPrintProcessor, 1);
				if (P2->pDatatype) add_assoc_string(Printer, "DATATYPE", P2->pDatatype, 1);
				if (P2->pParameters) add_assoc_string(Printer, "PARAMETRES", P2->pParameters, 1);
				add_assoc_long(Printer, "ATTRIBUTES", P2->Attributes);
				add_assoc_long(Printer, "PRIORITY", P2->Priority);
				add_assoc_long(Printer, "DEFAULTPRIORITY", P2->DefaultPriority);
				add_assoc_long(Printer, "STARTTIME", P2->StartTime);
				add_assoc_long(Printer, "UNTILTIME", P2->UntilTime);
				add_assoc_long(Printer, "STATUS", P2->Status);
				add_assoc_long(Printer, "CJOBS", P2->cJobs);
				add_assoc_long(Printer, "AVERAGEPPM", P2->AveragePPM);
				P2++;
				break;

			case 4:
				add_assoc_string(Printer, "PRINTERNAME", P4->pPrinterName, 1);
				add_assoc_string(Printer, "SERVERNAME", P4->pServerName, 1);
				add_assoc_long(Printer, "ATTRIBUTES", P4->Attributes);
				P4++;
				break;

			case 5:
				add_assoc_string(Printer, "PRINTERNAME", P5->pPrinterName, 1);
				add_assoc_string(Printer, "PORTNAME", P5->pPortName, 1);
				add_assoc_long(Printer, "ATTRIBUTES", P5->Attributes);
				add_assoc_long(Printer, "DEVICENOTSELECTEDTIMEOUT", P5->DeviceNotSelectedTimeout);
				add_assoc_long(Printer, "TRANSMISSIONRETRYTIMEOUT", P5->TransmissionRetryTimeout);
				P5++;
				break;
		}

		zend_hash_index_update(Z_ARRVAL_P(return_value), i, (void *) &Printer, sizeof(zval *), (void **) &Printer_ptr);
	}
}

/* }}} */


/* {{{ proto bool printer_set_option(resource connection,string option,mixed value)
   Configure the printer device */
PHP_FUNCTION(printer_set_option)
{
	DWORD dwNeeded =0;
	PRINTER_DEFAULTS pd;
	zval **arg1, **arg2, **arg3;
	printer *resource;

	if( zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_long_ex(arg2);

	memset(&pd,0,sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	SetLastError(0);

	switch(Z_LVAL_PP(arg2)) {
		case COPIES:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmCopies=(short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields|=DM_COPIES;
			break;

		case MODE:
			convert_to_string_ex(arg3);
			if (resource->info.lpszDatatype) {
				efree((char *)resource->info.lpszDatatype);
			}
			resource->info.lpszDatatype		= estrdup(Z_STRVAL_PP(arg3));
			resource->info.cbSize			= sizeof(resource->info);
			break;

		case TITLE:
			convert_to_string_ex(arg3);
			if (resource->info.lpszDocName) {
				efree((char *)resource->info.lpszDocName);
			}
			resource->info.lpszDocName		= estrdup(Z_STRVAL_PP(arg3));
			resource->info.cbSize			= sizeof(resource->info);
			break;

		case OUTPUT_FILE:
			convert_to_string_ex(arg3);
			if (resource->info.lpszOutput) {
				efree((char *)resource->info.lpszOutput);
			}
			resource->info.lpszOutput		= estrdup(Z_STRVAL_PP(arg3));
			resource->info.cbSize			= sizeof(resource->info);
			break;

		case ORIENTATION:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmOrientation=(short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields|=DM_ORIENTATION;
			break;

		case YRESOLUTION:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmYResolution = (short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields|=DM_YRESOLUTION;
			break;
		
		case XRESOLUTION:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmPrintQuality= (short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields|=DM_PRINTQUALITY;
			break;

		case PAPER_FORMAT:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmPaperSize	= (short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields|=DM_PAPERSIZE;
			break;

		case PAPER_LENGTH:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmPaperLength = (short)(Z_LVAL_PP(arg3) * 10);
			resource->dmModifiedFields|=DM_PAPERLENGTH;
			break;

		case PAPER_WIDTH:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmPaperWidth	= (short)(Z_LVAL_PP(arg3) * 10);
			resource->dmModifiedFields |=  DM_PAPERWIDTH;
			break;

		case SCALE:
			convert_to_long_ex(arg3);
			resource->pi2->pDevMode->dmScale		= (short)Z_LVAL_PP(arg3);
			resource->dmModifiedFields |= DM_SCALE;
			break;

		case BG_COLOR:
			convert_to_string_ex(arg3);
			SetBkColor(resource->dc, hex_to_rgb(Z_STRVAL_PP(arg3)));
			break;

		case TEXT_COLOR:
			convert_to_string_ex(arg3);
			SetTextColor(resource->dc, hex_to_rgb(Z_STRVAL_PP(arg3)));
			break;

		case TEXT_ALIGN:
			convert_to_string_ex(arg3);
			SetTextAlign(resource->dc, Z_LVAL_PP(arg3));
			break;
		case VALID_OPTIONS:
			resource->pi2->pSecurityDescriptor=NULL;
			resource->pi2->pDevMode->dmFields=resource->dmModifiedFields;
			resource->dmModifiedFields=0;
			DocumentProperties(NULL,resource->handle,resource->name,resource->pi2->pDevMode,resource->pi2->pDevMode,DM_IN_BUFFER | DM_OUT_BUFFER);
			SetPrinter(resource->handle,2,(LPBYTE)resource->pi2,0);
			SendMessageTimeout(HWND_BROADCAST,WM_DEVMODECHANGE,0L,(LPARAM)(LPCSTR)resource->name,SMTO_NORMAL,1000,NULL);
		break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unknown option passed to printer_set_option()");
			RETURN_FALSE;
	}


	RETURN_TRUE;
}
/* }}} */


/* {{{ proto mixed printer_get_option(int handle, string option)
   Get configured data */
PHP_FUNCTION(printer_get_option)
{
	zval **arg1, **arg2;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_long_ex(arg2);

	switch(Z_LVAL_PP(arg2)) {
		case COPIES:
			RETURN_LONG(resource->pi2->pDevMode->dmCopies);

		case MODE:
			RETURN_STRING((char*)resource->info.lpszDatatype,1);

		case TITLE:
			RETURN_STRING((char*)resource->info.lpszDocName,1);

		case OUTPUT_FILE:
			if (resource->info.lpszOutput) {
				RETURN_STRING((char*)resource->info.lpszOutput,1);
			}
			else {
				RETURN_NULL();
			}

		case ORIENTATION:
			RETURN_LONG(resource->pi2->pDevMode->dmOrientation);

		case YRESOLUTION:
			RETURN_LONG(resource->pi2->pDevMode->dmYResolution);
		
		case XRESOLUTION:
			RETURN_LONG(resource->pi2->pDevMode->dmPrintQuality);

		case PAPER_FORMAT:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperSize);

		case PAPER_LENGTH:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperLength / 10);

		case PAPER_WIDTH:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperWidth / 10);

		case SCALE:
			RETURN_LONG(resource->pi2->pDevMode->dmScale);

		case BG_COLOR:
			RETURN_STRING(rgb_to_hex(GetBkColor(resource->dc)), 0);

		case TEXT_COLOR:
			RETURN_STRING(rgb_to_hex(GetTextColor(resource->dc)), 0);

		case TEXT_ALIGN:
			RETURN_LONG(GetTextAlign(resource->dc));

		case DEVICENAME:
			RETURN_STRING(resource->name, 1);

		case DRIVER_VERSION:
			RETURN_LONG(resource->pi2->pDevMode->dmDriverVersion);

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unknown option passed to printer_get_option()");
			RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto void printer_create_dc(int handle)
   Create a device content */
PHP_FUNCTION(printer_create_dc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if( resource->dc != NULL ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Deleting old DeviceContext");
		DeleteDC(resource->dc);
	}
	
	resource->dc = CreateDC(NULL, resource->name, NULL, resource->pi2->pDevMode);
}
/* }}} */


/* {{{ proto bool printer_delete_dc(int handle)
   Delete a device content */
PHP_FUNCTION(printer_delete_dc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if( resource->dc != NULL ) {
		DeleteDC(resource->dc);
		resource->dc = NULL;
		RETURN_TRUE;
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No DeviceContext created");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool printer_start_doc(int handle)
   Start a document */
PHP_FUNCTION(printer_start_doc)
{
	zval **parameter[2];
	printer *resource;
	int argc = ZEND_NUM_ARGS();
	
	if (argc > 2 || argc < 1 || zend_get_parameters_array_ex(argc, parameter) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, parameter[0], -1, "Printer Handle", le_printer);
	
	if(argc == 2) {
		convert_to_string_ex(parameter[1]);
		if (resource->info.lpszDocName) {
			efree((char *)resource->info.lpszDocName);
		}
		resource->info.lpszDocName = estrdup(Z_STRVAL_PP(parameter[1]));
		resource->info.cbSize	   = sizeof(resource->info);
	}

	if(StartDoc(resource->dc, &resource->info) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't allocate new print job");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_end_doc(int handle)
   End a document */
PHP_FUNCTION(printer_end_doc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(EndDoc(resource->dc) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't terminate print job");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_start_page(int handle)
   Start a page */
PHP_FUNCTION(printer_start_page)
{
	zval **arg1;
	printer *resource;

	if( ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE ) {
		;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(StartPage(resource->dc) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't start a new page");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_end_page(int handle)
   End a page */
PHP_FUNCTION(printer_end_page)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(EndPage(resource->dc) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "couldn't end the page");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto mixed printer_create_pen(int style, int width, string color)
   Create a pen */
PHP_FUNCTION(printer_create_pen)
{
	zval **arg1, **arg2, **arg3;
	HPEN pen;

	if( zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);

	pen = CreatePen(Z_LVAL_PP(arg1), Z_LVAL_PP(arg2), hex_to_rgb(Z_STRVAL_PP(arg3)));

	if(!pen) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pen, le_pen);
}
/* }}} */


/* {{{ proto void printer_delete_pen(resource pen_handle)
   Delete a pen */
PHP_FUNCTION(printer_delete_pen)
{
	zval **arg1;
	HPEN pen;
	
	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pen, HPEN, arg1, -1, "Pen Handle", le_pen);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_pen(resource printer_handle, resource pen_handle)
   Select a pen */
PHP_FUNCTION(printer_select_pen)
{
	zval **arg1, **arg2;
	HPEN pen;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(pen, HPEN, arg2, -1, "Pen Handle", le_pen);

	SelectObject(resource->dc, pen);
}
/* }}} */


/* {{{ proto mixed printer_create_brush(resource handle)
   Create a brush */
PHP_FUNCTION(printer_create_brush)
{
	zval **arg1, **arg2;
	HBRUSH brush;
	HBITMAP bmp;
	char* path;

	if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);

	switch(Z_LVAL_PP(arg1)) {
		case BRUSH_SOLID:
			brush = CreateSolidBrush(hex_to_rgb(Z_STRVAL_PP(arg2)));
			break;
		case BRUSH_CUSTOM:
			virtual_filepath(Z_STRVAL_PP(arg2), &path TSRMLS_CC);
			bmp = LoadImage(0, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			brush = CreatePatternBrush(bmp);
			break;
		default:
			brush = CreateHatchBrush(Z_LVAL_PP(arg1), hex_to_rgb(Z_STRVAL_PP(arg2)));
	}

	if(!brush) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, brush, le_brush);
}
/* }}} */


/* {{{ proto void printer_delete_brush(resource brush_handle)
   Delete a brush */
PHP_FUNCTION(printer_delete_brush)
{
	zval **arg1;
	HBRUSH brush;
	
	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(brush, HBRUSH, arg1, -1, "Brush Handle", le_brush);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_brush(resource printer_handle, resource brush_handle)
   Select a brush */
PHP_FUNCTION(printer_select_brush)
{
	zval **arg1, **arg2;
	HBRUSH brush;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(brush, HBRUSH, arg2, -1, "Brush Handle", le_brush);

	SelectObject(resource->dc, brush);
}
/* }}} */


/* {{{ proto mixed printer_create_font(string face, int height, int width, int font_weight, bool italic, bool underline, bool strikeout, int orientaton)
   Create a font */
PHP_FUNCTION(printer_create_font)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	HFONT font;
	char *face;

	if( zend_get_parameters_ex(8, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	face = estrndup(Z_STRVAL_PP(arg1), 32);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_boolean_ex(arg5);
	convert_to_boolean_ex(arg6);
	convert_to_boolean_ex(arg7);
	convert_to_long_ex(arg8);

	font = CreateFont(Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg8), Z_LVAL_PP(arg8), Z_LVAL_PP(arg4), Z_BVAL_PP(arg5), Z_BVAL_PP(arg6), Z_BVAL_PP(arg7), DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_ROMAN, face);
	efree(face);

	if(!font) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, font, le_font);
}
/* }}} */


/* {{{ proto void printer_delete_font(int fonthandle)
   Delete a font */
PHP_FUNCTION(printer_delete_font)
{
	zval **arg1; 
	HFONT font;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(font, HFONT, arg1, -1, "Font Handle", le_font);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_font(int printerhandle, int fonthandle)
   Select a font */
PHP_FUNCTION(printer_select_font)
{
	zval **arg1, **arg2;
	HFONT font;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(font, HFONT, arg2, -1, "Font Handle", le_font);

	SelectObject(resource->dc, font);
}
/* }}} */


/* {{{ proto int printer_logical_fontheight(int handle, int height)
   Get the logical font height */
PHP_FUNCTION(printer_logical_fontheight)
{
	zval **arg1, **arg2;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);

	RETURN_LONG(MulDiv(Z_LVAL_PP(arg2), GetDeviceCaps(resource->dc, LOGPIXELSY), 72));
}
/* }}} */


/* {{{ proto void printer_draw_roundrect(resource handle, int ul_x, int ul_y, int lr_x, int lr_y, int width, int height)
   Draw a roundrect */	
PHP_FUNCTION(printer_draw_roundrect)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7; 
	printer *resource;
	
	if( zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);

	RoundRect(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7));
}
/* }}} */


/* {{{ proto void printer_draw_rectangle(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw a rectangle */
PHP_FUNCTION(printer_draw_rectangle)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5; 
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	Rectangle(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_elipse(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw an elipse */
PHP_FUNCTION(printer_draw_elipse)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5; 
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	Ellipse(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_text(resource handle, string text, int x, int y)
   Draw text */
PHP_FUNCTION(printer_draw_text)
{
	zval **arg1, **arg2, **arg3, **arg4; 
	printer *resource;

	if( zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);

	ExtTextOut(resource->dc, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), ETO_OPAQUE, NULL, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2), NULL);
}
/* }}} */


/* {{{ proto void printer_draw_line(int handle, int fx, int fy, int tx, int ty)
   Draw line from x, y to x, y*/
PHP_FUNCTION(printer_draw_line)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	MoveToEx(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), NULL);
	LineTo(resource->dc, Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_chord(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad_x, int rad_y, int rad_x1, int rad_y1)
   Draw a chord*/
PHP_FUNCTION(printer_draw_chord)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	printer *resource;

	if( zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);

	Chord(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7), Z_LVAL_PP(arg8), Z_LVAL_PP(arg9));
}
/* }}} */


/* {{{ proto void printer_draw_pie(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad1_x, int rad1_y, int rad2_x, int rad2_y)
   Draw a pie*/
PHP_FUNCTION(printer_draw_pie)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	printer *resource;

	if( zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);

	Pie(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7), Z_LVAL_PP(arg8), Z_LVAL_PP(arg9));
}
/* }}} */


/* {{{ proto mixed printer_draw_bmp(resource handle, string filename, int x, int y [, int width, int height])
   Draw a bitmap */
PHP_FUNCTION(printer_draw_bmp)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5 = NULL, **arg6 = NULL;
	printer *resource;
	HBITMAP hbmp;
	BITMAP bmp_property;
	HDC dummy;
	char* path;

	int args = ZEND_NUM_ARGS();
	switch (args) {
		case 4:
			if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE ) {
				RETURN_FALSE;
			}
			break;
		case 6:
			if (zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE ) {
				RETURN_FALSE;
			}
			convert_to_long_ex(arg5);
			convert_to_long_ex(arg6);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);

	virtual_filepath(Z_STRVAL_PP(arg2), &path TSRMLS_CC);

	hbmp = (HBITMAP)LoadImage(0, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE);

	if (hbmp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to load bitmap %s", Z_STRVAL_PP(arg2));
		RETURN_FALSE;
	}

	if (GetObject(hbmp, sizeof(BITMAP), &bmp_property) == 0) {
		if(hbmp)
			DeleteObject(hbmp);
		RETURN_FALSE;
	}

	if (!(GetDeviceCaps(resource->dc, RASTERCAPS) & RC_STRETCHBLT)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Printer does not support bitmaps");
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	if ((dummy = CreateCompatibleDC(resource->dc)) == NULL) {
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	if (SelectObject(dummy, hbmp) == NULL) {
		DeleteDC(dummy);
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	
	if (args > 4) {
		if (!StretchBlt(resource->dc,(int)Z_LVAL_PP(arg3), (int)Z_LVAL_PP(arg4), (int)Z_LVAL_PP(arg5), (int)Z_LVAL_PP(arg6), dummy, 0, 0, bmp_property.bmWidth,bmp_property.bmHeight, SRCCOPY)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Printer failed to accept bitmap");
			DeleteDC(dummy);
			DeleteObject(hbmp);
			RETURN_FALSE;
		}
	}
	else {
		if (!BitBlt(resource->dc, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), bmp_property.bmWidth, bmp_property.bmHeight, dummy, 0, 0, SRCCOPY)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Printer failed to accept bitmap");
			DeleteDC(dummy);
			DeleteObject(hbmp);
			RETURN_FALSE;
		}
	}

	DeleteDC(dummy);
	DeleteObject(hbmp);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void printer_abort(resource handle)
   Abort printing*/
PHP_FUNCTION(printer_abort)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	
	AbortPrinter(resource->handle);
}
/* }}} */


char *get_default_printer(void) {
	PRINTER_INFO_2 *printer;
	DWORD need, received;
	char *printer_name = NULL, *strtok_buf = NULL, buffer[250];

	if(GetVersion() < 0x80000000){
		GetProfileString("windows", "device", ",,,", buffer, 250);
		php_strtok_r(buffer, ",", &strtok_buf);
		printer_name = pestrdup(buffer, 1);
	}
	else {
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &need, &received);
		if(need > 0) {
			printer = (PRINTER_INFO_2 *)emalloc(need+1);
			EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)printer, need, &need, &received);
			printer_name = pestrdup(printer->pPrinterName, 1);
			efree(printer);
		}
	}

	return printer_name;
}


int hex2dec(char hex){
	switch(hex) { 
		case 'F': case 'f': 
			return 15;
			break;
        case 'E': case 'e':
			return 14;
			break;
        case 'D': case 'd':
			return 13;
			break;
        case 'C': case 'c':
			return 12;
			break;
        case 'B': case 'b':
			return 11;
			break;
        case 'A': case 'a':
			return 10;
        default:
			return (int)hex;
	}
}

/* convert a hexadecimal number to the rgb colorref */
COLORREF hex_to_rgb(char* hex)
{
	int r = 0,g = 0,b = 0;

	if(strlen(hex) < 6) {
		return RGB(0,0,0);
	}
	else {
		r = hex2dec(hex[0])*16 + hex2dec(hex[1]);
		g = hex2dec(hex[2])*16 + hex2dec(hex[3]);
		b = hex2dec(hex[4])*16 + hex2dec(hex[5]);
		return RGB(r,g,b);
	}

}

/* convert an rgb colorref to hex number */
char *rgb_to_hex(COLORREF rgb)
{
	char* string = emalloc(sizeof(char)*6);
	sprintf(string, "%02x%02x%02x", GetRValue(rgb), GetGValue(rgb),GetBValue(rgb));
	return string;
} 

static void printer_close(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	printer *p = (printer*)resource->ptr;

	ClosePrinter(p->handle);
	if (p->info.lpszDocName) {
		efree((char *)p->info.lpszDocName);
	}
	if (p->info.lpszOutput) {
		efree((char *)p->info.lpszOutput);
	}
	if (p->info.lpszDatatype) {
		efree((char *)p->info.lpszDatatype);
	}
	if (p->pi2) {
		if (p->pi2->pDevMode) {
			efree((char *)p->pi2->pDevMode);
		}
		efree((char *)p->pi2);
	}
	efree(p);
}
static void object_close(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	HGDIOBJ p = (HGDIOBJ)resource->ptr;
	DeleteObject(p);
}

#endif
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
