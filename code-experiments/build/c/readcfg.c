#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINESIZE 1024
#define MAXKEYNUMBER 100
char *suite_name;
char *functions_id;
char *instances_id;
char *dimensions;
char *observer_name;
char *result_folder;
char *algorithm_info;
char *algorithm_name;
char *parameters_name;
char *complete_triggers;
int number_evaluation_triggers;
char *base_evaluation_triggers;
int number_target_triggers;
int number_interval_triggers;
/**
  A struct for dic.
  
*/
typedef struct Dict{
  int n;
  size_t size;
  char ** section;
  char ** value;
  char ** key;
}Dict;

typedef enum _LINE_{
  EMPTY,
  COMMENT,
  SECTION,
  VALUE,
  ERROR
} linecontent ;
/* craete a Dict*/

static char * xstrdup(const char * s);
static unsigned strstrip(char * s);
Dict * new_Dict(size_t size);
int set_Dict(Dict * dict, const char * section, const char * key, const char * value);
void del_Dict(Dict * dict);
static linecontent add_Line(const char * input_line, char * section, char * key, char * value);
Dict * load(const char * filename);
const char * get_Dict_String(const Dict *dict, const char * section, const char * key);
const int get_Dict_Int(const Dict *dict, const char * section, const char * key);

static char * xstrdup(const char * s){
  char * t;
  size_t  len;
  if(!s) 
    return NULL;

  len = strlen(s) + 1;
  t = (char *) malloc (len);
  if(!t){
    printf("malloc memory error when duplicate string (char *)\n");
    exit(1);
  }
  else
    memcpy(t,s,len);
  return t;
}

static unsigned strstrip(char * s)
{
    char *last = NULL ;
    char *dest = s;

    if (s==NULL) return 0;

    last = s + strlen(s);
    while (isspace((int)*s) && *s) s++;
    while (last > s) {
        if (!isspace((int)*(last-1)))
            break ;
        last -- ;
    }
    *last = (char)0;

    memmove(dest,s,last - s + 1);
    return last - s;
}

Dict * new_Dict(size_t size){
  Dict * dict; 
  dict = (Dict*) calloc(1, sizeof *dict) ;

  if(dict){
    dict->size = size;
    dict->section = (char**) calloc(size, sizeof *dict->section);
    dict->value = (char**) calloc(size, sizeof *dict->value);
    dict->key  = (char**) calloc(size, sizeof *dict->key);
  }
  dict->n = 0;
  return dict;
}

int set_Dict(Dict * dict, const char * section, const char * key, const char * value){
  size_t i ;

  if (dict == NULL) 
    return -1 ;

  if (dict->n > 0){
    for(i = 0; i < dict->n; ++i){
      if(key == NULL) continue;
      if(value == NULL) continue;
      if(dict->key[i] == NULL) continue;
      if(dict->section[i] == NULL) continue;
      if(!strcmp(key, dict->key[i]) && !strcmp(section,dict->section[i])){
        if(dict->value[i] != NULL){
          printf("Muti setting of key(%s) or section(%s)\n",key,section);
          return -1;
        }
      }
    }
  }
  dict->section[dict->n] = xstrdup(section);
  dict->key[dict->n]  = xstrdup(key);
  dict->value[dict->n]  = xstrdup(value) ;
  dict->n = dict->n + 1;
  return 0 ;
}

void del_Dict(Dict * dict){
  size_t i;

  if(dict == NULL) return ;
  for(i = 0; i < dict->size; ++i){
    if(dict->section[i] != NULL)
      free(dict->section[i]);
    if(dict->key[i] != NULL)
      free(dict->key[i]);
    if(dict->value[i] != NULL)
      free(dict->value[i]);
  }
  
  free(dict->value);
  free(dict->key);
  free(dict->section);
  free(dict);
  return ;
}

const char * get_Dict_String(const Dict *dict, const char * section, const char * key){
  size_t i;
  if(key == NULL){
    printf("EMPTY KEY INPUT.\n");
    exit(1);
  }
  if(section == NULL){
    printf("EMPTY SECTION INPUT.\n");
    exit(1);
  }
  for(i = 0; i < dict->n; ++i){
    if(dict->key[i] == NULL) continue;
    if(dict->section[i] == NULL) continue;
    if(!strcmp(key, dict->key[i]) && !strcmp(section,dict->section[i]))
      return dict->value[i];
  }
  printf("Can not find the corresponding configuration for key: %s in section : %s\n",key,section);
  exit(1);
  return NULL;
}

const int get_Dict_Int(const Dict *dict, const char * section, const char * key){
  int result;
  char * str;
  str = get_Dict_String(dict, section, key);
  result = atoi(str);
  return result;
}

static linecontent add_Line(const char * input_line, char * section, char * key, char * value){
  linecontent content;
  char * line  = NULL;
  size_t len;

  line = xstrdup(input_line);
  len = strlen(line);

  if(len < 0)
    content = EMPTY;
  else if(line[0] == '#' || line[0] == ';')
    content = COMMENT;
  else if(line[0] == '['  && line[len-1] == ']'){
    sscanf(line, "[%[^]]",section);
    content = SECTION;
  }
  else if(sscanf (line, "%[^=] = \"%[^\"]", key, value) == 2 || sscanf (line, "%[^=] = '%[^\']", key, value) == 2){
    content = VALUE;
  }
  else if(sscanf (line, "%[^=] = %[^;#]", key, value) == 2){
    content = VALUE;
  }
  else{
   content = ERROR;
  }

  free(line);
  return content;
}

Dict * load(const char * filename){
  FILE * fp;

  char line[MAXLINESIZE];
  char section[MAXLINESIZE];
  char key[MAXLINESIZE];
  char value[MAXLINESIZE];

  size_t len;
  Dict * dict;
  linecontent lc;

  fp = fopen(filename,"r");
  if(fp == NULL){
    printf("Cannot open file %s\n",filename);
    exit(1);
  }

  dict = new_Dict(MAXKEYNUMBER);
  if(dict == NULL){
    fclose(fp);
    return NULL;
  }

  memset(line,0,MAXLINESIZE);
  memset(section,0,MAXLINESIZE);
  memset(key,0,MAXLINESIZE);
  memset(value,0,MAXLINESIZE);

  while(fgets(line,MAXLINESIZE,fp) != NULL){
    len = strlen(line) -1;
    if(len <= 0)
        continue;

    if(line[len] != '\n' && !feof(fp)){
      printf("the line is too long\n");
      return NULL;
    }

    while(len >= 0 && (line[len] == '\n' || line[len] == ' ')){
      line[len] = 0;
      --len;
    }
    
    if(len < 0)
      len = 0;

    lc = add_Line(line,section,key,value);
    switch(lc){
      case EMPTY:
        break;
      case COMMENT:
        break;
      case SECTION:
        strstrip(section);
        set_Dict(dict,section,NULL,NULL);
        break;
      case VALUE:
        strstrip(key);
        set_Dict(dict,section,key,value);
        break;
      case ERROR:
        printf("There is an error for line: \"%s\"",line);
        break;
    }
    if(lc == ERROR){
      del_Dict(dict);
      dict = NULL;
      break;
    }
  }
  fclose(fp);
  return dict;
  
}

void readcfg(char * filename){

  Dict *dict;
  dict = load(filename);
  suite_name = get_Dict_String(dict,"suite","suite_name");
  functions_id = get_Dict_String(dict,"suite","functions_id");
  instances_id = get_Dict_String(dict,"suite","instances_id");
  dimensions = get_Dict_String(dict,"suite","dimensions");
  
  
  observer_name = get_Dict_String(dict,"observer","observer_name");
  result_folder = get_Dict_String(dict,"observer","result_folder");
  algorithm_info = get_Dict_String(dict,"observer","algorithm_info");
  algorithm_name = get_Dict_String(dict,"observer","algorithm_name");
  parameters_name = get_Dict_String(dict,"observer","parameters_name");

  
  complete_triggers = get_Dict_String(dict,"triggers","complete_triggers");
  base_evaluation_triggers = get_Dict_String(dict,"triggers","base_evaluation_triggers");
  number_target_triggers = get_Dict_Int(dict,"triggers","number_target_triggers");
  number_interval_triggers = get_Dict_Int(dict,"triggers","number_interval_triggers");

}