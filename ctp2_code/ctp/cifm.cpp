#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>

#include "cifm.h"

bool ci_FileExists(const char* name) {
    if(!name) return true;
    if(name[0]=='\0') return true;
    if(strcmp(name,"/")==0) return true;
    
    struct stat info;
    int ret = stat(name, &info);
    return (ret==0);
}

struct ci_name_cache_t{
 char* source;
 char* dest;
};
#define CI_MAX 64
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
static ci_name_cache_t CacheRepo[CI_MAX];
static ci_name_cache_t *Cache[CI_MAX];

void CachePutFirst(int f) {
    if(!f) return;
    ci_name_cache_t *tmp = Cache[f];
    memmove(Cache+1, Cache+0, sizeof(ci_name_cache_t*)*f);
    Cache[0] = tmp;
}

const char* get_name(const char *name) {
    static int inited = 0;
    static char* tmp_c[8];
    static int tmp_i = 0;
    if(!inited) {
        for (int i=0; i<CI_MAX; i++) {
            CacheRepo[i].source = (char*)malloc(MAX_PATH);
            CacheRepo[i].dest =  (char*)malloc(MAX_PATH);
            CacheRepo[i].source[0] ='\0';
            CacheRepo[i].dest[0] ='\0';
            Cache[i] = &CacheRepo[i];
        }
        for (int i=0; i<8; i++) {
            tmp_c[i] = (char*)malloc(MAX_PATH);
            tmp_c[i][0] = '\0';
        }
        inited = 1;
    }

    if(!name) return name;
    if(name[0]=='\0') return name;
    if(strcmp(name,"/")==0) return name;
    if(ci_FileExists(name)) {
        //printf("This one is good \"%s\"\n", name);
        return name;
    }
#define TMP(a) char* p##a = tmp_c[tmp_i]; tmp_i=(tmp_i+1)&7
    if(strchr(name, '\\')) {
        TMP(1);
        strcpy(p1, name);
        char* p = p1;
        while((p=strchr(p, '\\'))) *p='/';
        return get_name(p1);
    }
    if(name[strlen(name)-1]=='/') {
        TMP(1);
        strcpy(p1, name);
        p1[strlen(name)-1] = '\0';
        TMP(2);
        strcpy(p2, get_name(p1));
        strcat(p2, "/");
        return p2;
    }

    //printf("Try to fix \"%s\"\n", name);

    // search in repo...
    for (int i=0; i<CI_MAX; i++)
        if(strcasecmp(name, Cache[i]->source)==0)
            if(ci_FileExists(Cache[i]->dest)) {
                //test if ok first, then return the value
                CachePutFirst(i);
                return Cache[0]->dest;
            }

    // split name / folder
    char *p = (char*)strrchr(name, '/');
    char *r = (p)?strndup(name, p-name):NULL;
    char *n = (p)?strdup(p+1):strdup(name);
    
    const char *new_r = get_name(r);
    
    if(!ci_FileExists(new_r)) {
        // fail to find the right path...
        free(r);
        free(n);
        return name;
    }
    // try to find a match for name now
	DIR *d;
    struct dirent *dir;
    TMP(1);
    d = opendir(new_r?new_r:".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
            if (strcasecmp(dir->d_name, n)==0)
            {
                strcpy(p1, new_r?new_r:"");
                strcat(p1, new_r?"/":"");
                strcat(p1, dir->d_name);
                if(ci_FileExists(p1)) {
                    CachePutFirst(CI_MAX-1);
                    strcpy(Cache[0]->source, name);
                    strcpy(Cache[0]->dest, p1);
                    free(r);
                    free(n);
                    closedir(d);
                    return Cache[0]->dest;
                }
            }
		}
		closedir(d);
    }
    // build something...
    strcpy(p1, new_r?new_r:"");
    strcat(p1, new_r?"/":"");
    strcat(p1, n);
// fail
    free(r);
    free(n);

    return p1;
}


FILE* ci_fopen(const char* name, const char* mode)
{
    FILE *ret = fopen(name, mode);
    if(ret) {
        return ret;
    }
    const char* fixedname = get_name(name);
    ret = fopen(fixedname, mode);
    return ret;
}

const char* CI_FixName(const char* name)
{
    const char* fixedname = get_name(name);
    
    return fixedname;
}

int CI_FileExists(const char* name)
{
    if (ci_FileExists(name))
        return 1;
    return 0;
}
    