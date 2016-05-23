#ifndef PRJFILE_H
#define PRJFILE_H

#include <set>
#include <string>

#include "c3files.h"

#ifndef WIN32
#include <stdio.h>
#else
typedef struct _iobuf FILE;
typedef void *HANDLE;
#endif
struct ZFS_FHEADER;
struct ZFS_DTABLE;

#define MAX_PRJFILE_PATHS 16
#define MAX_RECORDNAME_LENGTH 16

enum PRJFILE_PATH_TYPE {
    PRJFILE_PATH_NULL,
    PRJFILE_PATH_DOS,
    PRJFILE_PATH_ZFS,
    PRJFILE_PATH_ZMS
};

struct PFPath
{
    PRJFILE_PATH_TYPE   type;
    FILE *              zfs_fp;
    unsigned char *     zms_start;
    unsigned char *     zms_end;
#ifdef WIN32
    HANDLE              zms_hf;
    HANDLE              zms_hm;
#else
    int                 zms_hf;
    size_t              zms_size;
#endif
    char                dos_path[256];
};

struct PFEntry {
    char rname[MAX_RECORDNAME_LENGTH];
    long offset;
    long size;
    long path;
};

class ProjectFile
{
public:
    ProjectFile();
    ~ProjectFile();

    int addPath(char const * path, bool use_filemapping = false);

    void * getData(char const * rname, size_t & size, C3DIR dir = C3DIR_DIRECT);
    void * getData(char const * rname, size_t & size, HANDLE * hFileMap, size_t & offset);

    void freeData(void *);

    bool exists(char const * fname) const;

    char const * getError() const { return m_error_string; }

    bool    IsReported(char const * a_FileName) const;
    void    MarkReported(char const * a_FileName);

private:
    int mergeEntries(PFEntry *newList, int newCount);
    PFEntry *findRecord(char const * rname) const;

    int readDOSdir(long path, PFEntry *table);
    int verify_ZFS_header(ZFS_FHEADER *header);
    void read_ZFS_dtable(int pathnum, ZFS_DTABLE *dtable,
                         PFEntry **tlp, long *rcount);

    int addPath_DOS(char const * path);
    int addPath_ZFS(char const * path);
    int addPath_ZMS(char const * path);

    void *getData_DOS(PFEntry *entry, size_t & size, C3DIR dir = C3DIR_DIRECT);
    void *getData_ZFS(PFEntry *entry, size_t & size);
    void *getData_ZMS(PFEntry *entry, size_t & size);
    void *getData_ZMS(PFEntry *entry, size_t & size,
                      HANDLE *hFileMap, size_t & offset);

    PFPath m_paths[MAX_PRJFILE_PATHS];
    long m_num_paths;

    PFEntry *m_entries;
    long m_num_entries;

    char m_error_string[256];

    /// List of items that have been reported, to avoid annoying multiple pop-ups.
    std::set<std::string>   m_Reported;

};

#endif
