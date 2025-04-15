/*
 *  Copyright 2006-2024 Carnegie Mellon University
 *  See license information in LICENSE.txt.
 */
/**
 *  @file exTool.c
 *  Example export tool which uses Fixbuf IPFIX protocol library
 *  It uses command line parameters:
 *  @param filename - file with template and data to export
 *  @param host - ip address of a collector
 *  @param port - port number collector uses (usually 4739)
 *  
 *  As of now it exports only by using UDP connection
 */
#include <stdio.h>
#include <stdlib.h>
#include <fixbuf/public.h>

#define FATAL(e)                                \
    { fprintf(stderr, "Failed at %s:%d: %s\n",  \
              __FILE__, __LINE__, e->message);  \
        exit(1); }
#define ERROR(fmt, ...)                             \
    { fprintf(stderr, "Failed at %s:%d: " fmt "\n", \
              __FILE__, __LINE__, ##__VA_ARGS__);   \
        exit(1); }

typedef struct {
  fbInfoElementSpec_t *templateArr;
  char                *typeArr; // same size, 's' or 'n' showing type of spec - string or number
  uint16_t            arrSize;
  uint8_t             *actualData;
  uint32_t            dataSize;
  uint16_t            flowCount;
  uint16_t            flowLen;
} loadedFile_t;
/**
 * Constructor for loadedFile_t 
 * !! this->arrSize must be filled 
 * !! this->dataSize must be filled
 * 
 * @param this - pointer to object
 */
void allocLoaded(loadedFile_t *this) {
  uint32_t size = (this->arrSize + 1) * sizeof(fbInfoElementSpec_t);
  // +1 for <end> record with zeroes
  this->templateArr = (fbInfoElementSpec_t*)malloc(size);
  memset(this->templateArr, 0, size);
  this->typeArr = (char*)malloc(this->arrSize);
  memset(this->typeArr, 0, this->arrSize);
  this->actualData = (uint8_t*)malloc(this->dataSize+1);
  memset(this->actualData, 0, this->dataSize+1);
}
/**
 * Destructor for loadedFile_t
 *
 * @param this - pointer to object
 */
void freeLoaded(loadedFile_t *this) {
  uint16_t i;
  for (i = 0; i < this->arrSize; i++) {
    if (this->templateArr[i].name) {
      free(this->templateArr[i].name);
    }
  }
  free(this->templateArr);
  free(this->typeArr);
  free(this->actualData);
}

/**
 *
 * Splits str by delimiter into array of pointers res 
 * >> modifies original str making it show only first element 
 * >> res stores pointers to parts of original string 
 * >> stops if nowhere to store 
 *  
 * @param str [in] string to split
 * @param res [out] array of pointers to splitted parts 
 * @param resSize [in] size of res
 * @param delimiter [in]
 * 
 * @return uint16_t number of split strings
 */
uint16_t splitString(char* str, char** res, uint16_t resSize, char delimiter) {
  uint16_t size = 0;
  char *cur = str;
  if (!cur) return 0; //ignore NULL
  while (*cur == delimiter) cur++; //skip leading delimiters
  if (*cur == 0) return 0; //ignore empty string
  res[size++] = cur; // store first
  cur++; //go to next
  while (*cur) {
    if (*cur == delimiter) {
      *cur = 0; //actual cutting string here
      cur++;
      while (*cur == delimiter) cur++;
      res[size++] = cur; // store next
      if (size == resSize) { return size; }
    }
    cur++;
  }
  return size;
}

/**
 * Constructor of loadedFile_t from file 
 *  
 * Fills template elements spec from the text file and put all 
 * actual data into an array 
 *  
 * @param this [out] - pointer to unconstructed structure  
 * @param filename [in] - file to read data from 
 * 
 * @note user must ensure to free memory by calling freeLoaded
 * 
 */

/* File format:
   <number of elements> <size of data in bytes, <num of flows> * <size of flow> >
   <elem name> <elem size> [s|n]
   0 0 0
   <value of first elem> <value of second elem> <...>
   0 0 <number of 0 equal to number of elems> 
 
   Example:
2 16
destinationIPv4Address 4 n
sourceIPv4Address 4 n
0 0 0
3232248610 3232238593
3232248611 3232238593
0 0
 
*/
void loadFile(loadedFile_t *this, const char* filename) {
// make sure its enough to store one line of data from the file, single record
  #define B_SIZE 10240
// max number of *words* in the line, meaning max number of elements in the template
  #define A_SIZE 128
  #define READ_SPLIT(x) {                                                     \
           tmp = fgets(buffer, B_SIZE, f);                                    \
           if (!tmp) { fclose(f); ERROR("Read file failed"); }                \
           len = splitString(buffer, arr, A_SIZE, ' ');                       \
           if (len != x) { fclose(f); ERROR("expected %u, got %u [%s]", x, len, buffer); } \
         }
    char buffer[B_SIZE];
    char *arr[A_SIZE];
    uint16_t len = 0, i = 0, j;
    char* tmp;
    uint8_t * cur;
    FILE *f;
    memset(this, 0, sizeof(loadedFile_t));
    f = fopen(filename, "r");
    if (!f) return;
    // read sizes and allocate
    READ_SPLIT(2);
    this->arrSize = atoi(arr[0]);
    this->dataSize = atoi(arr[1]);
    allocLoaded(this);
    // read template information
    READ_SPLIT(3);
    while (*arr[2] != '0') {
      // correct spec
      i++;
      if (i > this->arrSize) { ERROR("Invalid file format"); }
      this->templateArr[i - 1].name = (char*)malloc(strlen(arr[0])+1);
      strcpy(this->templateArr[i-1].name, arr[0]);
      this->templateArr[i - 1].flags = 0;
      this->templateArr[i - 1].len_override = atoi(arr[1]);
      this->flowLen += this->templateArr[i - 1].len_override;
      this->typeArr[i - 1] = *arr[2];
      READ_SPLIT(3);
    }
    if (i < this->arrSize) { ERROR("Invalid file format"); }
    // read actual data
    cur = this->actualData;
    this->flowCount = this->dataSize / this->flowLen;
    READ_SPLIT(this->arrSize);
    i = 0;
    while (*arr[0] != '0') {
      if (i >= this->flowCount) { ERROR("Invalid file format %u >= %u arr0 = [%s]", i, this->flowCount, *arr); }
      for (j = 0; j < this->arrSize; j++) {
        if (this->typeArr[j] == 's') {
          memcpy(cur, arr[j], this->templateArr[j].len_override);
          cur += this->templateArr[j].len_override;
          continue;
        }
        switch (this->templateArr[j].len_override) {
        case 8:
          *((uint64_t*)cur) = strtoull(arr[j],&tmp,10);
          break;
        case 4:
          *((uint32_t*)cur) = strtoul(arr[j],&tmp,10);
          break;
        case 2:
          *((uint16_t*)cur) = (uint16_t)strtoul(arr[j],&tmp,10);
          break;
        case 1:
          *cur = (uint8_t)strtoul(arr[j],&tmp,10);
          break;
        default:
          ERROR("Invalid file format");
        }
        cur += this->templateArr[j].len_override;
      }
      READ_SPLIT(this->arrSize);
      i++;
    }
    fclose(f);
  #undef READ_SPLIT
  #undef A_SIZE
  #undef B_SIZE
}

// ignore ip and port and export to file "result" for debug purpose
#define EXP_FILE 0
// future proof for lib version 3.0 which is not released yet
#define VER3 0

void exportLoaded(loadedFile_t *this, char* ip, char* l4port) {
    fbInfoModel_t   *model;
    fbSession_t     *session;
    fbExporter_t    *exporter;
    fbTemplate_t    *tmpl;
    fbTemplate_t    *tmpl_in;
    fBuf_t          *fbuf;
    uint16_t         tid, tid_in;
    int              i;
    GError          *err = NULL;

#if EXP_FILE
    FILE *f;
    f = fopen("result", "w");
#else
    fbConnSpec_t    spec;
    char host[30];
    char port[10];
    memset(&spec, 0, sizeof(spec));
    sprintf(host, "%s", ip);
    sprintf(port, "%s", l4port);
    spec.transport = FB_UDP;
    spec.host = host;
    spec.svc = port;
#endif
    model = fbInfoModelAlloc();
    session = fbSessionAlloc(model);
#if EXP_FILE
    exporter = fbExporterAllocFP(f);
#else
    exporter = fbExporterAllocNet(&spec);
#endif
    fbuf = fBufAllocForExport(session, exporter);
    tmpl = fbTemplateAlloc(model);
    if (!fbTemplateAppendSpecArray(tmpl, this->templateArr, ~0, &err))
        FATAL(err);
#if VER3
    if (!(tid = fbSessionAddTemplatesForExport(
              session, FB_TID_AUTO, tmpl, NULL, &err)))
        FATAL(err);
    if (!fBufSetTemplatesForExport(fbuf, tid, &err))
        FATAL(err);
#else
    if (!(tid = fbSessionAddTemplate(
          session, 1, FB_TID_AUTO, tmpl, &err)))
        FATAL(err);
    if (!fBufSetInternalTemplate(fbuf, tid, &err))
        FATAL(err);
    if (!(tid = fbSessionAddTemplate(
              session, 0, FB_TID_AUTO, tmpl, &err)))
        FATAL(err);
    if (!fBufSetExportTemplate(fbuf, tid, &err))
        FATAL(err);
    if (!fbSessionExportTemplates(session, &err))
        FATAL(err);
#endif
    uint8_t *cur = this->actualData;
    for (i = 0; i < this->flowCount; ++i) {
      if (!fBufAppend(fbuf, cur, this->flowLen, &err))
            FATAL(err);
      cur += this->flowLen;
    }

    if (!fBufEmit(fbuf, &err))
        FATAL(err);
    //  This frees the Buffer, Session, Templates, and Exporter.
    fBufFree(fbuf);
    fbInfoModelFree(model);
#if EXP_FILE
    fclose(f);
#endif
}

int main(int argc, char *argv[])
{
    loadedFile_t obj;

    char filename[30];
    char host[30];
    char port[10];

    if (argc < 4) {
      printf("run as ./exTool <filename> <ip> <port>\r\n example:\r\n./exTool toExport.txt 192.168.51.123 4739\r\n");
      return 0;
    }

    sprintf(filename, "%s", argv[1]);
    sprintf(host, "%s", argv[2]);
    sprintf(port, "%s", argv[3]);

    loadFile(&obj,filename);
    exportLoaded(&obj, host, port);

    return 0;
}
