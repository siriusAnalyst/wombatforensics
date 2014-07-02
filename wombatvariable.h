#ifndef WOMBATVARIABLE_H
#define WOMBATVARIABLE_H

#include "wombatinclude.h"

struct CaseObject
{
    int id;
    QString name;
    QString dbname;
    QString dirpath;
};

struct EvidenceObject
{
    int id;
    int type;
    int objecttype;
    int itemcount;
    QString name;
    int size;
    int sectsize;
    int parentid;
    int parimgid;
    QStringList namelist;
    QString fullpath; 
    std::vector<std::string> fullpathvector;
    TSK_IMG_INFO* imageinfo; // These may disappear when the image is closed, will have to test this concept
    TSK_VS_INFO* volinfo;
    std::vector<const TSK_VS_PART_INFO*> partinfovector;
    std::vector<TSK_FS_INFO*> fsinfovector;
    std::vector<int> fsidvector;
    QString dbname;
    void Clear()
    {
        fullpath = "";
        name = "";
        id = 0;
        type = 0;
        itemcount = 0;
        size = 0;
        sectsize = 0;
        namelist.clear();
        parentid = 0;
        fullpathvector.clear();
        imageinfo = NULL;
        volinfo = NULL;
        partinfovector.clear();
        fsinfovector.clear();
        dbname = "";
    };
};

struct FileExportData
{
    enum PathStatus
    {
        include = 0,
        exclude = 1
    };

    enum FileStatus
    {
        selected = 0,
        checked = 1,
        listed = 2
    };

    std::string exportpath;
    int id;
    FileStatus filestatus;
    PathStatus pathstatus;
    std::string name;
    std::string fullpath;
    EvidenceObject evidenceobject;
    int exportcount;
};

struct SelectedObject
{
    int id;
    int objtype;
    QString name;
    int size;
    int parimgid;
    int sectstart;
    int sectlength;
    int blocksize;
    int blockcount;
    int byteoffset;
    int address;
    int type;
};

struct WombatVariable
{

    // ADD ENUMS FOR JOBSTATUS, JOBTYPE, ETC TO SIMPLIFY INTEGER ASSOCIATION AND READABILITY IN CODE
    enum JobStatus
    {
        failed = 0,
        finish = 1,
        finisherror = 2,
        cancelled = 3
    };

    enum JobType
    {
        generic = 0,
        addevidence = 1,
        remedvidence = 2,
        exportfiles = 3
    };

    // id values
    int jobtype;
    int jobid;
    int fileid;
    int volid;
    int filecount;
    int processcount;
    int omnivalue;
    int visibleviewer;
    int currentevidenceid;
    int currentvolumeid;
    int currentpartitionid;
    int currentfilesystemid;
    int currentrootinum;
    QString currentevidencename;
    QString currentvolumename;
    QString casespath;
    QString settingspath;
    QString datapath;
    //QString tmpfilepath;
    QString curerrmsg;
    QString wombatdbname;
    QString evidremovestring;
    int evidremoveid;
    int evidrowsremoved;
    CaseObject caseobject;
    QStringList casenamelist;
    QStringList evidencenamelist;
    EvidenceObject evidenceobject;
    QVector<EvidenceObject> evidenceobjectvector;
    SelectedObject selectedobject;
    FileExportData exportdata;
    QVector<FileExportData> exportdatavector;
    QString htmlcontent;
    QSqlDatabase casedb;
    QSqlDatabase appdb;
    QList<QSqlRecord> sqlrecords;
    QList<QVariant> bindvalues;
};

Q_DECLARE_METATYPE(WombatVariable)
Q_DECLARE_METATYPE(FileExportData)
Q_DECLARE_METATYPE(CaseObject)
Q_DECLARE_METATYPE(EvidenceObject)
Q_DECLARE_METATYPE(SelectedObject);

#endif // WOMBATVARIABLE_H