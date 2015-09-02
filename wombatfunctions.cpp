#include "wombatfunctions.h"

std::string GetTime()
{
    struct tm *newtime;
    time_t aclock;

    time(&aclock);   // Get time in seconds
    newtime = localtime(&aclock);   // Convert time to struct tm form 
    char timeStr[64];
    snprintf(timeStr, 64, "%.2d/%.2d/%.2d %.2d:%.2d:%.2d",
        newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year % 100, 
        newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

    return timeStr;
}

void LogMessage(QString logmsg)
{
    QString tmpstring = QDateTime::currentDateTime().toString(QString("MM/dd/yyyy hh:mm:ss t"));
    msglog->append(QString(tmpstring + " " + logmsg));
    logfile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    logfile.write(QString(tmpstring + "\t" + logmsg + "\n").toStdString().c_str());
    logfile.close();
}

char* TskTimeToStringUTC(time_t time, char buf[128])
{
    buf[0] = '\0';
    if (time <= 0) {
        strncpy(buf, "", 128);
    }
    else {
        //struct tm *tmTime = localtime(&time);
        struct tm *tmTime = gmtime(&time);

        snprintf(buf, 128, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", (int) tmTime->tm_year + 1900, (int) tmTime->tm_mon + 1, (int) tmTime->tm_mday, tmTime->tm_hour, (int) tmTime->tm_min, (int) tmTime->tm_sec);//, tzname[(tmTime->tm_isdst == 0) ? 0 : 1]);
    }
    return buf;
}

unsigned long long GetChildCount(int type, unsigned long long address, unsigned long long parimgid)
{
    unsigned long long tmpcount = 0;
    QSqlQuery childquery(fcasedb);
    QString querystring = "SELECT COUNT(objectid) FROM data WHERE parentid = ?";
    if(type < 4)
        querystring += " AND objecttype < 5";
    else
        querystring += " AND objecttype = 5";
    if(type != 1)
        querystring += " AND parimgid = ?";
    childquery.prepare(querystring);
    childquery.addBindValue(address);
    if(type != 1)
        childquery.addBindValue(parimgid);
    if(childquery.exec())
    {
        childquery.next();
        tmpcount = childquery.value(0).toULongLong();
    }
    childquery.finish();
    return tmpcount;
}

bool FileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

bool ProcessingComplete()
{
    if(filesfound == filesprocessed && filesfound > 0)
        return true;
    
    return false;
}

void ProcessFile(QVector<QString> tmpstrings, QVector<unsigned long long> tmpints)
{
    FileData tmpdata;
    tmpdata.type = tmpints[0];
    tmpdata.name = tmpstrings[0];
    tmpdata.paraddr = tmpints[1];
    tmpdata.path = tmpstrings[1];
    tmpdata.atime = tmpints[2];
    tmpdata.ctime = tmpints[3];
    tmpdata.crtime = tmpints[4];
    tmpdata.mtime = tmpints[5];
    tmpdata.size = tmpints[6];
    tmpdata.addr = tmpints[7];
    tmpdata.evid = currentevidenceid;
    tmpdata.fsid = tmpints[8];
    mutex.lock();
    filedatavector.append(tmpdata);
    mutex.unlock();
        /*
    }
    else
    {
        //LogEntry(0, currentevidenceid, currentjobid, 0, QString("Error while processing " + tmpstrings[1] + " " + fcasedb.lastError().text()));
        LogMessage(QString("Error while processing " + tmpstrings[1] + " " + fcasedb.lastError().text()));
        filesprocessed++;
        errorcount++;
        //isignals->ProgUpd();
    }*/
}

TSK_WALK_RET_ENUM GetBlockAddress(TSK_FS_FILE* tmpfile, TSK_OFF_T off, TSK_DADDR_T addr, char* buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr)
{
    if(off < 0)
    {
        // remove compile warning
    }
    if(buf)
    {
        // remove compile warning
    }
    if(ptr)
    {
        // remove compile warning
    }
    //qDebug() << tmpfile->name->name << addr;

    // WILL HAVE TO CREATE A SWITCH TO ACCOUNT FOR THE DIFFERENT FILE SYSTEMS
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT)
    {
        blockstring += QString::number(addr) + "|";
        // NEED TO FIGURE OUT HOW TO GET EACH BLOCK SO I CAN STORE THE RESPECTIVE VALUES
        //qDebug() << "File Name:" << tmpfile->name->name << "Block Address:" << addr;
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT)
    {
        // iso is already done in the previous function
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        //qDebug() << tmpfile->name->name << addr;
        blockstring += QString::number(addr) + "|";
        //if(flags & TSK_FS_BLOCK_FLAG_RES)
        //    qDebug() << "resident file, not sure what it yields:" << addr;
        //qDebug() << "File Name:" << tmpfile->name->name << "Address:" << addr;
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_YAFFS2_DETECT)
    {
        if(flags & TSK_FS_BLOCK_FLAG_CONT)
        {
            blockstring += QString::number(addr) + "|";
        }
    }
    else if(tmpfile->name != NULL)
    {
        if((strcmp(tmpfile->name->name, "$FAT1") == 0) || (strcmp(tmpfile->name->name, "$FAT2") == 0) || (strcmp(tmpfile->name->name, "$MBR") == 0) || (strcmp(tmpfile->name->name, "$OprhanFiles") == 0))
        {
            //qDebug() << tmpfile->name->name << addr;
            blockstring += QString::number(addr) + "|";
        }
    }
    else
    {
        if(flags & TSK_FS_BLOCK_FLAG_CONT)
        {
            int i, s;
            for(i = 0, s = (int) size; s > 0; s -= tmpfile->fs_info->block_size, i++)
            {
                if(addr)
                {
                    blockstring += QString::number(addr + i) + "|";
                }
            }
        }
    }
    return TSK_WALK_CONT;
}

QString GetFilePermissions(TSK_FS_META* tmpmeta)
{
    QString tmpstring = "----------";
    tmpstring.replace(0, 1, tsk_fs_meta_type_str[tmpmeta->type][0]);
    if(tmpmeta->mode & TSK_FS_META_MODE_IRUSR)
        tmpstring.replace(1, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWUSR)
        tmpstring.replace(2, 1, "w");
    if(tmpmeta->mode & TSK_FS_META_MODE_ISUID)
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXUSR)
            tmpstring.replace(3, 1, "s");
        else
            tmpstring.replace(3, 1, "S");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXUSR)
        tmpstring.replace(3, 1, "x");
    if(tmpmeta->mode & TSK_FS_META_MODE_IRGRP)
        tmpstring.replace(4, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWGRP)
        tmpstring.replace(5, 1, "w");
    if(tmpmeta->mode && TSK_FS_META_MODE_ISGID)
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXGRP)
            tmpstring.replace(6, 1, "s");
        else
            tmpstring.replace(6, 1, "S");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXGRP)
        tmpstring.replace(6, 1, "x");
    if(tmpmeta->mode & TSK_FS_META_MODE_IROTH)
        tmpstring.replace(7, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWOTH)
        tmpstring.replace(8, 1, "w");
    if(tmpmeta->mode & TSK_FS_META_MODE_ISVTX) // sticky bit
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXOTH)
            tmpstring.replace(9, 1, "t");
        else
            tmpstring.replace(9, 1, "T");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXOTH)
        tmpstring.replace(9, 1, "x");
    return tmpstring;
}

TSK_WALK_RET_ENUM FileEntries(TSK_FS_FILE* tmpfile, const char* tmppath, void* tmpptr)
{
    filesfound++;
    //processphase++;
    isignals->ProgUpd();
    if(tmpptr != NULL)
    {
        LogMessage("TmpPtr got a value somehow");
    }

    QVector<QString> filestrings;
    if(tmpfile->name != NULL) filestrings.append(QString(tmpfile->name->name));
    else filestrings.append(QString("unknown.wbt"));
    filestrings.append(QString("/") + QString(tmppath));

    QVector<unsigned long long> fileints;
    if(tmpfile->name != NULL)
    {
        fileints.append((unsigned long long)tmpfile->name->type);
        fileints.append((unsigned long long)tmpfile->name->par_addr);
    }
    else
    {
        fileints.append(0);
        fileints.append(0);
    }
    if(tmpfile->meta != NULL)
    {
        fileints.append((unsigned long long)tmpfile->meta->atime);
        fileints.append((unsigned long long)tmpfile->meta->ctime);
        fileints.append((unsigned long long)tmpfile->meta->crtime);
        fileints.append((unsigned long long)tmpfile->meta->mtime);
        fileints.append((unsigned long long)tmpfile->meta->size);
        fileints.append((unsigned long long)tmpfile->meta->addr);
    }
    else
    {
        fileints.append(0);
        fileints.append(0);
        fileints.append(0);
        fileints.append(0);
        fileints.append(0);
        fileints.append(0);
    }
    fileints.append(currentfilesystemid);


    // add the extra file info for the alternate data stream
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        if(strcmp(tmpfile->name->name, ".") != 0)
        {
            if(strcmp(tmpfile->name->name, "..") != 0)
            {
                int cnt, i;
                cnt = tsk_fs_file_attr_getsize(tmpfile);
                qDebug() << "file name:" << tmpfile->name->name;
                for(i = 0; i < cnt; i++)
                {
                    char type[512];
                    const TSK_FS_ATTR* fsattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                    if(ntfs_attrname_lookup(tmpfile->fs_info, fsattr->type, type, 512) == 0)
                    {
                        if(QString::compare(QString(type), "$DATA", Qt::CaseSensitive) == 0)
                        {
                            if(QString::compare(QString(fsattr->name), "") != 0 && QString::compare(QString(fsattr->name), "$I30", Qt::CaseSensitive) != 0)
                            {
                                // MAKE THIS A FILE VARIABLE ADSDATA, WHICH WHEN I CALL THE NEXT FUNCTION, IT LOOKS TO SEE IF THE
                                // BOOLEAN VARIABLE HASADS IS SET TO TRUE. IF ITS TRUE, THEN CALL THE NEXT DB ENTRY TO ENTER
                                // THE ADSDATA AFTER THE FILEDATA, THEN SET THE ADS VARIABLE TO FALSE AND RESET ADSDATA TO 0'S AND "".
                                // 
                                // ACTUALLY, BECUASE ITS CONCURRENT, I'LL HAVE TO SEND THE ADS DATA AND ADSBOOLEAN VARIABLES
                                // TO THE CONCURRENT FUNCTION CALL SO THEY STAY WITH THE CORRECT THREAD...
                                FileData tmpdata;
                                tmpdata.type = (unsigned long long)tmpfile->name->type;
                                tmpdata.paraddr = (unsigned long long)tmpfile->meta->addr;
                                tmpdata.name = QString(":") + QString(fsattr->name);
                                tmpdata.size = (unsigned long long)fsattr->size;
                                tmpdata.mftattrid = (unsigned long long)fsattr->id; // STORE attr id in this variable in the db.

                                // IF I WANT TO ADD THIS TO THE DB AFTER I ADD THE MAIN FILE, I'LL NEED TO STORE THIS AND CALL IT SOMEWHERE
                                // ELSE AFTER I CALL THE MAIN ONE...
                                //
                                //
                                // NOW I NEED TO CREATE A FILE ENTRY WITH THE SAME INFO, EXCEPT IT CONTAINS DIFFERENT CONTENT, SO I NEED
                                // A WAY TO REFERENCE THAT DIFFERENT CONTENT WHEN I CLICK ON IT...
                                // 
                                // ICAT USES TSK_FS_FILE_WALK_TYPE(TMPFILE, ATTR TYPE 128, ID 6, CALLBACK) TO PRINT IT... 
                                // I'LL HAVE TO FIGURE OUT SOME WAY TO STORE THAT INFORMATION SO I CAN RETRIEVE THE INFORMATION I NEED...
                                qDebug() << "attr type:" << QString::fromStdString(std::string(type)) << "attr name:" << fsattr->name;
                            }
                        }
                        // the above gets me the attribute name such as $STANDARD_INFORMATION, $DATA, $FILE_NAME, $OBJECT_ID, etc.
                    }
                }
            }
        }
    }

    /*
     *
     * NTFS ALTERNATE DATA STREAM CODE
     * 
     * if ((TSK_FS_TYPE_ISNTFS(fs_file->fs_info->ftype))
            && (fs_file->meta)) {
            uint8_t printed = 0;
            int i, cnt;

            // cycle through the attributes
            cnt = tsk_fs_file_attr_getsize(fs_file);
            for (i = 0; i < cnt; i++) {
                const TSK_FS_ATTR *fs_attr =
                    tsk_fs_file_attr_get_idx(fs_file, i);
                if (!fs_attr)
                    continue;

                if (fs_attr->type == TSK_FS_ATTR_TYPE_NTFS_DATA) {
                    printed = 1;

                    if (fs_file->meta->type == TSK_FS_META_TYPE_DIR) {

                        if ((fs_file->name->name[0] == '.')
                            && (fs_file->name->name[1])
                            && (fs_file->name->name[2] == '\0')
                            && ((fls_data->flags & TSK_FS_FLS_DOT) == 0)) {
                            continue;
                        }
                    }

                    printit(fs_file, a_path, fs_attr, fls_data);

		    if ((fs_attr) && (fs_attr->name)) {
		        if ((fs_attr->type != TSK_FS_ATTR_TYPE_NTFS_IDXROOT) ||
		            (strcmp(fs_attr->name, "$I30") != 0)) {
		            tsk_fprintf(hFile, ":");
		            buf = malloc(strlen(fs_attr->name) + 1);
		            strcpy(buf, fs_attr->name);
		            for (i = 0; i < strlen(buf); i++) {
		                if (TSK_IS_CNTRL(buf[i])) buf[i] = '^';
		            }
		            tsk_fprintf(hFile, "%s", buf);
     *
     */ 

    //filesfound++;
    QFuture<void> tmpfuture = QtConcurrent::run(ProcessFile, filestrings, fileints);
    //filewatcher.setFuture(tmpfuture);
    //threadvector.append(tmpfuture);
    //qDebug() << "thread added. new thread vector count: " << threadvector.count();
    return TSK_WALK_CONT;
}

void SecondaryProcessing()
{
    QSqlQuery filequery(fcasedb);
    unsigned long long fsoffset = 0;
    unsigned long long parfsid = 0;
    filequery.prepare("SELECT objectid, parimgid, parfsid, address FROM data WHERE objecttype = 5;");
    if(filequery.exec())
    {
        while(filequery.next())
        {
            parfsid = filequery.value(2).toULongLong();
            const TSK_TCHAR** imagepartspath;
            unsigned long long objectid = 0;
            TSK_IMG_INFO* readimginfo;
            TSK_FS_INFO* readfsinfo;
            TSK_FS_FILE* readfileinfo;
            // Open Parent Image
            std::vector<std::string> pathvector;
            pathvector.clear();
            QSqlQuery imgquery(fcasedb);
            imgquery.prepare("SELECT fullpath FROM dataruns WHERE objectid = ? ORDER BY seqnum;");
            imgquery.bindValue(0, filequery.value(1).toULongLong());
            if(imgquery.exec())
            {
                while(imgquery.next())
                {
                    pathvector.push_back(imgquery.value(0).toString().toStdString());
                }
            }
            imgquery.finish();

            objectid = filequery.value(0).toULongLong();
            imagepartspath = (const char**)malloc(pathvector.size()*sizeof(char*));

            for(uint i=0; i < pathvector.size(); i++)
            {
                imagepartspath[i] = pathvector.at(i).c_str();
            }
            readimginfo = tsk_img_open(pathvector.size(), imagepartspath, TSK_IMG_TYPE_DETECT, 0);
            free(imagepartspath);
            //OpenParentFileSystem
            QSqlQuery fsquery(fcasedb);
            fsquery.prepare("SELECT byteoffset FROM data where objectid = ?;");
            fsquery.bindValue(0, filequery.value(2).toULongLong());
            fsquery.exec();
            fsquery.next();
            fsoffset = fsquery.value(0).toULongLong();
            readfsinfo = tsk_fs_open_img(readimginfo, fsquery.value(0).toULongLong(), TSK_FS_TYPE_DETECT);
            fsquery.finish();
            //OpenFile
            readfileinfo = tsk_fs_file_open_meta(readfsinfo, NULL, filequery.value(3).toULongLong());
            HashFile(readfileinfo, objectid);
            MagicFile(readfileinfo, objectid);
            BlockFile(readfileinfo, objectid);
            PropertyFile(readfileinfo, objectid, fsoffset, readfsinfo->block_size, parfsid);

            filesprocessed++;
            isignals->ProgUpd();

            tsk_fs_file_close(readfileinfo);
            tsk_fs_close(readfsinfo);
            tsk_img_close(readimginfo);
        }
    }
    filequery.finish();
}

void GenerateThumbnails()
{
    QSqlQuery filequery(fcasedb);
    filequery.prepare("SELECT objectid, parimgid, parfsid, address FROM data WHERE objecttype = 5 AND filemime LIKE '%image/%';");
    if(filequery.exec())
    {
        while(filequery.next())
        {
            const TSK_TCHAR** imagepartspath;
            unsigned long long objectid = 0;
            TSK_IMG_INFO* readimginfo;
            TSK_FS_INFO* readfsinfo;
            TSK_FS_FILE* readfileinfo;
            // Open Parent Image
            std::vector<std::string> pathvector;
            pathvector.clear();
            QSqlQuery imgquery(fcasedb);
            imgquery.prepare("SELECT fullpath FROM dataruns WHERE objectid = ? ORDER BY seqnum;");
            imgquery.bindValue(0, filequery.value(1).toULongLong());
            if(imgquery.exec())
            {
                while(imgquery.next())
                {
                    pathvector.push_back(imgquery.value(0).toString().toStdString());
                }
            }
            imgquery.finish();

            objectid = filequery.value(0).toULongLong();
            imagepartspath = (const char**)malloc(pathvector.size()*sizeof(char*));

            for(uint i=0; i < pathvector.size(); i++)
            {
                imagepartspath[i] = pathvector.at(i).c_str();
            }
            readimginfo = tsk_img_open(pathvector.size(), imagepartspath, TSK_IMG_TYPE_DETECT, 0);
            free(imagepartspath);
            //OpenParentFileSystem
            QSqlQuery fsquery(fcasedb);
            fsquery.prepare("SELECT byteoffset FROM data where objectid = ?;");
            fsquery.bindValue(0, filequery.value(2).toULongLong());
            fsquery.exec();
            fsquery.next();
            readfsinfo = tsk_fs_open_img(readimginfo, fsquery.value(0).toULongLong(), TSK_FS_TYPE_DETECT);
            fsquery.finish();
            //OpenFile
            readfileinfo = tsk_fs_file_open_meta(readfsinfo, NULL, filequery.value(3).toULongLong());

            ThumbFile(readfileinfo, objectid);

            tsk_fs_file_close(readfileinfo);
            tsk_fs_close(readfsinfo);
            tsk_img_close(readimginfo);

        }
    }
    filequery.finish();

}

void PropertyFile(TSK_FS_FILE* tmpfile, unsigned long long objid, unsigned long long fsoffset, int blksize, unsigned long long parfsid)
{
    QStringList proplist;
    proplist.clear();
    if(tmpfile->name != NULL) proplist << "Short Name" << tmpfile->name->shrt_name << "Short Name for a file";
    if(tmpfile->meta != NULL)
    {
        proplist << "File Permissions" << GetFilePermissions(tmpfile->meta) << "Unix Style Permissions. r - file, d - directory, l - symbolic link, c - character device, b - block device, p - named pipe, v - virtual file created by the forensic tool; r - read, w - write, x - execute, s - set id and executable, S - set id, t - sticky bit executable, T - sticky bit. format is type|user|group|other - [rdlcbpv]|rw[sSx]|rw[sSx]|rw[tTx]";
        proplist << "User ID" << QString::number(tmpfile->meta->uid) << "User ID";
        proplist << "Group ID" << QString::number(tmpfile->meta->gid) << "Group ID";
        proplist << "Allocation Status";
        if(tmpfile->meta->flags == TSK_FS_META_FLAG_ALLOC)
            proplist << "Currently Allocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_UNALLOC)
            proplist << "Currently Unallocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_USED)
            proplist << "Allocated at least once";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_UNUSED)
            proplist << "Never allocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_COMP)
            proplist << "Contents are compressed";
        else
            proplist << "Unspecified";
        proplist << "allocation status for the file.";

        /* insert ntfs and hfs attributes here */
        if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
        {
            int cnt, i;
            cnt = tsk_fs_file_attr_getsize(tmpfile);
            for(i = 0; i < cnt; i++)
            {
                char type[512];
                const TSK_FS_ATTR* fsattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                if(ntfs_attrname_lookup(tmpfile->fs_info, fsattr->type, type, 512) == 0)
                {
                    if(QString::compare(QString(type), "$DATA", Qt::CaseSensitive) == 0)
                    {
                        if(QString::compare(QString(fsattr->name), "") != 0 && QString::compare(QString(fsattr->name), "$I30", Qt::CaseSensitive) != 0)
                        {
                            //qDebug() << "attr type:" << QString::fromStdString(std::string(type)) << "attr name:" << fsattr->name;
                        }
                    }
                    // the above gets me the attribute name such as $STANDARD_INFORMATION, $DATA, $FILE_NAME, $OBJECT_ID, etc.
                }
            }
            /*
            const TSK_FS_ATTR* fsattr;
            fsattr = tsk_fs_attrlist_get(tmpfile->meta->attr, NTFS_ATYPE_SI);
            if(fsattr)
            {
                ntfs_attr_si* si = (ntfs_attr_si*)fsattr->rd.buf;
                char* sidstr;
                int a = 0;
                proplist << "$STANDARD_INFORMATION Attribute Values";
                // ntfs.c line 4214 working on Flags.
            }
            */
        }
        if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT)
        {
            // hfs.c line 5446 working on hfs file information.
        }

        QSqlQuery objquery(fcasedb);
        objquery.prepare("SELECT blockaddress, filemime, filesignature, address FROM data WHERE objectid = ?;");
        objquery.bindValue(0, objid);
        objquery.exec();
        objquery.next();
        proplist << "Block Address" << objquery.value(0).toString() << "List of block addresses which contain the contents of the file";
        unsigned long long tmpoffset = 0;
        unsigned long long resoffset = 0;
        unsigned long long fileaddress = 0;
        fileaddress = objquery.value(3).toULongLong();
        proplist << "Byte Offset";
        if(objquery.value(0).toString().compare("") != 0)
            tmpoffset = objquery.value(0).toString().split("|", QString::SkipEmptyParts).at(0).toULongLong()*blksize + fsoffset;
        else
        {
            QSqlQuery resquery(fcasedb);
            QStringList inputs;
            QList<unsigned long long> outputs;
            inputs << "%0x0B%" << "%0x0D%" << "%0x30%" << "%0x40%";
            for(int i=0; i < inputs.count(); i++)
            {
                resquery.prepare("SELECT value from properties where objectid = ? and description like(?);");
                resquery.addBindValue(parfsid);
                resquery.addBindValue(inputs.at(i));
                resquery.exec();
                resquery.next();
                outputs.append(resquery.value(0).toULongLong());
            }
            resquery.finish();
            mftrecordsize = outputs.at(3);
            resoffset = ((outputs.at(0) * outputs.at(1) * outputs.at(2)) + (outputs.at(3)*fileaddress));

            tmpoffset = resoffset + fsoffset;
        }
        proplist << QString::number(tmpoffset) << "Byte offset for the start of the file in a block or in the MFT";
        proplist << "File Signature" << objquery.value(1).toString() << objquery.value(2).toString();
        objquery.finish();

        fcasedb.transaction();
        QSqlQuery propquery(fcasedb);
        propquery.prepare("INSERT INTO properties (objectid, name, value, description) VALUES(?, ?, ?, ?);");
        for(int i=0; i < proplist.count()/3; i++)
        {
            propquery.bindValue(0, objid);
            propquery.bindValue(1, proplist.at(3*i));
            propquery.bindValue(2, proplist.at(3*i+1));
            propquery.bindValue(3, proplist.at(3*i+2));
            propquery.exec();
        }
        fcasedb.commit();
        propquery.finish();
        processphase++;
        //filesprocessed++;
        isignals->ProgUpd();
    }
}

void BlockFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    blockstring = "";
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT)
    {
        if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT)
        {
            tsk_fs_file_walk_type(tmpfile, TSK_FS_ATTR_TYPE_HFS_DATA, HFS_FS_ATTR_ID_DATA, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT)
        {
            iso9660_inode* dinode;
            dinode = (iso9660_inode*)tsk_malloc(sizeof(iso9660_inode));
            iso9660_inode_node* n;
            n = ((ISO_INFO*)tmpfile->fs_info)->in_list;
            while(n && (n->inum != tmpfile->meta->addr))
                n = n->next;
            if(n)
                memcpy(dinode, &n->inode, sizeof(iso9660_inode));
            int block = tsk_getu32(tmpfile->fs_info->endian, dinode->dr.ext_loc_m);
            TSK_OFF_T size = tmpfile->meta->size;
            while((int64_t)size > 0)
            {
                blockstring += QString::number(block++) + "|";
                //qDebug() << "File Name:" << tmpfile->name->name << "Block Address:" << block++;
                size -= tmpfile->fs_info->block_size;
            }
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
        {
            // NEED TO COMPARE TMPFILE->MFTATTRID == TMPATTR->ID TO ENSURE ITS THE CORRECT ATTRIBUTE FILE.
            // CURRENTLY I'M GETTING THE BLOCKS FOR THE REGULAR DATA AND THE ADS DATA.
            if(tmpfile->meta != NULL)
            {
                if(tmpfile->meta->attr)
                {
                    int cnt, i;
                    cnt = tsk_fs_file_attr_getsize(tmpfile);
                    for(i = 0; i < cnt; i++)
                    {
                        //char type[512];
                        const TSK_FS_ATTR* tmpattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                        if(tmpattr->flags & TSK_FS_ATTR_NONRES) // non resident attribute
                        {
                            tsk_fs_file_walk_type(tmpfile, tmpattr->type, tmpattr->id, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
                        }
                    }
                }
            }
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT)
        {
            tsk_fs_file_walk(tmpfile, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
        }
    }
    else
        tsk_fs_file_walk(tmpfile, TSK_FS_FILE_WALK_FLAG_AONLY, GetBlockAddress, NULL);
    QSqlQuery blockquery(fcasedb);
    blockquery.prepare("UPDATE data SET blockaddress = ? WHERE objectid = ?;");
    blockquery.bindValue(0, blockstring);
    blockquery.bindValue(1, objid);
    blockquery.exec();
    blockquery.next();
    blockquery.finish();
    processphase++;
    isignals->ProgUpd();
    //proplist << "Block Address" << blockstring << "List of block addresses which contain the contents of the file";
    //*/

}

void MagicFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    // FILE MIME TYPE
    char magicbuffer[1024];
    const char* mimesig;
    const char* sigtype;
    char* sigp1;
    char* sigp2;
    ssize_t readlen = tsk_fs_file_read(tmpfile, 0, magicbuffer, 1024, TSK_FS_FILE_READ_FLAG_NONE);
    if(readlen > 0)
    {
        mimesig = magic_buffer(magicmimeptr, magicbuffer, readlen);
        sigp1 = strtok((char*)mimesig, ";");
        sigtype = magic_buffer(magicptr, magicbuffer, readlen);
        sigp2 = strtok((char*)sigtype, ",");
    }
    QSqlQuery mimequery(fcasedb);
    mimequery.prepare("UPDATE data SET filemime = ?, filesignature = ? WHERE objectid = ?;");
    if(readlen > 0)
    {
        mimequery.bindValue(0, QString::fromStdString(sigp1));
        mimequery.bindValue(1, QString::fromStdString(sigp2));
    }
    else
    {
        mimequery.bindValue(0, QString("Zero File"));
        mimequery.bindValue(1, QString("Zero File"));
    }
    mimequery.bindValue(2, objid);
    mimequery.exec();
    mimequery.next();
    mimequery.finish();
    processphase++;
    //filesprocessed++;
    isignals->ProgUpd();
}

void ThumbFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    if(tmpfile->meta != NULL)
    {
        QByteArray thumbdata;
        QImage thumbimage;
        QBuffer thumbuf(&thumbdata);
        QImage origimage;
        char imagebuffer[tmpfile->meta->size];
        ssize_t imglen = tsk_fs_file_read(tmpfile, 0, imagebuffer, tmpfile->meta->size, TSK_FS_FILE_READ_FLAG_NONE);
        bool imageloaded = origimage.loadFromData(QByteArray::fromRawData(imagebuffer, imglen));
        if(imageloaded)
        {
            thumbimage = origimage.scaled(QSize(320, 320), Qt::KeepAspectRatio, Qt::FastTransformation);
            thumbuf.open(QIODevice::WriteOnly);
            thumbimage.save(&thumbuf, "PNG");
            thumbdata = thumbdata.toBase64();
            QSqlQuery imgquery(thumbdb);
            imgquery.prepare("INSERT INTO thumbs(objectid, thumbblob) VALUES(?, ?);");
            imgquery.bindValue(0, objid);
            imgquery.bindValue(1, QString(thumbdata));
            imgquery.exec();
            imgquery.next();
            imgquery.finish();
        }
    }
    //processphase++;
    //isignals->ProgUpd();
}

void HashFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    TSK_FS_HASH_RESULTS hashresults;
    uint8_t retval = tsk_fs_file_hash_calc(tmpfile, &hashresults, TSK_BASE_HASH_MD5);
    if(retval == 0)
    {
        char sbuf[17];
        int sint = 0;
        for(int i=0; i < 16; i++)
        {
            sint = sprintf(sbuf+(2*i), "%02X", hashresults.md5_digest[i]);
        }
        /*
        if(sint > 0)
            fileshash.insert(objid, QString(sbuf)); 
        else
            fileshash.insert(objid, QString(""));
        */
        QSqlQuery hashquery(fcasedb);
        hashquery.prepare("UPDATE data SET md5 = ? where objectid = ?;");
        if(sint > 0)
            hashquery.bindValue(0, QString(sbuf));
        else
            hashquery.bindValue(0, QString(""));
        hashquery.bindValue(1, objid);
        hashquery.exec();
        hashquery.next();
        hashquery.finish();
    }
    processphase++;
    isignals->ProgUpd();
}

void cnid_to_array(uint32_t cnid, uint8_t array[4])
{
    array[3] = (cnid >> 0) & 0xff;
    array[2] = (cnid >> 8) & 0xff;
    array[1] = (cnid >> 16) & 0xff;
    array[0] = (cnid >> 24) & 0xff;
}


std::string GetSegmentValue(IMG_AFF_INFO* curaffinfo, const char* segname)
{
    unsigned char buf[512];
    size_t buflen = 512;
    std::stringstream stm;
    std::string s;
    int ilimit = 0;
    if(af_get_seg(curaffinfo->affile, segname, NULL, buf, &buflen) == 0)
    {
        if(strcmp(segname, AF_MD5) == 0 || strcmp(segname,AF_SHA1) == 0 || strcmp(segname, AF_IMAGE_GID) == 0)
        {
            if(strcmp(segname,AF_MD5) == 0)
                ilimit = 16;
            else if(strcmp(segname, AF_SHA1) == 0)
                ilimit = 20;
            else
                ilimit = buflen;
            unsigned char* bytebuf = buf;
            stm << std::hex << std::setfill('0');
            for(int i=0; i < ilimit; i++)
            {
                stm << std::setw(2) << static_cast<int>(bytebuf[i]);
            }
            s = stm.str();
        }
        else
        {
            buf[buflen] = '\0';
            stm << buf;
            s = stm.str();
        }
    }
    return s;
}

QImage MakeThumb(const QString &img)
{
    QByteArray ba = QByteArray::fromBase64(QByteArray::fromStdString(img.toStdString()));
    if(ba.length() > 0)
        return QImage::fromData(ba, "PNG").scaled(QSize(thumbsize, thumbsize), Qt::KeepAspectRatio, Qt::FastTransformation);
    else
        return QImage(":/bar/missingthumb").scaled(QSize(thumbsize, thumbsize), Qt::KeepAspectRatio, Qt::FastTransformation);
}
