#include "wombatforensics.h"

WombatForensics::WombatForensics(QWidget *parent) : QMainWindow(parent), ui(new Ui::WombatForensics)
{
    ui->setupUi(this);
    threadpool = QThreadPool::globalInstance();
    wombatcasedata = new WombatDatabase();
    wombatprogresswindow = new ProgressWindow(wombatcasedata);
    isleuthkit = new SleuthKitPlugin(wombatcasedata);
    ibasictools = new BasicTools();
    connect(wombatprogresswindow, SIGNAL(HideProgressWindow(bool)), this, SLOT(HideProgressWindow(bool)), Qt::DirectConnection);
    connect(isleuthkit, SIGNAL(UpdateStatus(int, int)), this, SLOT(UpdateProgress(int, int)), Qt::QueuedConnection);
    connect(isleuthkit, SIGNAL(UpdateMessageTable()), this, SLOT(UpdateMessageTable()), Qt::QueuedConnection);
    connect(isleuthkit, SIGNAL(ReturnImageNode(QStandardItem*)), this, SLOT(GetImageNode(QStandardItem*)), Qt::QueuedConnection);
    qRegisterMetaType<WombatVariable>("WombatVariable");
    qRegisterMetaType<FileExportData>("FileExportData");
    connect(this, SIGNAL(LogVariable(WombatVariable)), isleuthkit, SLOT(GetLogVariable(WombatVariable)), Qt::QueuedConnection);
    connect(wombatcasedata, SIGNAL(DisplayError(QString, QString, QString)), this, SLOT(DisplayError(QString, QString, QString)), Qt::DirectConnection);
    connect(isleuthkit, SIGNAL(LoadFileContents(QString)), this, SLOT(SendFileContents(QString)), Qt::QueuedConnection);
    connect(isleuthkit, SIGNAL(PopulateProgressWindow(WombatVariable)), this, SLOT(PopulateProgressWindow(WombatVariable)), Qt::QueuedConnection);
    wombatprogresswindow->setModal(false);
    wombatvariable.caseid = 0;
    wombatvariable.evidenceid = 0;
    wombatvariable.jobtype = 0;
    InitializeAppStructure();
    InitializeSleuthKit();
}

void WombatForensics::HideProgressWindow(bool checkedstate)
{
    ui->actionView_Progress->setChecked(checkedstate);
}

std::string WombatForensics::GetTime()
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

void WombatForensics::InitializeAppStructure()
{
    QString homePath = QDir::homePath();
    homePath += "/WombatForensics/";
    wombatvariable.settingspath = homePath + "settings";
    wombatvariable.datapath = homePath + "data/";
    wombatvariable.casespath = homePath + "cases/";
    wombatvariable.tmpfilepath = homePath + "tmpfiles";
    bool mkPath = (new QDir())->mkpath(wombatvariable.settingspath);
    if(mkPath == false)
        DisplayError("2.0", "App Settings Folder Failed.", "App Settings Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatvariable.datapath);
    if(mkPath == false)
        DisplayError("2.1", "App Data Folder Failed.", "Application Data Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatvariable.casespath);
    if(mkPath == false)
        DisplayError("2.2", "App Cases Folder Failed.", "App Cases Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatvariable.tmpfilepath);
    if(mkPath == false)
        DisplayError("2.2", "App TmpFile Folder Failed.", "App TmpFile Folder was not created.");
    QString logPath = wombatvariable.datapath + "WombatLog.db";
    bool logFileExist = wombatcasedata->FileExists(logPath.toStdString());
    if(!logFileExist)
    {
        const char* errstring = wombatcasedata->CreateLogDB(logPath);
        if(strcmp(errstring, "") != 0)
            DisplayError("1.0", "Log File Error", errstring);
    }
    QString tmpPath = wombatvariable.datapath + "WombatCase.db";
    bool doesFileExist = wombatcasedata->FileExists(tmpPath.toStdString());
    if(!doesFileExist)
    {
        const char* errstring = wombatcasedata->CreateCaseDB(tmpPath);
        if(strcmp(errstring, "") != 0)
            DisplayError("1.0", "File Error", errstring);
    }
    else
    {
        const char* errstring = wombatcasedata->OpenCaseDB(tmpPath);
        if(strcmp(errstring, "") != 0)
            DisplayError("1.1", "SQL", errstring);
    }
    if(wombatcasedata->ReturnCaseCount() == 0)
    {
        ui->actionOpen_Case->setEnabled(false);
    }
    else if(wombatcasedata->ReturnCaseCount() > 0)
    {
        ui->actionOpen_Case->setEnabled(true);
    }
    else
    {
        DisplayError("1.0", "Case Count", "Invalid Case Count returned.");
    }
    ui->fileViewTabWidget->addTab(ibasictools->setupHexTab(), "Hex View");
    ui->fileViewTabWidget->addTab(ibasictools->setupTxtTab(), "Text View");
    ui->fileInfoTabWidget->addTab(ibasictools->setupDirTab(), "Directory List");
    ui->fileInfoTabWidget->addTab(ibasictools->setupTypTab(), "File Type");
    SetupDirModel();
}

void WombatForensics::InitializeSleuthKit()
{
    ThreadRunner* initrunner = new ThreadRunner(isleuthkit, "initialize", wombatvariable);
    threadpool->start(initrunner);
    threadpool->waitForDone();
}

void WombatForensics::AddEvidence()
{
    QString evidenceFilePath = QFileDialog::getOpenFileName(this, tr("Select Evidence Item"), tr("./"));
    //fprintf(stderr, "Evidence FilePath: %s\n", evidenceFilePath.toStdString().c_str());
    if(evidenceFilePath != "")
    {
        wombatprogresswindow->show();
        wombatprogresswindow->ClearTableWidget();
        wombatvariable.jobtype = 1; // add evidence
        // DETERMINE IF THE EVIDENCE NAME EXISTS, IF IT DOES THEN PROMPT USER THAT ITS OPEN ALREADY. IF THEY WANT TO OPEN A SECOND COPY
        // THEN SET NEWEVIDENCENAME EVIDENCEFILEPATH.SPLIT("/").LAST() + "COPY.DB"
        QString evidenceName = evidenceFilePath.split("/").last();
        evidenceName += ".db";
        wombatvariable.evidenceid = wombatcasedata->InsertEvidence(evidenceName, evidenceFilePath, wombatvariable.caseid);
        wombatvariable.evidenceidlist.append(wombatvariable.evidenceid);
        wombatvariable.evidencepath = evidenceFilePath;
        wombatvariable.evidencepathlist << wombatvariable.evidencepath;
        wombatvariable.evidencedbname = evidenceName;
        wombatvariable.evidencedbnamelist << wombatvariable.evidencedbname;
        wombatvariable.jobid = wombatcasedata->InsertJob(wombatvariable.jobtype, wombatvariable.caseid, wombatvariable.evidenceid);
        emit LogVariable(wombatvariable);
        QString tmpString = evidenceName;
        tmpString += " - ";
        tmpString += QString::fromStdString(GetTime());
        QStringList tmpList;
        tmpList << tmpString << QString::number(wombatvariable.jobid);
        wombatprogresswindow->UpdateAnalysisTree(0, new QTreeWidgetItem(tmpList));
        wombatprogresswindow->UpdateFilesFound("0");
        wombatprogresswindow->UpdateFilesProcessed("0");
        wombatprogresswindow->UpdateAnalysisState("Adding Evidence to Database");
        LOGINFO("Adding Evidence Started");
        wombatcasedata->InsertMsg(wombatvariable.caseid, wombatvariable.evidenceid, wombatvariable.jobid, 2, "Adding Evidence Started");
        ThreadRunner* trun = new ThreadRunner(isleuthkit, "openevidence", wombatvariable);
        threadpool->start(trun);
        //fprintf(stderr, "open evidence exists");
    }
}

void WombatForensics::RemEvidence()
{
    wombatprogresswindow->ClearTableWidget();
    wombatvariable.jobtype = 2; // remove evidence
    fprintf(stderr, "remove evidence\n");
    QStringList evidenceList;
    evidenceList.clear();
    // populate case list here
    evidenceList = wombatcasedata->ReturnCaseActiveEvidence(wombatvariable.caseid);
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Remove Existing Evidence"), tr("Select Evidence to Remove: "), evidenceList, 0, false, &ok);
    if(ok && !item.isEmpty()) // open selected case
    {
        wombatvariable.evidenceid = wombatcasedata->ReturnEvidenceID(item);
        wombatvariable.jobid = wombatcasedata->InsertJob(wombatvariable.jobtype, wombatvariable.caseid, wombatvariable.evidenceid);
        emit LogVariable(wombatvariable);
        QString tmpstring = item.split("/").last() + " - " + QString::fromStdString(GetTime());
        QStringList tmplist;
        tmplist << tmpstring << QString::number(wombatvariable.jobid);
        wombatprogresswindow->UpdateAnalysisTree(2, new QTreeWidgetItem(tmplist));
        wombatprogresswindow->UpdateFilesFound("");
        wombatprogresswindow->UpdateFilesProcessed("");
        wombatprogresswindow->UpdateAnalysisState("Removing Evidence");
        LOGINFO("Removing Evidence Started");
        wombatcasedata->InsertMsg(wombatvariable.caseid, wombatvariable.evidenceid, wombatvariable.jobid, 2, "Removing Evidence Started");
        UpdateMessageTable();
        wombatcasedata->RemoveEvidence(item);
        wombatprogresswindow->UpdateProgressBar(25);
        QString tmppath = wombatvariable.evidencedirpath + item.split("/").last() + ".db";
        if(QFile::remove(tmppath))
        {
            //fprintf(stderr, "file was removed");
        }
        else
            emit DisplayError("2.1", "Evidence DB File was NOT Removed", "");
        wombatprogresswindow->UpdateProgressBar(50);
        UpdateCaseData(wombatvariable);
        wombatprogresswindow->UpdateProgressBar(75);
        LOGINFO("Removing Evidence Finished");
        wombatcasedata->InsertMsg(wombatvariable.caseid, wombatvariable.evidenceid, wombatvariable.jobid, 2, "Removing Evidence Finished");
        wombatcasedata->UpdateJobEnd(wombatvariable.jobid, 0, 0);
        UpdateMessageTable();
        wombatprogresswindow->UpdateAnalysisState("Removing Evidence Finished");
        wombatprogresswindow->UpdateProgressBar(100);
    }
}

int WombatForensics::StandardItemCheckState(QStandardItem* tmpitem, int checkcount)
{
    int curcount = checkcount;
    fprintf(stderr, "curcount: %i\n", curcount);
    if(tmpitem->hasChildren())
    {
        for(int i=0; i < tmpitem->rowCount(); i++)
        {
            curcount = StandardItemCheckState(tmpitem->child(i,1), curcount);
        }
    }
    if(tmpitem->checkState() == 2)
        curcount++;

    return curcount;
}

std::vector<FileExportData> WombatForensics::SetFileExportProperties(QStandardItem* tmpitem, FileExportData tmpexport, std::vector<FileExportData> tmpexportlist)
{
    if(tmpitem->hasChildren())
    {
        for(int i=0; i < tmpitem->rowCount(); i++)
        {
            tmpexportlist = SetFileExportProperties(tmpitem->child(i,1), tmpexport, tmpexportlist);
        }
    }
    if(tmpitem->checkState() == 2) // if checked
    {
        // update evidence/file info for each item bit.
        QModelIndex curindex = tmpitem->index();
        tmpexport.id = curindex.sibling(curindex.row(), 1).data().toString().toInt(); // unique object id
        wombatvariable.evidenceid = wombatcasedata->ReturnObjectEvidenceID(tmpexport.id); // evidence id
        QStringList currentevidencelist = wombatcasedata->ReturnEvidenceData(wombatvariable.evidenceid); // evidence data
        wombatvariable.evidencepath = currentevidencelist[0]; // evidence path
        wombatvariable.evidencedbname = currentevidencelist[1]; // evidence db name
        wombatvariable.fileid = wombatcasedata->ReturnObjectFileID(tmpexport.id); // file id
        tmpexport.name = curindex.sibling(curindex.row(), 0).data().toString().toStdString(); // file name
        if(tmpexport.pathstatus == FileExportData::include)
        {
            tmpexport.fullpath = tmpexport.exportpath + "/" + curindex.sibling(curindex.row(), 2).data().toString().toStdString(); // export path with original path
        }
        else if(tmpexport.pathstatus == FileExportData::exclude)
        {
            tmpexport.fullpath = tmpexport.exportpath + "/" + tmpexport.name; // export path without original path
        }
        fprintf(stderr, "export full path checked: %s\n", tmpexport.fullpath.c_str());

        tmpexportlist.push_back(tmpexport);
    }

    return tmpexportlist;
}

int WombatForensics::StandardItemListCount(QStandardItem* tmpitem, int listcount)
{
    int curcount = listcount;
    if(tmpitem->hasChildren())
    {
        for(int i=0; i < tmpitem->rowCount(); i++)
        {
            curcount = StandardItemListCount(tmpitem->child(i,1), curcount);
        }
    }
    curcount++;

    return curcount;
}

void WombatForensics::ExportEvidence()
{
    int checkcount = 0;
    int listcount = 0;

    QStandardItem* rootitem = wombatdirmodel->invisibleRootItem();
    for(int i = 0; i < rootitem->rowCount(); i++)
    {
        QStandardItem* imagenode = rootitem->child(i,0);
        for(int j = 0; j < imagenode->rowCount(); j++)
        {
            QStandardItem* volumenode = imagenode->child(j,0);
            for(int k = 0; k < volumenode->rowCount(); k++)
            {
                QStandardItem* fsnode = volumenode->child(k,0);
                checkcount = StandardItemCheckState(fsnode, checkcount);
                listcount = StandardItemListCount(fsnode, listcount);
            }
        }
    }
    fprintf(stderr, "checked count: %i\n", checkcount);
    fprintf(stderr, "listed item count: %i\n", listcount);
    exportdialog = new ExportDialog(this, checkcount, listcount);
    connect(exportdialog, SIGNAL(FileExport(FileExportData)), this, SLOT(FileExport(FileExportData)), Qt::DirectConnection);
    exportdialog->show();
}

void WombatForensics::FileExport(FileExportData exportdata)
{
    /*
     * NEED TO SETUP THE PROGRESS WINDOW JOB FOR THIS EXPORT AND POPULATE IT ACCORDINGLY AS IT GOES THROUGH THE LOOPING PROCESS IN SLEUTHKIT...
     */ 
    std::vector<FileExportData> exportevidencelist;
    if(exportdata.filestatus == FileExportData::selected)
    {
        // need to store the image's dd path, whatever i need to set the db correctly.
        // for image db, i need the evidencedirpath and evidencedbname.
        // for image dd, i need the evidencepath
        exportdata.id = curselindex.sibling(curselindex.row(), 1).data().toString().toInt(); // unique objectid
        wombatvariable.evidenceid = wombatcasedata->ReturnObjectEvidenceID(exportdata.id); // evidence id
        QStringList currentevidencelist = wombatcasedata->ReturnEvidenceData(wombatvariable.evidenceid); // evidence data
        wombatvariable.evidencepath = currentevidencelist[0]; // evidence path
        wombatvariable.evidencedbname = currentevidencelist[1]; // evidence db name
        wombatvariable.fileid = wombatcasedata->ReturnObjectFileID(exportdata.id); // file id
        exportdata.name = curselindex.sibling(curselindex.row(),0).data().toString().toStdString(); // file name
        if(exportdata.pathstatus == FileExportData::include)
        {
            exportdata.fullpath = exportdata.exportpath;
            exportdata.fullpath += "/";
            exportdata.fullpath += curselindex.sibling(curselindex.row(), 2).data().toString().toStdString(); // export path with original path
        }
        else if(exportdata.pathstatus == FileExportData::exclude)
        {
            exportdata.fullpath = exportdata.exportpath + "/" + exportdata.name; // export path without original path
        }
        fprintf(stderr, "export full path: %s\n", exportdata.fullpath.c_str());

        exportevidencelist.push_back(exportdata);
    }
    else if(exportdata.filestatus == FileExportData::checked)
    {
        QStandardItem* rootitem = wombatdirmodel->invisibleRootItem();
        for(int i = 0; i < rootitem->rowCount(); i++) // loop over images
        {
            QStandardItem* imagenode = rootitem->child(i,0);
            for(int j = 0; j < imagenode->rowCount(); j++) // loop over volume(s)
            {
                QStandardItem* volumenode = imagenode->child(j,0); // loop over partition(s)
                for(int k = 0; k < volumenode->rowCount(); k++)
                {
                    QStandardItem* fsnode = volumenode->child(k,0);
                    exportevidencelist = SetFileExportProperties(fsnode, exportdata, exportevidencelist);
                }
            }
        }
    }
    else if(exportdata.filestatus == FileExportData::listed)
    {
        QStandardItem* rootitem = wombatdirmodel->invisibleRootItem()->child(0,0)->child(0,0)->child(0,0);
        // get the files listed and add them to the list
    }
    wombatvariable.exportdatalist = exportevidencelist;

    ThreadRunner* trun = new ThreadRunner(isleuthkit, "exportfiles", wombatvariable);
    threadpool->start(trun);
}

void WombatForensics::UpdateCaseData(WombatVariable wvariable)
{
    // refresh views here
    currenthexwidget = ui->fileViewTabWidget->findChild<BinViewWidget *>("bt-hexview");
    currenthexwidget->setModel(0);
    currenttxtwidget = ui->fileViewTabWidget->findChild<QTextEdit*>("bt-txtview");
    currenttxtwidget->setPlainText("");
    // refresh treeviews here
    wombatdirmodel->clear();
    QStringList headerList;
    headerList << "Name" << "Unique ID" << "Full Path" << "Size" << "Created" << "MD5 Hash";
    wombatdirmodel->setHorizontalHeaderLabels(headerList);
    currenttreeview = ui->fileInfoTabWidget->findChild<QTreeView *>("bt-dirtree");
    currenttreeview->setModel(wombatdirmodel);
    ThreadRunner* trun = new ThreadRunner(isleuthkit, "refreshtreeviews", wombatvariable);
    threadpool->start(trun);
}

void WombatForensics::UpdateProgress(int filecount, int processcount)
{
    int curprogress = (int)((((float)processcount)/(float)filecount)*100);
    wombatprogresswindow->UpdateFilesFound(QString::number(filecount));
    wombatprogresswindow->UpdateFilesProcessed(QString::number(processcount));
    wombatprogresswindow->UpdateProgressBar(curprogress);
}

void WombatForensics::UpdateMessageTable()
{
    wombatprogresswindow->ClearTableWidget();
    QStringList tmplist = wombatcasedata->ReturnMessageTableEntries(wombatvariable.jobid);
    wombatprogresswindow->UpdateMessageTable(tmplist);
}

void WombatForensics::PopulateProgressWindow(WombatVariable wvariable)
{
    int treebranch = 0;
    wombatvariable = wvariable;
    QStringList joblist = wombatcasedata->ReturnJobDetails(wvariable.jobid);
    QString tmpstring = wvariable.evidencedbname + " - " + joblist[0];
    QStringList tmplist;
    tmplist << tmpstring << QString::number(wvariable.jobid);
    if(wvariable.jobtype == 1) treebranch = 0;
    else if(wvariable.jobtype == 2) treebranch = 2;
    else treebranch = 1;
    wombatprogresswindow->UpdateAnalysisTree(treebranch,  new QTreeWidgetItem(tmplist));
    if(wvariable.jobtype == 2)
    {
        wombatprogresswindow->UpdateProgressBar(100);
        wombatprogresswindow->UpdateFilesFound("");
        wombatprogresswindow->UpdateFilesProcessed("");
    }
    else
    {
        wombatprogresswindow->UpdateFilesFound(joblist[1]);
        wombatprogresswindow->UpdateFilesProcessed(joblist[2]);
        int finalprogress = (int)((((float)joblist[2].toInt())/(float)joblist[1].toInt())*100);
        wombatprogresswindow->UpdateProgressBar(finalprogress);
    }
    wombatprogresswindow->UpdateAnalysisState(joblist[3]);
    UpdateMessageTable();
}


void WombatForensics::DisplayError(QString errorNumber, QString errorType, QString errorValue)
{
    QString tmpString = errorNumber;
    tmpString += ". SqlError: ";
    tmpString += errorType;
    tmpString += " Returned ";
    tmpString += errorValue;
    QMessageBox::warning(this, "Error", tmpString, QMessageBox::Ok);
}

void WombatForensics::GetImageNode(QStandardItem* imagenode)
{
    QStandardItem* currentroot = wombatdirmodel->invisibleRootItem();
    currentroot->appendRow(imagenode);
    currenttreeview->setModel(wombatdirmodel);
    ResizeColumns(wombatdirmodel);
    UpdateMessageTable();

}

void WombatForensics::ResizeColumns(QStandardItemModel* currentmodel)
{
    for(int i=0; i < currentmodel->columnCount(); i++)
    {
        // may need to compare treeview->model() == currentmodel) to determine what to set it to.
        // depending on the design though, i may not need multiple layouts since the columns can be sorted.
        // have to see as i go. for now its good.
        currenttreeview->resizeColumnToContents(i);
    }
    fprintf(stderr, "Resizing Column\n");
}

void WombatForensics::SetupDirModel(void)
{
    wombatdirmodel = new QStandardItemModel();
    QStringList headerList;
    headerList << "Name" << "Unique ID" << "Full Path" << "Size" << "Created" << "MD5 Hash";
    wombatdirmodel->setHorizontalHeaderLabels(headerList);
    QStandardItem *evidenceNode = wombatdirmodel->invisibleRootItem();
    currenttreeview = ui->fileInfoTabWidget->findChild<QTreeView *>("bt-dirtree");
    currenttreeview->setModel(wombatdirmodel);
    ResizeColumns(wombatdirmodel);
    connect(currenttreeview, SIGNAL(clicked(QModelIndex)), this, SLOT(dirTreeView_selectionChanged(QModelIndex)));
    connect(currenttreeview, SIGNAL(expanded(const QModelIndex &)), this, SLOT(ResizeViewColumns(const QModelIndex &)));
}

void WombatForensics::SendFileContents(QString filepath)
{
    ibasictools->LoadFileContents(filepath);
}

WombatForensics::~WombatForensics()
{
    delete ui;
}

void WombatForensics::closeEvent(QCloseEvent* event)
{
    wombatprogresswindow->close();
    RemoveTmpFiles();
    //const char* errmsg = wombatcasedata->CloseCaseDB();
    //fprintf(stderr, "CloseDB: %s\n", errmsg);
}

void WombatForensics::RemoveTmpFiles()
{
    QString homePath = QDir::homePath();
    homePath += "/WombatForensics/";
    homePath += "tmpfiles/";
    QDir tmpdir(homePath);
    if(!tmpdir.removeRecursively())
    {
        DisplayError("2.7", "Tmp File Removal", "All tmp files may not have been removed. Please manually remove them to avoid evidence contamination.");
    }
    fprintf(stderr, "remove dir: %s\n", homePath.toStdString().c_str());
}

void WombatForensics::on_actionNew_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(wombatvariable.caseid > 0)
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        // create new case here
        bool ok;
        QString text = QInputDialog::getText(this, tr("New Case Creation"), "Enter Case Name: ", QLineEdit::Normal, "", &ok);
        if(ok && !text.isEmpty())
        {
            wombatvariable.caseid = wombatcasedata->InsertCase(text);

            QString tmpTitle = "Wombat Forensics - ";
            tmpTitle += text;
            this->setWindowTitle(tmpTitle); // update application window.
            // make Cases Folder
            QString userPath = wombatvariable.casespath;
            userPath += QString::number(wombatvariable.caseid);
            userPath += "-";
            userPath += text;
            userPath += "/";
            bool mkPath = (new QDir())->mkpath(userPath);
            if(mkPath == true)
            {
                wombatvariable.casedirpath = userPath;
            }
            else
            {
                DisplayError("2.0", "Cases Folder Creation Failed.", "New Case folder was not created.");
            }
            userPath = wombatvariable.casedirpath;
            userPath += "evidence/";
            mkPath = (new QDir())->mkpath(userPath);
            if(mkPath == true)
            {
                wombatvariable.evidencedirpath = userPath;
            }
            else
            {
                DisplayError("2.0", "Case Evidence Folder Creation Failed", "Failed to create case evidence folder.");
            }
            if(wombatcasedata->ReturnCaseCount() > 0)
            {
                ui->actionOpen_Case->setEnabled(true);
            }
        }
    }
}

void WombatForensics::on_actionOpen_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(wombatvariable.caseid > 0)
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        // open case here
        QStringList caseList;
        caseList.clear();
        // populate case list here
        caseList = wombatcasedata->ReturnCaseNameList();
        bool ok;
        QString item = QInputDialog::getItem(this, tr("Open Existing Case"), tr("Select the Case to Open: "), caseList, 0, false, &ok);
        if(ok && !item.isEmpty()) // open selected case
        {
            wombatvariable.caseid = wombatcasedata->ReturnCaseID(item);
            QString tmpTitle = "Wombat Forensics - ";
            tmpTitle += item;
            this->setWindowTitle(tmpTitle);
            QString userPath = wombatvariable.casespath;
            userPath += QString::number(wombatvariable.caseid);
            userPath += "-";
            userPath += item;
            userPath += "/";
            bool mkPath = (new QDir())->mkpath(userPath);
            if(mkPath == true)
            {
                wombatvariable.casedirpath = userPath;
            }
            else
            {
                DisplayError("2.0", "Cases Folder Check Failed.", "Existing Case folder did not exist.");
            }
            userPath = wombatvariable.casedirpath;
            userPath += "evidence/";
            mkPath = (new QDir())->mkpath(userPath);
            if(mkPath == true)
            {
                wombatvariable.evidencedirpath = userPath;
            }
            else
            {
                DisplayError("2.0", "Case Evidence Folder Check Failed", "Case Evidence folder did not exist.");
            }
            ThreadRunner* trun = new ThreadRunner(isleuthkit, "populatecase", wombatvariable);
            threadpool->start(trun);
        }
    }
}

void WombatForensics::on_actionView_Progress_triggered(bool checked)
{
    if(!checked)
        wombatprogresswindow->hide();
    else
        wombatprogresswindow->show();
}

void WombatForensics::dirTreeView_selectionChanged(const QModelIndex &index)
{
    QString tmptext = "";
    curselindex = index;
    tmptext = index.sibling(index.row(), 1).data().toString();
    fprintf(stderr, "unique id:'%s'\n", tmptext.toStdString().c_str());
    if(tmptext != "")
    {
        wombatvariable.evidenceid = wombatcasedata->ReturnObjectEvidenceID(tmptext.toInt());
        QStringList currentevidencelist = wombatcasedata->ReturnEvidenceData(wombatvariable.evidenceid);
        wombatvariable.evidencepath = currentevidencelist[0];
        wombatvariable.evidencedbname = currentevidencelist[1];
        wombatvariable.fileid = wombatcasedata->ReturnObjectFileID(tmptext.toInt());
        ThreadRunner* tmprun = new ThreadRunner(isleuthkit, "showfile", wombatvariable);
        threadpool->start(tmprun);
    }
}
