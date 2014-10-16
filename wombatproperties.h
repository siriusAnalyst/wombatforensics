#ifndef WOMBATPROPERTIES_H
#define WOMBATPROPERTIES_H

#include "wombatinclude.h"
#include "wombatvariable.h"
#include "wombatfunctions.h"

class WombatProperties : public QObject
{
    Q_OBJECT
public:
    WombatProperties(WombatVariable* wombatvarptr);
    QString GetFileSystemLabel(TSK_FS_INFO* curinfo);
    QStringList PopulateEvidenceImageProperties(void);
    QStringList PopulateVolumeProperties(void);
    QStringList PopulatePartitionProperties(void);
    QStringList PopulateFileSystemProperties(TSK_FS_INFO* curfsinfo);
    QStringList PopulateFileProperties(void);
    QString ConvertGmtHours(int gmtvar);
    void yaffscache_objects_stats(YAFFSFS_INFO* yfs, unsigned int* objcnt, uint32_t* objfirst, uint32_t* objlast, uint32_t* vercnt, uint32_t* verfirst, uint32_t* verlast);
    uint32_t hfs_convert_2_unix_time(uint32_t hfsdate);

private:
    WombatVariable* wombatptr;
    QStringList proplist;
    IMG_AFF_INFO* affinfo;
    IMG_EWF_INFO* ewfinfo;
    uint8_t* ewfvalue;
    uint8_t uvalue8bit;
    int8_t value8bit;
    uint32_t value32bit;
    uint64_t value64bit;
    size64_t size64bit;
    libewf_error_t* ewferror;
    FFS_INFO* ffs;
    ffs_sb1* sb1;
    ffs_sb2* sb2;
    mac_part* macpart;
    bsd_disklabel* bsdpart;
    sun_dlabel_sparc* sunsparcpart;
    sun_dlabel_i386* sunx86part;
    gpt_head* gptpart;
    FATFS_INFO* fatfs;
    FATXXFS_SB* fatsb;
    NTFS_INFO* ntfsinfo;
    EXFATFS_MASTER_BOOT_REC* exfatsb;
    HFS_INFO* hfs;
    hfs_plus_vh* hsb;
    ISO_INFO* iso;
    iso9660_pvd_node* p;
    iso9660_svd_node* s;
    YAFFSFS_INFO* yfs;
    EXT2FS_INFO* ext2fs;
    char asc[512];
    char asc128[129];
    char timebuf[128];
};

#endif // WOMBATPROPERTIES_H
