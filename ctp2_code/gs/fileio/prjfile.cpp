#include "c3.h"

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>

#include "prjfile.h"
#include "CivPaths.h"

extern CivPaths *g_civPaths;

#ifndef WIN32
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define MAX_ENTRIES_PER_TABLE 100
#define ZFSFLAG_DELETED 0x0001

struct ZFS_RENTRY {
    char      rname[MAX_RECORDNAME_LENGTH];
    long      offset;
    long      rnum;
    long      size;
    long      time;
    long      flags;
};

struct ZFS_DTABLE {
    long          next_dtable;
    ZFS_RENTRY    rentry[MAX_ENTRIES_PER_TABLE];
};

struct ZFS_FHEADER {
    char      filetag[4];
    long      version;
    long      max_recordname_length;
    long      max_entries_per_table;
    long      num_rentries;
    long      encrypt_key;
    long      dtable_head;
};

char *strcasecpy(char *out, char *in)
{
    char *maxout = out + MAX_RECORDNAME_LENGTH - 1;
    char *rval = out;
    while(*in && (out < maxout)) {
        *out++ = tolower(*in++);
    }
    *out = 0;
    return(rval);
}

int PFEntry_compare(const void *a, const void *b)
{
    return(stricmp(((PFEntry *) a)->rname, ((PFEntry *) b)->rname));
}

void *mapFile(char *path, size_t *size, PFPath &pfp)
{
    void *ptr;

#ifdef WIN32
    pfp.zms_hf = CreateFile(path,
                          GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);
    if (pfp.zms_hf == INVALID_HANDLE_VALUE) {
#else
    struct stat tmpstat = { 0 };
    if (stat(path, &tmpstat) != 0) {
    	return NULL;
    }
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    pfp.zms_size = tmpstat.st_size;
    *size = tmpstat.st_size;
#endif
#ifdef WIN32
    *size = GetFileSize(*fhandle, NULL);

    pfp.zms_hm = CreateFileMapping(*fhandle,
                                 NULL,
                                 PAGE_READONLY,
                                 0,
                                 0,
                                 NULL);
    if (pfp.zms_hm == INVALID_HANDLE_VALUE) {
        CloseHandle(*fhandle);
        return(NULL);
    }

    ptr = MapViewOfFile(pfp.zms_hm, FILE_MAP_READ, 0, 0, 0);
    if (ptr == NULL) {
        CloseHandle(pfp.zms_hm);
        CloseHandle(pfp.zms_hf);
        return(NULL);
    }
#else
    ptr = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!ptr) {
    	close(fd);
        return(NULL);
    }
    pfp.zms_hf = fd;
#endif

    return(ptr);
}

void unmapFile(void *ptr, PFPath &pfp)
{
#ifdef WIN32
    UnmapViewOfFile(ptr);
    CloseHandle(pfp.zms_hm);
    CloseHandle(pfp.zms_hf);
#else
    munmap(ptr, pfp.zms_size);
    close(pfp.zms_hf);
#endif
}

ProjectFile::ProjectFile()
{
    m_error_string[0] = 0;

    memset(m_paths, 0, sizeof(PFPath) * MAX_PRJFILE_PATHS);
    m_num_paths = 0;

    m_entries = NULL;
    m_num_entries = 0;
}

ProjectFile::~ProjectFile()
{
    int i;

	m_num_paths = 0;

    if (m_entries) {
        free(m_entries);
        m_entries = NULL;
        m_num_entries = 0;
    }

    for(i=0; i<MAX_PRJFILE_PATHS; i++) {
        switch(m_paths[i].type) {
          case PRJFILE_PATH_NULL:
          case PRJFILE_PATH_DOS: {
              break;
          }
          case PRJFILE_PATH_ZFS: {
              fclose(m_paths[i].zfs_fp);
              break;
          }
          case PRJFILE_PATH_ZMS: {
              unmapFile(m_paths[i].zms_start, m_paths[i]);
              break;
          }
        }
    }
}

int ProjectFile::mergeEntries(PFEntry *newList, int newCount)
{

    if (m_entries) {
        m_entries =
            (PFEntry *)realloc(m_entries, (sizeof(PFEntry) *
                                           (newCount + m_num_entries)));
    } else {
        m_entries = (PFEntry *)malloc(sizeof(PFEntry) * newCount);
        m_num_entries = 0;
    }
    if (m_entries == NULL) {
        sprintf(m_error_string, "Not enough memory");
        return(0);
    }

    memcpy(m_entries + m_num_entries, newList, sizeof(PFEntry) * newCount);
    m_num_entries += newCount;

    qsort(m_entries, m_num_entries, sizeof(PFEntry), PFEntry_compare);

    return(1);
}

PFEntry *ProjectFile::findRecord(char *rname)
{
    PFEntry key;

    strcasecpy(key.rname, rname);

    return((PFEntry *)bsearch(&key, m_entries, m_num_entries,
                              sizeof(PFEntry), PFEntry_compare));
}

int ProjectFile::exists(char *rname)
{
    return(findRecord(rname) != NULL);
}

void *ProjectFile::getData_DOS(PFEntry *entry, size_t *size, C3DIR dir)
{
    FILE *fp;
    char *data;
    char tempstr[256];

	if (dir != C3DIR_DIRECT) {
		if (!g_civPaths->FindFile(dir, entry->rname, tempstr)) {
			sprintf(tempstr, "%s%s%s", m_paths[entry->path].dos_path, FILE_SEP, entry->rname);
		}
	} else {
		sprintf(tempstr, "%s%s%s", m_paths[entry->path].dos_path, FILE_SEP, entry->rname);
    }

	fp = fopen(tempstr, "rb");

    if (fp == NULL) {
        sprintf(m_error_string, "Couldn't open file \"%s\"", tempstr);
        return(NULL);
    }
    setvbuf(fp, NULL, _IONBF, 0);

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    data = (char *) malloc(*size);
    if (data == NULL) {
        sprintf(m_error_string, "Could not malloc data for record %s\n", tempstr);
        return(NULL);
    }

    fseek(fp, 0, SEEK_SET);
    if (fread(data, *size, 1, fp) < 1) {
        sprintf(m_error_string, "Could not read file \"%s\"", tempstr);
        free(data);
        fclose(fp);
        return(NULL);
    }

    fclose(fp);
    return(data);
}

void *ProjectFile::getData_ZFS(PFEntry *entry, size_t *size)
{
    FILE *fp = m_paths[entry->path].zfs_fp;
    char *data;

    if (fp == NULL) {
        return(NULL);
    }

    *size = entry->size;
    data = (char *) malloc(*size);
    if (data == NULL) {
        sprintf(m_error_string, "Could not malloc data for record %s\n",
                entry->rname);
        return(NULL);
    }

    fseek(fp, entry->offset, SEEK_SET);
    if (fread(data, *size, 1, fp) < 1) {
        sprintf(m_error_string, "Could not read record \"%s\" from \"%s\"" ,
                entry->rname, m_paths[entry->path].dos_path);
        free(data);
        return(NULL);
    }

    return(data);
}

void *ProjectFile::getData_ZMS(PFEntry *entry, size_t *size)
{
    char *data;

    *size = entry->size;
    data = (char *)(m_paths[entry->path].zms_start) + entry->offset;

    return(data);
}

void *ProjectFile::getData_ZMS(PFEntry *entry, size_t *size,
                               HANDLE *hFileMap, long *offset)
{
    char *data;

    *size = entry->size;
    data = (char *)(m_paths[entry->path].zms_start) + entry->offset;
#ifdef WIN32
    *hFileMap = m_paths[entry->path].zms_hm;
#endif
    *offset = entry->offset;

    return(data);
}

void *ProjectFile::getData(char *rname, size_t *size, C3DIR dir)
{
    PFEntry *entry = findRecord(rname);

    if (entry == NULL) {
        sprintf(m_error_string, "Couldn't find record \"%s\"", rname);
        *size = 0;
        return(NULL);
    }

    switch(m_paths[entry->path].type) {
      case PRJFILE_PATH_DOS: return(getData_DOS(entry, size, dir));
      case PRJFILE_PATH_ZFS: return(getData_ZFS(entry, size));
      case PRJFILE_PATH_ZMS: return(getData_ZMS(entry, size));
      case PRJFILE_PATH_NULL: return NULL;
    }

    return(NULL);
}

void *ProjectFile::getData(char *rname, size_t *size,
                           HANDLE *hFileMap, long *offset)
{
    PFEntry *entry = findRecord(rname);

    if (entry == NULL) {
        sprintf(m_error_string, "Couldn't find record \"%s\"", rname);
        *size = 0;
        *hFileMap = NULL;
        *offset = 0;
        return(NULL);
    }

    if (m_paths[entry->path].type != PRJFILE_PATH_ZMS) {
        sprintf(m_error_string, "Record \"%s\" is not file mapped", rname);
        *size = 0;
        *hFileMap = NULL;
        *offset = 0;
        return(NULL);
    }

    return(getData_ZMS(entry, size, hFileMap, offset));
}

void ProjectFile::freeData(void *ptr)
{
    int i;
    for(i=0; i<m_num_paths; i++) {
        if ((m_paths[i].type == PRJFILE_PATH_ZMS) &&
            (ptr >= m_paths[i].zms_start) &&
            (ptr < m_paths[i].zms_end)) {
            return;
        }
    }

    if (ptr)
        free(ptr);
}

int ProjectFile::readDOSdir(long path, PFEntry *table)
{
    char tmp[256];
#ifdef WIN32
    WIN32_FIND_DATA dirent;
    HANDLE dirhandle;
#else
    DIR *dir;
    struct dirent *dent = 0;
    struct stat tmpstat;
#endif
    int count=0;

    sprintf(tmp, "%s%s*.*", m_paths[path].dos_path, FILE_SEP);
#ifdef WIN32
    dirhandle = FindFirstFile(tmp, &dirent);
    if (dirhandle == INVALID_HANDLE_VALUE) {
#else
    dir = opendir(m_paths[path].dos_path);
    if (!dir) {
        sprintf(m_error_string, "Couldn't find \"%s\"", tmp);
        return(0);
    }
#endif

    do {
#ifndef WIN32
        dent = readdir(dir);
        if (!dent)
            continue;

        MBCHAR *name = dent->d_name;
#else
        MBCHAR *name = dirent.cFileName;
#endif

        if (!stricmp(".", name))
            continue;

        if (!stricmp("..", name))
            continue;

#ifndef WIN32
        snprintf(tmp, sizeof(tmp), "%s%s%s", m_paths[path].dos_path, FILE_SEP, name);
        if (!stat(tmp, &tmpstat))
            continue;

        if (!S_ISDIR(tmpstat.st_mode)) {
#else
        if (!(dirent.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
#endif
            if ((table) && (!exists(name))) {
                strcasecpy(table->rname, name);
                table->offset = 0;
                table->size = 0;
                table->path = path;
                table++;
                count++;
            } else if (!table) {
                count++;
            }
        }
#ifndef WIN32
    } while (dent);
    closedir(dir);
#else
    } while (FindNextFile(dirhandle, &dirent) == TRUE);
    FindClose(dirhandle);
#endif
    return(count);
}

int ProjectFile::addPath_DOS(char *path)
{
    int count;
    int pathnum = m_num_paths;

    m_paths[pathnum].type = PRJFILE_PATH_DOS;
    strcpy(m_paths[pathnum].dos_path, path);
    m_num_paths++;

    count = readDOSdir(pathnum, NULL);

    if (count == 0) {
        m_num_paths--;
        return(1);
    }

    PFEntry *tmpList = (PFEntry *)malloc(sizeof(PFEntry) * count);
    if (tmpList == NULL) {
        sprintf(m_error_string, "Couldn't add \"%s\", not enough memory", path);
        return(0);
    }

    count = readDOSdir(pathnum, tmpList);

    mergeEntries(tmpList, count);

    free(tmpList);

    return(1);
}

int ProjectFile::verify_ZFS_header(ZFS_FHEADER *header)
{
	if ((header->filetag[0] != 'Z') ||
		(header->filetag[1] != 'F') ||
		(header->filetag[2] != 'S') ||
		(header->filetag[3] != '3') ||
		(header->version != 1) ||
		(header->max_recordname_length != MAX_RECORDNAME_LENGTH) ||
		(header->max_entries_per_table != MAX_ENTRIES_PER_TABLE)) {
        return(0);
	}

    return(1);
}

void ProjectFile::read_ZFS_dtable(int pathnum, ZFS_DTABLE *dtable,
                                 PFEntry **tlp, long *rcount)
{
    int i;

    for (i=0;
         (i<MAX_ENTRIES_PER_TABLE) && (dtable->rentry[i].rname[0]);
         i++) {

        if (!(dtable->rentry[i].flags & ZFSFLAG_DELETED)) {
            ZFS_RENTRY *rentry = dtable->rentry + i;
            if (!exists(rentry->rname)) {
                strcasecpy((*tlp)->rname, rentry->rname);
                (*tlp)->offset = rentry->offset;
                (*tlp)->size = rentry->size;
                (*tlp)->path = pathnum;
                (*tlp)++;
                (*rcount)++;
            }
        }
    }
}

int ProjectFile::addPath_ZFS(char *path)
{
    FILE *fp;
    int count;
    int pathnum = m_num_paths;
    ZFS_FHEADER header;
    ZFS_DTABLE dtable;

    fp = fopen(path, "rb");
	if (fp == NULL) {
		sprintf(m_error_string, "Could not open file \"%s\"", path);
		return(0);
	}
    setvbuf(fp, NULL, _IONBF, 0);

    m_paths[pathnum].type = PRJFILE_PATH_ZFS;
    strcpy(m_paths[pathnum].dos_path, path);
    m_num_paths++;

    m_paths[pathnum].zfs_fp = fp;

 	fseek(fp, 0, SEEK_SET);

	if (fread(&header, sizeof(ZFS_FHEADER), 1, fp) < 1) {
		sprintf(m_error_string, "Could not read header of file \"%s\"", path);
		return(0);
	}

    if (!verify_ZFS_header(&header)) {
		sprintf(m_error_string, "Invalid header in file \"%s\"", path);
		return(0);
	}

    count = header.num_rentries;
    PFEntry *tmpList = (PFEntry *)malloc(sizeof(PFEntry) * count);
    if (tmpList == NULL) {
        sprintf(m_error_string, "Couldn't add \"%s\", not enough memory", path);
        return(0);
    }
    PFEntry *tlp = tmpList;

    long dhead = header.dtable_head;
    long rcount = 0;
    do {
        fseek(fp, dhead, SEEK_SET);

        if (fread(&dtable, sizeof(ZFS_DTABLE), 1, fp) < 1) {
            sprintf(m_error_string, "File \"%s\" is corrupt", path);
            return(0);
        }

        read_ZFS_dtable(pathnum, &dtable, &tlp, &rcount);

    } while ((dhead = dtable.next_dtable) != 0);

    mergeEntries(tmpList, rcount);

    free(tmpList);

    return(1);
}

int ProjectFile::addPath_ZMS(char *path)
{
    void *fbase;
    size_t fsize;
    int count;
    int pathnum = m_num_paths;
    ZFS_FHEADER *header;
    ZFS_DTABLE *dtable;

    fbase = mapFile(path, &fsize, m_paths[pathnum]);
    if (fbase == NULL) {
        sprintf(m_error_string, "Could not open file \"%s\"", path);
        return(0);
    }

    m_paths[pathnum].type = PRJFILE_PATH_ZMS;
    strcpy(m_paths[pathnum].dos_path, path);
    m_num_paths++;

    m_paths[pathnum].zms_start = fbase;
    m_paths[pathnum].zms_end = (char *)fbase + fsize;

    header = (ZFS_FHEADER *)fbase;
    if (!verify_ZFS_header(header)) {
		sprintf(m_error_string, "Invalid header in file \"%s\"", path);
		return(0);
	}

    count = header->num_rentries;
    PFEntry *tmpList = (PFEntry *)malloc(sizeof(PFEntry) * count);
    if (tmpList == NULL) {
        sprintf(m_error_string, "Couldn't add \"%s\", not enough memory", path);
        return(0);
    }
    PFEntry *tlp = tmpList;

    long dhead = header->dtable_head;
    long rcount = 0;
    do {
        dtable = (ZFS_DTABLE *)((char *)fbase + dhead);

        read_ZFS_dtable(pathnum, dtable, &tlp, &rcount);

    } while ((dhead = dtable->next_dtable) != 0);

    mergeEntries(tmpList, rcount);

    free(tmpList);

    return(1);
}

int ProjectFile::addPath(char *path, int use_filemapping)
{

    if (m_num_paths >= MAX_PRJFILE_PATHS) {
        sprintf(m_error_string, "Couldn't add \"%s\", too many paths", path);
        return(0);
    }

    char *ext = strrchr(path, '.');

    if (ext == NULL) {
        return(addPath_DOS(path));
    }
    ext++;

    if ((toupper(ext[0]) == 'Z') &&
        (toupper(ext[1]) == 'F') &&
        (toupper(ext[2]) == 'S') &&
        (toupper(ext[3]) == 0)) {
        if (use_filemapping) {
            return(addPath_ZMS(path));
        } else {
            return(addPath_ZFS(path));
        }
    }

    return(addPath_DOS(path));
}
