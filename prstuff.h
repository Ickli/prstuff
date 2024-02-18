#ifndef _PRSTUFF_C_
#define _PRSTUFF_C_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <locale.h>


// <SETTING VARIABLES>

const char* _prt_framevert_def = "║";
const char* _prt_vert_def = "│";
const char* _prt_empty_def = " ";

// </SETTING VARIABLES>


// <DEFINE_STRUCTURES>
#define _PR_VECTOR
#define _PR_PAIR
#define _PR_TABLEINFO // drawing table
#define _PR_HASHTABLE
#define COMMON_FUNCTIONS
// </DEFINE_STRUCTURES>


// <DEFINE_STRUCTURES CONNECTIONS> [goes from most to least complex structures, so no excessive difficulties while including sth]
#if defined(_PR_TABLEINFO)
#define _PR_VECTOR
#endif
// </DEFINE_STRUCTURES CONNECTIONS>





// <OPTIONS>
#define PRT_FLOATINGPOINT_PRECISION "8"
// </OPTIONS>




// <USER ORIENTED MACROS>
#define setrus setlocale(LC_ALL, "Russian")
#define setall setlocale(LC_ALL, "")
#define _PR_PROMPT_FAILURESTRING "Упс! Вы ввели неправильное значение.\n"
#define prPrompt(prompt, specString, argNum, ...) \
	printf(prompt);\
       	if(scanf(specString, __VA_ARGS__) != argNum) {\
		printf(_PR_PROMPT_FAILURESTRING);\
		exit(1);\
	}
// </USER ORIENTED DEFINES>




// <INNER ORIENTED MACROS>
#define _PR_CONCAT(value1, value2) value1##value2 
#define _PR_RETURN(value) value

#define _PRNOT1 0
#define _PRNOT0 1

#define _PRIF1(value) _PR_RETURN(value)
#define _PRIF0(value)

#define _PRIF(num, ifvalue) _PR_CONCAT(_PRIF, num)(ifvalue)
#define _PRIFNOT(num, ifnotvalue) _PRIF(_PR_CONCAT(_PRNOT, num), ifnotvalue)
#define _PRIFELSE(num, ifvalue, elsevalue) _PRIF(num, ifvalue) _PRIFNOT(num, elsevalue)

#define _FUNCPOSTFIX_POINTER(derefType) derefType##p 
#define _FUNCPOSTFIX_POINTER_IFELSE(num, derefType) _PRIFELSE(num, _FUNCPOSTFIX_POINTER(derefType), derefType)
// </INNER ORIENTED MACROS>




typedef unsigned int uint;
typedef unsigned char uchar;

size_t strlenz(const char *s) {
	return strlen(s) + 1;
}

void assert(int b, const char* errorMsg) {
	if(!b) {
		printf("%s\n", errorMsg);
		exit(1);
	}
}

uint utf8len(const char *s) {
    uint count = 0;
    while (*s) {
        count += (*s++ & 0xC0) != 0x80;
    }
    return count;
}


#ifdef _PR_VECTOR

typedef struct s_prVector {
	uint capacity;
	uint count;

	uint elementSize;
	// actually, array of bytes (not compatible with 7-bit byte and so on)
	void* elements; 
} prVector;

uint prVector_capacity_def = 2;

void prvFree(prVector* vector_p) {
	// vector_p->capacity = 0;
	// vector_p->count = 0;
	free(vector_p->elements);
}

void* _prvChangeArrP(prVector* vector_p, void* newArrP) {
	void* oldArrP = vector_p->elements;
	vector_p->elements = newArrP;
	return oldArrP;
}

uint _prvGetNewCapSize(prVector* vector_p, uint cap) {
	return vector_p->elementSize * cap;
}

// __cap is for tail call optimization
uint prvGetCapSize(prVector* vector_p, uint __cap) {
	return _prvGetNewCapSize(vector_p, vector_p->capacity);
}


prVector prvConstruct(uint capacity, uint elemSize) {
	prVector vec = {
		capacity,
		0,
		elemSize,
		0
	};
	vec.elements = malloc(_prvGetNewCapSize(&vec, capacity));
	return vec;
}

int prvCanPush(prVector* vector_p, uint elemCount) {
	return vector_p->count + elemCount <= vector_p->capacity;
}

void _prvCopyElements(prVector* vector_p, void* dest) {
	memcpy(dest, vector_p->elements, prvGetCapSize(vector_p, 0));
}

void _prvReserve(prVector* vector_p, uint desiredCapacity) {
	uint newCap = _prvGetNewCapSize(vector_p, desiredCapacity);

	void* newArrP = (void*)malloc(newCap);

	_prvCopyElements(vector_p, (void*)newArrP);
	prvFree(vector_p);
	_prvChangeArrP(vector_p, newArrP);

	vector_p->capacity = desiredCapacity;
}

void* _prvPlace(prVector* vector_p, const void* elems, uint index, uint count) {
	void* placeIndex = (char*)vector_p->elements + index*(vector_p->elementSize);
	memcpy(placeIndex, 
			elems, 
			_prvGetNewCapSize(vector_p, count)
	);

	return placeIndex;
}

void prvReserve(prVector* vector_p, uint desiredCapacity) {
	uint cap = vector_p->capacity;
	if(cap >= desiredCapacity)
		return;
	if(!cap) cap = 1;

	while (cap < desiredCapacity) cap *= 2;
	_prvReserve(vector_p, cap);
}

void* prvPushBack(prVector* vector_p, const void* elems, uint count) {
	prvReserve(vector_p, vector_p->count + count);
	void* elemPointer = _prvPlace(vector_p, elems, vector_p->count, count);
	vector_p->count += count;
	return elemPointer;
}

void* prvPlace(prVector* vector_p, void* elems, uint index, uint count) {
	prvReserve(vector_p, index+count);
	return _prvPlace(vector_p, elems, index, count);
}


#define prvCreate(capacity, type) prvConstruct(capacity, sizeof(type))
#define prvAt(vector, type, index) (((type*)(vector.elements))[index])
#define prv_pAt(vector_p, type, index) (((type*)(vector_p->elements))[index])
#define prvAtp(vector, type, index) ((type)vector.elements + index)
#define prv_pAtp(vector, type, index) ((type)vector->elements + index) 

#endif // _PR_VECTOR

#ifdef _PR_TABLEINFO
static const char* wchar_spec = "%lc";


typedef char* _prtValVecType;
typedef uint _prtLenVecType;
typedef struct s_TableInfo {
	// corners
	const char* leftupcorner;
	const char* leftdowncorner;
	const char* rightupcorner;
	const char* rightdowncorner;
	// frame
	const char* framehoriz;
	const char* framevert;
	const char* frameinter_down;
	const char* frameinter_up;
	const char* frameinter_right;
	const char* frameinter_left;
	// inner
	const char* horiz;
	const char* vert;
	const char* intersection;
	const char* empty;

	size_t framevertSize,
	       vertSize,
	       emptySize,
	       inCellPartSize;

	size_t columns,
	       rows;
	
	// widths
	size_t cellw,
	       borderValueOffset;

	prVector values;
	prVector lengths;
	prVector byteLengths;
	prVector allocatedValues; // when user passes non-string <not impl>or wishes to copy one</not impl> the table creates new string

	size_t curRow,
	       curColumn;

	// table parts
	char* downPart;
	char* upPart;
	char* rowDelimiterPart;
	char* inCellPart;
	bool arePartsRelevant;
} TableInfo;

static TableInfo _tableInfo;

void prtInit() {
	_tableInfo = (TableInfo){
	"╔",
	"╚",
	"╗",
	"╝",

	"═",
	_prt_framevert_def,
	"╤",
	"╧",
	"╟",
	"╢",

	"─",
	_prt_vert_def,
	"┼",
	_prt_empty_def,

	strlenz(_prt_framevert_def),
	strlenz(_prt_vert_def),
	strlenz(_prt_empty_def),
	0,

	0,
	0,

	0,
	1,

	prvConstruct(0, sizeof(_prtValVecType)),
	prvConstruct(0, sizeof(uint)),
	prvConstruct(0, sizeof(uint)),
	prvConstruct(0, sizeof(_prtValVecType)),
	};
}

void prtInitWith(uint columns, uint rows) {
	prtInit();
	_tableInfo.columns = columns;
	_tableInfo.rows = rows;
}

// const char* leftmost, const char* rightmost, const char* vertdelimiter, const char* inbetween
#define _PR_UPPART_SYMBOLS _tableInfo.leftupcorner,_tableInfo.rightupcorner,_tableInfo.frameinter_down,_tableInfo.framehoriz
#define _PR_DOWNPART_SYMBOLS _tableInfo.leftdowncorner,_tableInfo.rightdowncorner,_tableInfo.frameinter_up,_tableInfo.framehoriz
#define _PR_INCELLPART_SYMBOLS _tableInfo.framevert,_tableInfo.framevert,_tableInfo.vert, _tableInfo.empty
#define _PR_ROWDELIMITERPART_SYMBOLS _tableInfo.frameinter_right,_tableInfo.frameinter_left,_tableInfo.intersection,_tableInfo.horiz

#define prtSet(field, value) _tableInfo.##field = value; _tableInfo.arePartsRelevant = false

void _prtBroaden(uint w) {
	_tableInfo.cellw = w;
}


void prtPrint(const _prtValVecType string) {
	uint len = utf8len(string);
	uint bytelen = strlen(string);
	prvPushBack(&_tableInfo.lengths, &len, 1);
	prvPushBack(&_tableInfo.byteLengths, &bytelen, 1);

	len += 2*_tableInfo.borderValueOffset*utf8len(_tableInfo.empty); 
	if (_tableInfo.cellw < len) 
		_tableInfo.cellw = len;

	prvPushBack(&_tableInfo.values, (_prtValVecType*)&string, 1);

}

void _prtPrintsVA_ARGS(int n, va_list args) {
	//va_list args;
	for(int i = 0; i < n; i++) {
		_prtValVecType str = va_arg(args, char*);
		prtPrint(str);
	}
}

void prtPrints(int n, ...) {
	va_list args;
	va_start(args, n);
	_prtPrintsVA_ARGS(n, args);
	va_end(args);
}

void prtEnd();

// PRT_TABLE TEMPLATES

// max length for contents of a single cell
#define _MAX_PRTPRINT_NUMERIC_LENGTH 256

/* Declares function for printing some type into table.
 * scanf's value into string, string is owned by table.
 */
#define _DECLARE_PRTPRINT_RAW(funcPostfix, type, specifier) void prtPrint##funcPostfix( type num) {\
	_prtValVecType buf = (_prtValVecType)calloc(_MAX_PRTPRINT_NUMERIC_LENGTH, sizeof(char)); \
	sprintf(buf, specifier, num); \
	\
	uint trimmedBufLen = strlen(buf); \
	_prtValVecType trimmedBuf = (_prtValVecType)calloc(trimmedBufLen, sizeof(char)); \
	memcpy(trimmedBuf, buf, trimmedBufLen*sizeof(char)); \
	\
	\
	free(buf); \
	prvPushBack(&_tableInfo.allocatedValues, &trimmedBuf, 1); \
	prtPrint(trimmedBuf); \
}

/* Declares function printing primitive 
 * non-pointer types which can be scanf'ed.
 */
#define _DECLARE_PRTPRINT_NUMERIC(type, specifier) _DECLARE_PRTPRINT_RAW(type, type, specifier)

/* Declares function printing
 * for pointers to primitive types which can be scanf'ed.
 */
#define _DECLARE_PRTPRINT_POINTER(derefType, specifier) _DECLARE_PRTPRINT_RAW(_FUNCPOSTFIX_POINTER(derefType), derefType*, specifier)

/* Declares:
 * 1. Function accepting va_list as argument and iterating over it
 *    to forward to some prtPrint* function.
 * 2. Function accepting '...' as argument and forwarding it to
 *    the first function.
 */
#define _DECLARE_PRTPRINTS_RAW(funcPostfix, type)\
	void _prtPrintsVA_ARGS##funcPostfix(int n, va_list args) { \
	\
		for(int i = 0; i < n; i++) {\
		prtPrint##funcPostfix(va_arg(args, type));\
		}\
	}\
	\
	void prtPrints##funcPostfix(int n, ...) { \
		va_list args;\
		va_start(args, n);\
		_prtPrintsVA_ARGS##funcPostfix(n, args);\
		va_end(args);\
	}

/* Declares function printing variadic number of 
 * primitive non-pointer type arguments
 * which can be scanf'ed.
 */
#define _DECLARE_PRTPRINTS_NUMERIC(type) _DECLARE_PRTPRINTS_RAW(type, type)

/* Declares function printing variadic number of 
 * pointers to primite type which can be scanf'ed.
 */
#define _DECLARE_PRTPRINTS_POINTER(derefType) _DECLARE_PRTPRINTS_RAW(_FUNCPOSTFIX_POINTER(derefType), derefType*)

/* Declares function printing an argument of some type,
 * which can be scanf'ed, and ending current print session.
 *
 * Must be used only after appropriate _DECLARE_PRTPRINT_* macro is used
 */
#define _DECLARE_PRTPRINTEND_RAW(funcPostfix, type) void prtPrintEnd##funcPostfix(type num) { \
	prtPrint##funcPostfix(num);\
	prtEnd();\
}

/* Declares function printing arguments of some type,
 * which can be scanf'ed, and ending current print session
 *
 * Must be used only after appropriate _DECLARE_PRTPRINT_* macro is used
 */
#define _DECLARE_PRTPRINTEND_NUMERIC(type) _DECLARE_PRTPRINTEND_RAW(type, type)

/* Declares function printing arguments of some type,
 * which can be scanf'ed, and ending current print session
 *
 * Must be used only after appropriate _DECLARE_PRTPRINT_* macro is used
 */
#define _DECLARE_PRTPRINTEND_POINTER(derefType) _DECLARE_PRTPRINTEND_RAW(_FUNCPOSTFIX_POINTER(derefType), derefType*)

/* Declares function printing variadic number of 
 * arguments of some type, which can be scanf'ed,
 * and ending current print session.
 *
 * Must be used only after appropriate _DECLARE_PRTPRINTS_* macro is used
 */
#define _DECLARE_PRTPRINTSEND_RAW(funcPostfix, type) void prtPrintsEnd##funcPostfix(int num, ...) { \
	va_list args;\
	va_start(args, num);\
	_prtPrintsVA_ARGS##funcPostfix(num, args);\
	prtEnd();\
	va_end(args);\
}

/* Declares function printing variadic number of 
 * arguments of primitive type, which can be scanf'ed,
 * and ending current print session.
 *
 * Must be used only after appropriate _DECLARE_PRTPRINTS_* macro is used
 */
#define _DECLARE_PRTPRINTSEND_NUMERIC(type) _DECLARE_PRTPRINTSEND_RAW(type, type)

/* Declares function printing variadic number of 
 * pointers to primite type, which can be scanf'ed,
 * and ending current print session.
 *
 * Must be used only after appropriate _DECLARE_PRTPRINTS_* macro is used
 */
#define _DECLARE_PRTPRINTSEND_POINTER(derefType) _DECLARE_PRTPRINTSEND_RAW(_FUNCPOSTFIX_POINTER(derefType), derefType*)

/* Declares a bunch of printing functions
 * printing some type, which can be scanf'ed
 */
#define _DECLARE_PRTPRINT_PRTPRINTS_RAW(funcPostfix, type, specifier) \
	_DECLARE_PRTPRINT_RAW(funcPostfix, type, specifier) \
	_DECLARE_PRTPRINTS_RAW(funcPostfix, type) \
	_DECLARE_PRTPRINTEND_RAW(funcPostfix, type) \
	_DECLARE_PRTPRINTSEND_RAW(funcPostfix, type)

/* Declares a bunch of printing functions
 * printing some primitive type, which can be scanf'ed
 */
#define _DECLARE_PRTPRINT_PRTPRINTS_NUMERIC(type, specifier) _DECLARE_PRTPRINT_PRTPRINTS_RAW(type, type, specifier)

/* Declares a bunch of printing functions
 * printing pointers to primitive type, which can be scanf'ed
 */
#define _DECLARE_PRTPRINT_PRTPRINTS_POINTER(derefType, specifier) _DECLARE_PRTPRINT_PRTPRINTS_RAW(_FUNCPOSTFIX_POINTER(derefType), derefType*, specifier)

_DECLARE_PRTPRINT_PRTPRINTS_NUMERIC(int, "%d")
_DECLARE_PRTPRINT_PRTPRINTS_NUMERIC(double, "%" "." PRT_FLOATINGPOINT_PRECISION "lf")
_DECLARE_PRTPRINT_PRTPRINTS_POINTER(int, "%p")
_DECLARE_PRTPRINT_PRTPRINTS_POINTER(void, "%p")


#ifdef __cplusplus
#include <string>
#include <iostream>
#include <sstream>


/* Uses << operator for printing into cell.
 * New string is owned by table.
 */
template<typename T>
void prtStreamPrint(T&& value) {
	std::stringstream bufstr;
	bufstr << value;

	auto len = strlen(bufstr.str().c_str());
	_prtValVecType buf = (_prtValVecType)calloc(len, sizeof(char));
	assert(buf != NULL, "ERROR: prtStreamPrint не удалось выделить память для буфера\n");

	memcpy(buf, bufstr.str().c_str(), len);

	prvPushBack(&_tableInfo.allocatedValues, 
			&buf, 
			1
	);
	prtPrint(buf);
}
#endif // cplusplus

// --PRT_TABLE TEMPLATES

void prtEmptyValues() {
	prvFree(&_tableInfo.values);
	prvFree(&_tableInfo.byteLengths);
	prvFree(&_tableInfo.lengths);

	uint alValCount = _tableInfo.allocatedValues.count;
	for(int i = 0; i < alValCount; i++)
		free(prvAt(_tableInfo.allocatedValues, _prtValVecType, i));
	prvFree(&_tableInfo.allocatedValues);
}

void _prtFreeTableParts() {
	free(_tableInfo.inCellPart);
	free(_tableInfo.upPart);
	free(_tableInfo.downPart);
	free(_tableInfo.rowDelimiterPart);
}

uint _prtGetTablePartLen(const char* leftmost, const char* rightmost, const char* vertdelimiter, const char* inbetween) {
	return strlen(leftmost) 
	       + strlen(rightmost)
	       + strlen(vertdelimiter)*(_tableInfo.columns-1) 
	       + strlen(inbetween)*_tableInfo.columns*_tableInfo.cellw 
	       + 1;
}

_prtValVecType _prtCallocTablePart(uint len) {
	return (char*)calloc(len, sizeof(char));
}

_prtValVecType _prtPlaceTablePartElement(char* tablePart, const char* element, uint elemLen) {
	elemLen <= 0 ? elemLen = strlen(element) : elemLen;
	memcpy(tablePart, element, elemLen);
	return tablePart + elemLen;
}

void _prtMemcpyString(char* dest, char* src, uint size, uint count) {
	for(;count > 0; count--) {
		memcpy(dest, src, size);
		dest += size;
	}
}

uint _prtConstructTablePart(char** tablePartPointer, 
			    const char* leftmost, 
			    const char* rightmost, 
			    const char* vertdelimiter, 
			    const char* inbetween) 
{

	uint partLen = _prtGetTablePartLen(leftmost, rightmost, vertdelimiter, inbetween);
	char* tablePart = _prtCallocTablePart(partLen);
	*tablePartPointer = tablePart;

	// leftmost element placement
	char* curPartPos = _prtPlaceTablePartElement(tablePart, leftmost, 0);
	
	uint inbetweenStringLen = strlen(inbetween)*_tableInfo.cellw;
	uint vertdelimiterLen = strlen(vertdelimiter);
	char* inbetweenString = (char*)malloc(inbetweenStringLen);
	_prtMemcpyString(inbetweenString, ((char*)inbetween), strlen(inbetween), _tableInfo.cellw);

	bool lastElemIsVertDelimiter = false;
	for(uint step = 0; step < _tableInfo.columns; step++) {
		lastElemIsVertDelimiter = true;
		curPartPos = _prtPlaceTablePartElement(curPartPos, inbetweenString, inbetweenStringLen);
		curPartPos = _prtPlaceTablePartElement(curPartPos, vertdelimiter, vertdelimiterLen);
	}

	if (lastElemIsVertDelimiter)
	_prtPlaceTablePartElement(curPartPos - vertdelimiterLen, rightmost, 0);

	return partLen;
}

void _prtConstructTableParts() {
	if(_tableInfo.arePartsRelevant) return;
	_prtFreeTableParts();

	uint vertDelimAmount = _tableInfo.columns - 1;

	_prtConstructTablePart(&_tableInfo.upPart, _PR_UPPART_SYMBOLS);
	_prtConstructTablePart(&_tableInfo.downPart, _PR_DOWNPART_SYMBOLS);
	_tableInfo.inCellPartSize = _prtConstructTablePart(&_tableInfo.inCellPart, _PR_INCELLPART_SYMBOLS);
	_prtConstructTablePart(&_tableInfo.rowDelimiterPart, _PR_ROWDELIMITERPART_SYMBOLS);
}

_prtValVecType _prtGetCellValue(uint cindex) {
	return prvAt(_tableInfo.values, char*, cindex);
}

uint _prtPrintString(_prtValVecType* string, uint count) {
	printf("%.*s", count, *string);
	*string += count;
	return count;
}

uint _prtPrintCellValue(uint cindex) {
	_prtValVecType value = _prtGetCellValue(cindex);
	printf("%.*s", prvAt(_tableInfo.byteLengths, uint, cindex), value);
	return prvAt(_tableInfo.lengths, uint, cindex);
}

void _prtPrintNonContentPart(const char* part) {
	printf("%s\n", part);
}

uint _prtPrintContentPart(uint cindex) {
	_prtValVecType incell = _tableInfo.inCellPart;
	_prtValVecType incellEnd = _tableInfo.inCellPart + _tableInfo.inCellPartSize;
	uint curPos = 0;
	uint betweenValueOffset = _tableInfo.cellw + _tableInfo.vertSize;
	uint cellValueLen = 0;
	
	curPos += _prtPrintString(&incell, 1 + _tableInfo.emptySize*_tableInfo.borderValueOffset); // print start
	uint nextValPos = curPos + betweenValueOffset;

	uint valuesLeftToReturn = (_tableInfo.columns > _tableInfo.values.count ? _tableInfo.values.count : _tableInfo.columns);
	int valuesLeft = valuesLeftToReturn - 1;
	

	do {
		cellValueLen = _prtPrintCellValue(cindex); // print value
		curPos += cellValueLen; // increase curpos
		incell += cellValueLen; // increase incell

		_prtPrintString(&incell, nextValPos - curPos - 1); // print next (nextvalpos - curpos) chars
		curPos = nextValPos;
		nextValPos += betweenValueOffset;

		valuesLeft--;
		cindex++;
	} while (valuesLeft >= 0);

	if(incell < incellEnd)
		printf("%s", incell);
	printf("\n");
	return valuesLeftToReturn;
}

/* Ends current print session by printing table */
void prtEndNotEmpty() {
	if(_tableInfo.lengths.count == 0) {
		static char empty[] = {0};
		prtPrint(empty);
	}

	_prtConstructTableParts();

	_prtPrintNonContentPart(_tableInfo.upPart);

	uint cindex = _prtPrintContentPart(0);
	uint rowPrinted = 1;
	while(rowPrinted < _tableInfo.rows && cindex < _tableInfo.values.count) {
		
		_prtPrintNonContentPart(_tableInfo.rowDelimiterPart);
		cindex += _prtPrintContentPart(cindex);
		rowPrinted++;
	}

	_prtPrintNonContentPart(_tableInfo.downPart);
}

/* Ends current print session by printing table,
 * frees table parts and owned strings
 */
void prtEnd() {
	prtEndNotEmpty();
	prtEmptyValues();
}


// PRT_TABLE TEMPLATES

//#define _DECLARE_JUSTPRINT(type)

/* Initializes table, prints variadic numbers 
 * of arguments with 'string' prompt and immediately
 * ends print session
 */
void prtJustPrint(const _prtValVecType string, int n, ...) {
	prtInitWith(n+1, 1);
	prtPrint(string);

	va_list args;
	va_start(args, n);
	_prtPrintsVA_ARGS(n, args);
	prtEnd();
	va_end(args);
}

/* Defines function for immediate printing 
 * of some type which can be scanf'ed
 */
#define _DECLARE_PRTJUSTPRINT(type) void prtJustPrint##type(const _prtValVecType string, int n, ...) {\
	prtInitWith(n+1, 1);\
	prtPrint(string);\
	\
	va_list args;\
	va_start(args, n);\
	_prtPrintsVA_ARGS##type(n, args);\
	prtEnd();\
	va_end(args);\
}

_DECLARE_PRTJUSTPRINT(int)
_DECLARE_PRTJUSTPRINT(double)

// --PRT_TABLE TEMPLATES



#endif // _PR_TABLEINFO

#ifndef COMMON_FUNCTIONS_

#define FPLEN 128

typedef struct {
	char path[FPLEN];
	FILE* file;
} FileInfo;

#ifdef __cplusplus
#include <iostream>
#include <fstream>
#include <cstring>

struct StreamInfo {
	char path[FPLEN];
	std::fstream file;
};

StreamInfo safeOpenStreamWithInfo(const char* path, std::ios_base::openmode format) {
	StreamInfo si;
	si.file.open(path, format);
	assert(!si.file.fail(), "Не удалось открыть поток для файла\n");

	strcpy(si.path, path);
	return si;
}

StreamInfo tryOpenStreamWithInfo(int argc, char* argv[], std::ios_base::openmode format) {
        char buf[FPLEN];
        if(argc < 2) {
                printf("Введите путь до файла: ");
                assert(scanf("%s", buf) == 1, "Путь указан неверно\n");
                return safeOpenStreamWithInfo(buf, format);
        }
        return safeOpenStreamWithInfo(argv[1], format);

}

#endif // __cplusplus

FileInfo safeOpenWithInfo(const char* filepath, const char* format) {
	FileInfo fi;
	assert((long)(fi.file = fopen(filepath, format)), "Не удалось открыть файл\n");
	strcpy(fi.path, filepath);
	return fi;
}

FILE* safeOpen(const char* filepath, const char* format) {
	return safeOpenWithInfo(filepath, format).file;
}

FileInfo tryOpenWithInfo(int argc, char* argv[], const char* format) {
	FileInfo fi;
	if(argc < 2) {
		printf("Введите путь до файла: ");
		assert(scanf("%s", fi.path) == 1, "Путь указан неверно\n");
		fi.file = safeOpen(fi.path, format);
		return fi;
	}
	strcpy(fi.path, argv[1]);
	fi.file = safeOpen(fi.path, format);
	return fi;
}

FILE* tryOpen(int argc, char* argv[], const char* format) {
	return tryOpenWithInfo(argc, argv, format).file;
}
FILE* safeReopen(const char* filepath, const char* format, FILE* stream) {
	FILE* fileHandle = 0;
	assert((long)(fileHandle = freopen(filepath, format, stream)), "Не удалось открыть файл\n");
	return fileHandle;
}

#endif // COMMON_FUNCTIONS
#endif // _PRSTUFF_C_
