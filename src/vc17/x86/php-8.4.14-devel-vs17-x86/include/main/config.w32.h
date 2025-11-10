/*
	Build Configuration Template for Win32.
*/

/* Define the minimum supported version */
#undef _WIN32_WINNT
#undef NTDDI_VERSION
#define _WIN32_WINNT 0x0602
#define NTDDI_VERSION 0x06020000

/* Default PHP / PEAR directories */
#define PHP_CONFIG_FILE_PATH ""
#define PEAR_INSTALLDIR "C:\\php\\pear"
#define PHP_BINDIR "C:\\php"
#define PHP_SBINDIR "C:\\php"
#define PHP_DATADIR "C:\\php"
#define PHP_EXTENSION_DIR "C:\\php\\ext"
#define PHP_INCLUDE_PATH	".;C:\\php\\pear"
#define PHP_LIBDIR "C:\\php"
#define PHP_LOCALSTATEDIR "C:\\php"
#define PHP_PREFIX "C:\\php"
#define PHP_SYSCONFDIR "C:\\php"

/* PHP Runtime Configuration */
#define DEFAULT_SHORT_OPEN_TAG "1"

/* Platform-Specific Configuration. Should not be changed. */
/* Alignment for Zend memory allocator */
#define ZEND_MM_ALIGNMENT (size_t)8
#define ZEND_MM_ALIGNMENT_LOG2 (size_t)3
#define ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT 0
#define PHP_SIGCHILD 0
#define HAVE_GETSERVBYNAME 1
#define HAVE_GETSERVBYPORT 1
#define HAVE_GETPROTOBYNAME 1
#define HAVE_GETPROTOBYNUMBER 1
#define HAVE_GETHOSTNAME 1
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#undef HAVE_ADABAS
#undef HAVE_SOLID
#undef HAVE_SYMLINK

/* its in win32/time.c */
#define HAVE_USLEEP 1
#define HAVE_NANOSLEEP 1

#define HAVE_GETCWD 1
#undef HAVE_SETITIMER
#undef HAVE_IODBC
#define HAVE_LIBDL 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_PUTENV 1
#define HAVE_TZSET 1
#undef HAVE_FLOCK
#define HAVE_ALLOCA 1
#undef HAVE_SYS_TIME_H
#undef HAVE_STRUCT_STAT_ST_BLKSIZE
#undef HAVE_STRUCT_STAT_ST_BLOCKS
#define HAVE_STRUCT_STAT_ST_RDEV 1
#define HAVE_GETLOGIN 1
#define HAVE_SHUTDOWN 1
#define HAVE_STRCASECMP 1
#define HAVE_UTIME 1
#undef HAVE_DIRENT_H
#define HAVE_FCNTL_H 1
#undef HAVE_GRP_H
#undef HAVE_PWD_H
#undef HAVE_SYS_FILE_H
#undef HAVE_SYS_SOCKET_H
#undef HAVE_SYS_WAIT_H
#define HAVE_SYSLOG_H 1
#undef HAVE_UNISTD_H
#define HAVE_SYS_TYPES_H 1
#undef HAVE_ALLOCA_H
#undef HAVE_KILL
#define HAVE_GETPID 1
/* int and long are still 32bit in 64bit compiles */
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
/* MSVC.6/NET don't allow 'long long' or know 'intmax_t' */
#define SIZEOF_LONG_LONG 8 /* defined as __int64 */
#define SIZEOF_INTMAX_T 0
#define ssize_t SSIZE_T
#ifdef _WIN64
# define SIZEOF_SIZE_T 8
# define SIZEOF_PTRDIFF_T 8
#else
# define SIZEOF_SIZE_T 4
# define SIZEOF_PTRDIFF_T 4
#endif
#define SIZEOF_OFF_T 4
#define HAVE_FNMATCH
#define HAVE_GLOB
#define PHP_SHLIB_SUFFIX "dll"
#define PHP_SHLIB_EXT_PREFIX "php_"

/* Win32 supports socketpair by the emulation in win32/sockets.c */
#define HAVE_SOCKETPAIR 1
#define HAVE_SOCKLEN_T 1

/* Win32 support proc_open */
#define PHP_CAN_SUPPORT_PROC_OPEN 1

/* vs.net 2005 has a 64-bit time_t.  This will likely break
 * 3rdParty libs that were built with older compilers; switch
 * back to 32-bit */
#ifndef _WIN64
# define _USE_32BIT_TIME_T 1
#endif

#define _REENTRANT 1

#define HAVE_GETRUSAGE

#define HAVE_FTOK 1

#define HAVE_NICE

#ifdef __clang__
#define HAVE_FUNC_ATTRIBUTE_TARGET 1
#endif

#define HAVE_GETADDRINFO 1

/* values determined by configure.js */

/* The system that PHP was built on. */
#define PHP_BUILD_SYSTEM "Microsoft Windows Server 2022 Datacenter [10.0.20348]"

/* Configure line */
#define CONFIGURE_COMMAND "cscript /nologo /e:jscript configure.js  \"--enable-snapshot-build\"" " \"--enable-debug-pack\" \"--disable-zts\" \"--enable-object-out-dir=../obj/\"" " \"--enable-com-dotnet=shared\" \"--without-analyzer\" \"--with-pgo\""

/* The compiler used for the PHP build. */
#define PHP_BUILD_COMPILER "Visual C++ 2022"

/* Compiler compatibility ID */
#define PHP_COMPILER_ID "VS17"

/* The build architecture. */
#define PHP_BUILD_ARCH "x86"

/* Linker major version */
#define PHP_LINKER_MAJOR 14

/* Linker minor version */
#define PHP_LINKER_MINOR 44

#define HAVE_STRNLEN 1

#define ZEND_CHECK_STACK_LIMIT 1

#define HAVE_GAI_STRERROR 1

#define HAVE_IPV6 1

#define __SSE__ 1

#define __SSE2__ 1

/* Define to 1 if the PHP extension 'bcmath' is available. */
#define HAVE_BCMATH 1

/* Define to 1 if the PHP extension 'bz2' is available. */
#define HAVE_BZ2 1

/* Define to 1 if the PHP extension 'calendar' is available. */
#define HAVE_CALENDAR 1

/* Define to 1 if the PHP extension 'com_dotnet' is available. */
#define HAVE_COM_DOTNET 1

/* Define to 1 if the PHP extension 'ctype' is available. */
#define HAVE_CTYPE 1

/* Define to 1 if the PHP extension 'curl' is available. */
#define HAVE_CURL 1

/* Define to 1 if you have the <timelib_config.h> header file. */
#define HAVE_TIMELIB_CONFIG_H 1

/* Define to 1 if the PHP extension 'dba' is available. */
#define HAVE_DBA 1

/* The QDBM handler header file. */
#define QDBM_INCLUDE_FILE <depot.h>

/* Define to 1 if the dba extension uses the QDBM handler. */
#define DBA_QDBM 1

/* The LMDB handler header file. */
#define LMDB_INCLUDE_FILE <lmdb.h>

/* Define to 1 if the dba extension uses the LMDB handler. */
#define DBA_LMDB 1

/* Define to 1 if the PHP extension 'enchant' is available. */
#define HAVE_ENCHANT 1

/* Define to 1 if Enchant library has the 'enchant_get_version' function (available since 1.6.0). */
#define HAVE_ENCHANT_GET_VERSION 1

/* Define to 1 if the PHP extension 'ffi' is available. */
#define HAVE_FFI 1

/* Define to 1 if libffi supports the fastcall calling convention. */
#define HAVE_FFI_FASTCALL 1

/* Define to 1 if libffi supports the thiscall calling convention. */
#define HAVE_FFI_THISCALL 1

/* Define to 1 if libffi supports the stdcall calling convention. */
#define HAVE_FFI_STDCALL 1

/* Define to 1 if libffi supports the ms_cdecl calling convention. */
#define HAVE_FFI_MS_CDECL 1

/* Define to 1 if libffi supports the sysv calling convention. */
#define HAVE_FFI_SYSV 1

/* Define to 1 if libffi partially supports the vectorcall calling convention. */
#define HAVE_FFI_VECTORCALL_PARTIAL 1

/* Define to 1 if FTP over SSL is enabled. */
#define HAVE_FTP_SSL 1

/* Define to 1 if the PHP extension 'ftp' is available. */
#define HAVE_FTP 1

/* Define to 1 if you have the xpm library. */
#define HAVE_XPM 1

/* Define to 1 if gd extension has XPM support. */
#define HAVE_GD_XPM 1

/* Define to 1 if you have the libwebp library. */
#define HAVE_LIBWEBP 1

/* Define to 1 if gd extension has WebP support. */
#define HAVE_GD_WEBP 1

/* Define to 1 if gd extension uses GD library bundled in PHP. */
#define HAVE_GD_BUNDLED 1

/* Define to 1 if gd extension has PNG support. */
#define HAVE_GD_PNG 1

/* Define to 1 if gd extension has BMP support. */
#define HAVE_GD_BMP 1

/* Define to 1 if gd extension has TGA support. */
#define HAVE_GD_TGA 1

/* Define to 1 if you have the libpng library. */
#define HAVE_LIBPNG 1

/* Define to 1 if you have the libjpeg library. */
#define HAVE_LIBJPEG 1

/* Define to 1 if gd extension has JPEG support. */
#define HAVE_GD_JPG 1

/* Define to 1 if you have the FreeType library. */
#define HAVE_LIBFREETYPE 1

/* Define to 1 if gd extension has FreeType support. */
#define HAVE_GD_FREETYPE 1

/* Define to 1 if the PHP extension 'gmp' is available. */
#define HAVE_GMP 1

/* Define to 1 if mhash support is enabled. */
#define PHP_MHASH_BC 1

/* Define to 1 if the PHP extension 'iconv' is available. */
#define HAVE_ICONV 1

/* Define to 1 if you have the 'libiconv' function. */
#define HAVE_LIBICONV 1

/* Define to 1 if 'iconv()' is aliased to 'libiconv()'. */
#define ICONV_ALIASED_LIBICONV 1

/* The iconv implementation. */
#define PHP_ICONV_IMPL "\"libiconv\""

#define HAVE_LDAP_PARSE_RESULT 1

#define HAVE_LDAP_PARSE_REFERENCE 1

#define HAVE_LDAP_START_TLS_S 1

/* Define to 1 if the PHP extension 'ldap' is available. */
#define HAVE_LDAP 1

/* Define to 1 if the ldap extension has SASL support enabled. */
#define HAVE_LDAP_SASL 1

#define HAVE_LDAP_CONTROL_FIND 1

#define HAVE_LDAP_PARSE_EXTENDED_RESULT 1

#define HAVE_LDAP_EXTENDED_OPERATION_S 1

#define HAVE_LDAP_PASSWD 1

#define HAVE_LDAP_WHOAMI_S 1

#define HAVE_LDAP_REFRESH_S 1

#define HAVE_LDAP_EXTENDED_OPERATION 1

/* Define to 1 if the PHP extension 'odbc' is available. */
#define HAVE_UODBC 1

/* Define to 1 if ODBC library has 'SQLDataSources', as a function or macro. */
#define HAVE_SQLDATASOURCES 1

/* The highest supported ODBC version. */
#define ODBCVER 0x0350

/* Define to 1 if the PHP extension 'openssl' is available. */
#define HAVE_OPENSSL_EXT 1

#define HAVE_OSSL_SET_MAX_THREADS 0

/* Define to 1 if PHP uses the bundled PCRE library. */
#define HAVE_BUNDLED_PCRE 1

/* Number of bits in non-UTF mode for PCRE library. */
#define PCRE2_CODE_UNIT_WIDTH 8

/* Define to 1 if PCRE library is built statically. */
#define PCRE2_STATIC 1

/* Define to 1 if PCRE JIT is enabled and supported. */
#define HAVE_PCRE_JIT_SUPPORT 1

/* Define to 1 if the PHP extension 'shmop' is available. */
#define HAVE_SHMOP 1

/* Define to 1 if the PHP extension 'sockets' is available. */
#define HAVE_SOCKETS 1

/* Define to 1 if the PHP extension 'sodium' is available. */
#define HAVE_LIBSODIUMLIB 1

/* Define to 1 if the PHP extension 'sqlite3' is available. */
#define HAVE_SQLITE3 1

/* Define to 1 if SQLite library has the 'sqlite3_errstr' function. */
#define HAVE_SQLITE3_ERRSTR 1

/* Define to 1 if SQLite library has the 'sqlite3_expanded_sql' function. */
#define HAVE_SQLITE3_EXPANDED_SQL 1

/* Define to 1 if you have the <argon2.h> header file. */
#define HAVE_ARGON2_H 1

/* Define to 1 if the system has the 'libargon2' library. */
#define HAVE_ARGON2LIB 1

#define PHP_CONFIG_FILE_SCAN_DIR ""

/* Define to 1 if PHP uses its own crypt_r, and to 0 if using the external crypt library. */
#define PHP_USE_PHP_CRYPT_R 1

/* Define to 1 if the PHP extension 'sysvshm' is available. */
#define HAVE_SYSVSHM 1

/* Define to 1 if you have the <tidybuffio.h> header file. */
#define HAVE_TIDYBUFFIO_H 1

/* Define to 1 if the PHP extension 'tidy' is available. */
#define HAVE_TIDY 1

/* Define to 1 if you have the <tidy.h> header file. */
#define HAVE_TIDY_H 1

/* Define to 1 if Tidy library has the 'tidyOptGetDoc' function. */
#define HAVE_TIDYOPTGETDOC 1

/* Define to 1 if Tidy library has the 'tidyReleaseDate' function. */
#define HAVE_TIDYRELEASEDATE 1

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define to 1 if the PHP extension 'zlib' is available. */
#define HAVE_ZLIB 1

/* Define to 1 if the PHP extension 'libxml' is available. */
#define HAVE_LIBXML 1

/* Define to 1 if the PHP extension 'dom' is available. */
#define HAVE_DOM 1

/* Define to 1 if the PHP extension 'mbstring' is available. */
#define HAVE_MBSTRING 1

/* Define to 1 if mbstring has multibyte regex support enabled. */
#define HAVE_MBREGEX 1

/* Define to 1 if the PHP extension 'exif' is available. */
#define HAVE_EXIF 1

/* Define to 1 if mysqlnd has compressed protocol support. */
#define MYSQLND_COMPRESSION_ENABLED 1

/* Define to 1 if mysqlnd core SSL is enabled. */
#define MYSQLND_SSL_SUPPORTED 1

/* Define to 1 if mysqlnd extended SSL is enabled through a system library. */
#define MYSQLND_HAVE_SSL 1

/* Define to 1 to enable JIT. */
#define HAVE_JIT 1

/* Define to 1 if the pdo_mysql extension uses mysqlnd. */
#define PDO_USE_MYSQLND 1

/* Define to 1 if the PHP extension 'pdo_pgsql' is available. */
#define HAVE_PDO_PGSQL 1

/* Define to 1 if SQLite library was compiled with the SQLITE_ENABLE_COLUMN_METADATA and has the 'sqlite3_column_table_name' function. */
#define HAVE_SQLITE3_COLUMN_TABLE_NAME 1

/* Define to 1 if SQLite library has the 'sqlite3_close_v2' function. */
#define HAVE_SQLITE3_CLOSE_V2 1

/* Define to 1 if the PHP extension 'pgsql' is available. */
#define HAVE_PGSQL 1

/* Define to 1 if the PHP extension 'session' is available. */
#define HAVE_PHP_SESSION 1

/* Define to 1 if the PHP extension 'simplexml' is available. */
#define HAVE_SIMPLEXML 1

/* Define to 1 if the PHP extension 'snmp' is available. */
#define HAVE_SNMP 1

/* Define to 1 if the PHP extension 'soap' is available. */
#define HAVE_SOAP 1

/* Define to 1 if the PHP extension 'xml' is available. */
#define HAVE_XML 1

/* Define to 1 if the PHP extension 'xmlreader' is available. */
#define HAVE_XMLREADER 1

/* Define to 1 if the PHP extension 'xmlwriter' is available. */
#define HAVE_XMLWRITER 1

/* Define to 1 if the system has the EXSLT extension library for XSLT. */
#define HAVE_XSL_EXSLT 1

/* Define to 1 if the PHP extension 'xsl' is available. */
#define HAVE_XSL 1

#define ZIP_STATIC 1

/* Define to 1 if the PHP extension 'zip' is available. */
#define HAVE_ZIP 1

#if __has_include("main/config.pickle.h")
#include "main/config.pickle.h"
#endif
