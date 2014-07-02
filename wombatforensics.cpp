#include "wombatforensics.h"

WombatForensics::WombatForensics(QWidget *parent) : QMainWindow(parent), ui(new Ui::WombatForensics)
{
    ui->setupUi(this);
    threadpool = QThreadPool::globalInstance();
    wombatvarptr = &wombatvariable;
    this->menuBar()->hide();
    this->statusBar()->setSizeGripEnabled(true);
    selectedoffset = new QLabel(this);
    selectedoffset->setText("Offset: 00");
    selectedhex = new QLabel(this);
    selectedhex->setText("Length: 0");
    filtercountlabel = new QLabel(this);
    filtercountlabel->setText("Filtered: 0");
    filecountlabel = new QLabel(this);
    filecountlabel->setText("Files: 0");
    processcountlabel = new QLabel(this);
    processcountlabel->setText("Processed: 0");
    statuslabel = new QLabel(this);
    statuslabel->setText("");
    vline1 = new QFrame(this);
    vline1->setFrameStyle(QFrame::VLine | QFrame::Raised);
    vline1->setLineWidth(1);
    vline1->setMidLineWidth(0);
    vline2 = new QFrame(this);
    vline2->setFrameStyle(QFrame::VLine | QFrame::Raised);
    vline2->setLineWidth(1);
    vline2->setMidLineWidth(0);
    this->statusBar()->addWidget(selectedoffset, 0);
    this->statusBar()->addWidget(selectedhex, 0);
    this->statusBar()->addWidget(vline1, 0);
    this->statusBar()->addWidget(filtercountlabel, 0);
    this->statusBar()->addWidget(filecountlabel, 0);
    this->statusBar()->addWidget(processcountlabel, 0);
    this->statusBar()->addPermanentWidget(vline2, 0);
    this->statusBar()->addPermanentWidget(statuslabel, 0);
    tskobjptr = &tskobject;
    tskobjptr->readimginfo = NULL;
    tskobjptr->readfsinfo = NULL;
    tskobjptr->readfileinfo = NULL;
    wombatdatabase = new WombatDatabase(wombatvarptr);
    wombatframework = new WombatFramework(wombatvarptr);
    propertywindow = new PropertiesWindow(wombatdatabase);
    isignals = new InterfaceSignals();
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(LoadComplete(bool)));
    connect(ui->actionView_Properties, SIGNAL(triggered(bool)), this, SLOT(on_actionView_Properties_triggered(bool)), Qt::DirectConnection);
    connect(propertywindow, SIGNAL(HidePropertyWindow(bool)), this, SLOT(HidePropertyWindow(bool)), Qt::DirectConnection);
    connect(isignals, SIGNAL(ProgressUpdate(int, int)), this, SLOT(UpdateProgress(int, int)), Qt::QueuedConnection);
    wombatvarptr->caseobject.id = 0;
    wombatvarptr->omnivalue = 1; // web view is default omniviewer view to display
    connect(wombatdatabase, SIGNAL(DisplayError(QString, QString, QString)), this, SLOT(DisplayError(QString, QString, QString)), Qt::DirectConnection);
    propertywindow->setModal(false);
    InitializeAppStructure();
    connect(&sqlwatcher, SIGNAL(finished()), this, SLOT(InitializeQueryModel()), Qt::QueuedConnection);
    connect(&remwatcher, SIGNAL(finished()), this, SLOT(FinishRemoval()), Qt::QueuedConnection);

    treemodel = new TreeModel(this);
    ui->dirTreeView->setModel(treemodel);
    ui->dirTreeView->hideColumn(4);
    ui->dirTreeView->hideColumn(5);
    ui->dirTreeView->hideColumn(11);
    ui->dirTreeView->hideColumn(12);
    ui->dirTreeView->hideColumn(13);
    ui->dirTreeView->hideColumn(14);
    connect(ui->dirTreeView, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(ExpandCollapseResize(const QModelIndex &)));
    connect(ui->dirTreeView, SIGNAL(expanded(const QModelIndex &)), this, SLOT(ExpandCollapseResize(const QModelIndex &)));
    connect(ui->dirTreeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(SelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void WombatForensics::HidePropertyWindow(bool checkedstate)
{
    qDebug() << "set propertybutton to false";
    ui->actionView_Properties->setChecked(checkedstate);
}

void WombatForensics::HideProgressWindow(bool checkedstate)
{
    ui->actionView_Progress->setChecked(checkedstate);
}

void WombatForensics::InitializeAppStructure()
{
    QString homePath = QDir::homePath();
    homePath += "/WombatForensics/";
    wombatvarptr->settingspath = homePath + "settings";
    wombatvarptr->datapath = homePath + "data/";
    wombatvarptr->casespath = homePath + "cases/";
    //wombatvarptr->tmpfilepath = homePath + "tmpfiles"; // currently not used if i continue to view objects in memory...
    bool mkPath = (new QDir())->mkpath(wombatvarptr->settingspath);
    if(mkPath == false)
        DisplayError("2.0", "App Settings Folder Failed.", "App Settings Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatvarptr->datapath);
    if(mkPath == false)
        DisplayError("2.1", "App Data Folder Failed.", "Application Data Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatvarptr->casespath);
    if(mkPath == false)
        DisplayError("2.2", "App Cases Folder Failed.", "App Cases Folder was not created.");
    //mkPath = (new QDir())->mkpath(wombatvarptr->tmpfilepath);
    //if(mkPath == false)
    //    DisplayError("2.2", "App TmpFile Folder Failed.", "App TmpFile Folder was not created.");
    wombatvarptr->wombatdbname = wombatvarptr->datapath + "WombatApp.db";
    wombatvarptr->appdb = QSqlDatabase::addDatabase("QSQLITE", "wombatapp");
    wombatvarptr->appdb.setDatabaseName(wombatvarptr->wombatdbname);
    bool appFileExist = FileExists(wombatvarptr->wombatdbname.toStdString());
    if(!appFileExist)
    {
        wombatdatabase->CreateAppDB();
        if(wombatvarptr->curerrmsg.compare("") != 0)
            DisplayError("1.0", "App File Error", wombatvarptr->curerrmsg);
    }
    else
    {
        wombatdatabase->OpenAppDB();
        if(wombatvarptr->curerrmsg.compare("") != 0)
            DisplayError("1.1", "SQL", wombatvarptr->curerrmsg);
    }
    if(wombatdatabase->ReturnCaseCount() == 0)
    {
        ui->actionOpen_Case->setEnabled(false);
    }
    else if(wombatdatabase->ReturnCaseCount() > 0)
    {
        ui->actionOpen_Case->setEnabled(true);
    }
    else
    {
        DisplayError("1.0", "Case Count", "Invalid Case Count returned.");
    }
    ui->actionAdd_Evidence->setEnabled(false);
    ui->actionRemove_Evidence->setEnabled(false);
    ui->actionViewTxt->setEnabled(false);
    ui->actionViewOmni->setEnabled(false);
    ui->actionView_Progress->setEnabled(false);
    ui->actionView_Properties->setEnabled(false);
    ui->actionExport_Evidence->setEnabled(false);
    QList<int> sizelist;
    sizelist.append(height()/2);
    sizelist.append(height()/2);
    ui->splitter->setSizes(sizelist);
    SetupHexPage();
    SetupToolbar();
}

void WombatForensics::InitializeCaseStructure()
{
    // create new case here
    bool ok;
    wombatvarptr->caseobject.name = QInputDialog::getText(this, tr("New Case Creation"), "Enter Case Name: ", QLineEdit::Normal, "", &ok);
    if(ok && !wombatvarptr->caseobject.name.isEmpty())
    {
        wombatdatabase->InsertCase();
        QString tmpTitle = "Wombat Forensics - ";
        tmpTitle += wombatvarptr->caseobject.name;
        this->setWindowTitle(tmpTitle); // update application window.
        // make Cases Folder
        QString casestring = QString::number(wombatvarptr->caseobject.id) + "-" + wombatvarptr->caseobject.name;
        wombatvarptr->caseobject.dirpath = wombatvarptr->casespath + casestring + "/";
        bool mkPath = (new QDir())->mkpath(wombatvarptr->caseobject.dirpath);
        if(mkPath == false)
        {
            DisplayError("2.0", "Cases Folder Creation Failed.", "New Case folder was not created.");
        }
        // CREATE CASEID-CASENAME.DB RIGHT HERE.
        wombatvarptr->caseobject.dbname = wombatvarptr->caseobject.dirpath + casestring + ".db";
        wombatvarptr->casedb = QSqlDatabase::addDatabase("QSQLITE", "casedb"); // may not need this
        wombatvarptr->casedb.setDatabaseName(wombatvarptr->caseobject.dbname);
        if(!FileExists(wombatvarptr->caseobject.dbname.toStdString()))
        {
            wombatdatabase->CreateCaseDB();
            if(wombatvarptr->curerrmsg.compare("") != 0)
                DisplayError("1.2", "Course DB Creation Error", wombatvarptr->curerrmsg);
        }
        else
        {
            wombatdatabase->OpenCaseDB();
            if(wombatvarptr->curerrmsg.compare("") != 0)
                DisplayError("1.3", "SQL", wombatvarptr->curerrmsg);
        }
        fcasedb = wombatvarptr->casedb;
        if(wombatdatabase->ReturnCaseCount() > 0)
        {
            ui->actionOpen_Case->setEnabled(true);
        }
        ui->actionAdd_Evidence->setEnabled(true);
    }
}

void WombatForensics::InitializeOpenCase()
{
    // open case here
    wombatvarptr->casenamelist.clear();
    // populate case list here
    wombatdatabase->ReturnCaseNameList();
    bool ok;
    wombatvarptr->caseobject.name = QInputDialog::getItem(this, tr("Open Existing Case"), tr("Select the Case to Open: "), wombatvarptr->casenamelist, 0, false, &ok);
    if(ok && !wombatvarptr->caseobject.name.isEmpty()) // open selected case
    {
        wombatdatabase->ReturnCaseID();
        QString tmpTitle = "Wombat Forensics - ";
        tmpTitle += wombatvarptr->caseobject.name;
        this->setWindowTitle(tmpTitle);
        QString casestring = QString::number(wombatvarptr->caseobject.id) + "-" + wombatvarptr->caseobject.name;
        wombatvarptr->caseobject.dirpath = wombatvarptr->casespath + casestring + "/";
        bool mkPath = (new QDir())->mkpath(wombatvarptr->caseobject.dirpath);
        if(mkPath == false)
        {
            DisplayError("2.0", "Cases Folder Check Failed.", "Existing Case folder did not exist.");
        }
        // CREATE CASEID-CASENAME.DB RIGHT HERE.
        wombatvarptr->caseobject.dbname = wombatvarptr->caseobject.dirpath + casestring + ".db";
        wombatvarptr->casedb = QSqlDatabase::addDatabase("QSQLITE", "casedb"); // may not need this
        wombatvarptr->casedb.setDatabaseName(wombatvarptr->caseobject.dbname);
        bool caseFileExist = FileExists(wombatvarptr->caseobject.dbname.toStdString());
        if(!caseFileExist)
        {
            wombatdatabase->CreateCaseDB();
            if(wombatvarptr->curerrmsg.compare("") != 0)
                DisplayError("1.2", "Course DB Creation Error", wombatvarptr->curerrmsg);
        }
        else
        {
            wombatdatabase->OpenCaseDB();
            if(wombatvarptr->curerrmsg.compare("") != 0)
                DisplayError("1.3", "SQL", wombatvarptr->curerrmsg);
        }
        fcasedb = wombatvarptr->casedb;
        ui->actionAdd_Evidence->setEnabled(true);
        processcountlabel->setText("Processed: 0");
        filecountlabel->setText("Files: 0");
        wombatdatabase->GetEvidenceObjects();

        for(int i=0; i < wombatvarptr->evidenceobjectvector.count(); i++)
        {
            wombatvarptr->currentevidencename = QString::fromStdString(wombatvarptr->evidenceobjectvector.at(i).fullpathvector[0]).split("/").last(); 
            currentevidencename = wombatvarptr->currentevidencename;
            wombatdatabase->ReturnFileSystemObjectList(wombatvarptr->evidenceobjectvector.at(i).id);
            wombatvarptr->currentevidenceid = wombatvarptr->evidenceobjectvector.at(i).id;
            wombatvarptr->evidenceobject = wombatvarptr->evidenceobjectvector.at(i);
            statuslabel->setText("Processed 0%");
            OpenEvidenceStructure();
            if(ui->dirTreeView->model() != NULL)
                ui->actionRemove_Evidence->setEnabled(true);
        }
    }

}

void WombatForensics::InitializeQueryModel()
{
    fcasedb.commit();
    if(ProcessingComplete())
    {
        statuslabel->setText("Processing Complete");
        qDebug() << "All threads have finished.";
        fcasedb.commit();
        qDebug() << "DB Commit finished.";
        treemodel->AddEvidence(wombatvarptr->currentevidenceid);
        ResizeColumns();
        ui->actionRemove_Evidence->setEnabled(true);
        //statuslabel->setText("");
        wombatframework->CloseInfoStructures();
    }
}

void WombatForensics::SelectionChanged(const QItemSelection &curitem, const QItemSelection &previtem)
{
    if(previtem.indexes().count() > 0)
        oldselectedindex = previtem.indexes().at(0);
    selectedindex = curitem.indexes().at(0);
    ui->actionView_Properties->setEnabled(true);
    ui->actionExport_Evidence->setEnabled(true);
    wombatvarptr->selectedobject.id = selectedindex.sibling(selectedindex.row(), 0).data().toInt(); // object id
    wombatvarptr->selectedobject.name = selectedindex.sibling(selectedindex.row(), 1).data().toString(); // object name
    wombatdatabase->GetObjectValues(); // now i have selectedobject.values.
    UpdateOmniValue();
    UpdateViewer();
    if(propertywindow->isVisible())
        UpdateProperties();
}

/*
// FUNCTION GETS IMPLEMNTED WHEN YOU CLICK ON A CHECKBOX, BUT DO NOT SELECT THE ROW
void WombatForensics::CurrentChanged(const QModelIndex &curindex, const QModelIndex &previndex)
{
    qDebug() << "current index changed.";
    //dirTreeView_selectionChanged(curindex);
}
*/

void WombatForensics::InitializeEvidenceStructure()
{
    wombatframework->OpenEvidenceImage();
    wombatdatabase->InsertEvidenceObject(); // add evidence to data and image parts to dataruns
    wombatframework->OpenVolumeSystem();
    wombatframework->GetVolumeSystemName();
    wombatdatabase->InsertVolumeObject(); // add volume to data
    wombatframework->OpenPartitions();
    wombatdatabase->InsertPartitionObjects();
    wombatdatabase->InsertFileSystemObjects();
    wombatdatabase->ReturnFileSystemObjectList(wombatvarptr->currentevidenceid);
    wombatframework->OpenFiles();
}

void WombatForensics::OpenEvidenceStructure()
{
    treemodel->AddEvidence(wombatvarptr->currentevidenceid);
}

void WombatForensics::AddEvidence()
{
    int isnew = 1;
    threadvector.clear();
    wombatdatabase->GetEvidenceObjects();
    QStringList tmplist = QFileDialog::getOpenFileNames(this, tr("Select Evidence Image(s)"), tr("./"));
    if(tmplist.count())
    {
        wombatvarptr->currentevidencename = tmplist[0].split("/").last();
        for(int i=0; i < wombatvarptr->evidenceobjectvector.count(); i++)
        {
            if(wombatvarptr->evidenceobjectvector.at(i).fullpathvector[0].compare(tmplist[0].toStdString()) == 0)
                isnew = 0;
        }
        if(isnew == 1)
        {
            currentevidencename = wombatvarptr->currentevidencename;
            for(int i=0; i < tmplist.count(); i++)
            {
                wombatvarptr->evidenceobject.fullpathvector.push_back(tmplist[i].toStdString());
            }
            wombatvarptr->evidenceobject.itemcount = tmplist.count();
            processcountlabel->setText("Processed: 0");
            filecountlabel->setText("Files: 0");
            statuslabel->setText("Processed 0%");
            // THIS SHOULD HANDLE WHEN THE THREADS ARE ALL DONE.

            sqlfuture = QtConcurrent::run(this, &WombatForensics::InitializeEvidenceStructure);
            sqlwatcher.setFuture(sqlfuture);
            threadvector.append(sqlfuture);
        }
        else
            DisplayError("1.8", "Evidence already exists in the case.", "Add Evidence cancelled");
    }
}

void WombatForensics::UpdateProperties()
{
    // get data here...
    // wombatvarptr->selectedobject.
    wombatdatabase->ReturnObjectPropertyList();
    propertywindow->UpdateTableView();
    qDebug() << "window is visible. properties get updated here.";
}

void WombatForensics::UpdateViewer()
{
    if(ui->dirTreeView->model() != NULL)
    {
        wombatvarptr->visibleviewer = ReturnVisibleViewerID();
        if(wombatvarptr->visibleviewer == 0) // hex
            LoadHexContents();
        else if(wombatvarptr->visibleviewer == 1) // txt
            LoadTxtContents();
        else if(wombatvarptr->visibleviewer == 2) // web
            LoadWebContents();
        else if(wombatvarptr->visibleviewer == 3) // pic
            LoadImgContents();
        else if(wombatvarptr->visibleviewer == 4) // vid
            LoadVidContents();
    }
}

void WombatForensics::LoadHexContents()
{
    if(tskobjptr->readimginfo != NULL)
        tsk_img_close(tskobjptr->readimginfo);
    if(tskobjptr->readfsinfo != NULL)
        tsk_fs_close(tskobjptr->readfsinfo);
    if(tskobjptr->readfileinfo != NULL)
        tsk_fs_file_close(tskobjptr->readfileinfo);

    if(wombatvarptr->selectedobject.objtype == 1) // image file
    {
        OpenParentImage(wombatvarptr->selectedobject.id);
        tskobjptr->offset = 0;
        tskobjptr->objecttype = 1;
        tskobjptr->length = wombatvarptr->selectedobject.size;
    }
    else if(wombatvarptr->selectedobject.objtype == 2) // volume object
    {
        OpenParentImage(wombatvarptr->selectedobject.parimgid);
        tskobjptr->objecttype = 3;
        tskobjptr->offset = wombatvarptr->selectedobject.sectstart * wombatvarptr->selectedobject.blocksize;
        tskobjptr->length = wombatvarptr->selectedobject.size;
    }
    else if(wombatvarptr->selectedobject.objtype == 4) // fs object
    {
        OpenParentImage(wombatvarptr->selectedobject.parimgid);
        tskobjptr->offset = wombatvarptr->selectedobject.byteoffset;
        tskobjptr->objecttype = 4;
        tskobjptr->length = wombatvarptr->selectedobject.size;
    }
    else if(wombatvarptr->selectedobject.objtype == 5) // file object
    {
        OpenParentImage(wombatvarptr->selectedobject.parimgid);
        OpenParentFileSystem();
        tskobjptr->offset = 0; 
        tskobjptr->objecttype = 5;
        tskobjptr->address = wombatvarptr->selectedobject.address;
        tskobjptr->length = wombatvarptr->selectedobject.size;
        OpenFileSystemFile();
    }
    if(wombatvarptr->selectedobject.objtype != 3)
    {
        hexwidget->openimage();
        hexwidget->set2BPC();
        hexwidget->setBaseHex();
    }
}

void WombatForensics::LoadTxtContents()
{
}

void WombatForensics::LoadWebContents()
{
    if(wombatvarptr->selectedobject.objtype == 1)
    {
        ui->webView->setUrl(QUrl("qrc:///html/infohtml"));
    }
}

void WombatForensics::LoadImgContents()
{
}

void WombatForensics::LoadVidContents()
{
}

void WombatForensics::LoadComplete(bool isok)
{
    wombatvarptr->htmlcontent = "";
    int curidx = 0;
    //int curidx = wombatframework->DetermineVectorIndex();
    if(isok && curidx > -1)
    {
        if(wombatvarptr->selectedobject.objtype == 1) // image file
        {
            wombatvarptr->htmlcontent += "<div id='infotitle'>image information</div><br/>";
            wombatvarptr->htmlcontent += "<table><tr><td class='property'>imagetype:</td><td class='pvalue'>";
            wombatvarptr->htmlcontent += QString(tsk_img_type_todesc((TSK_IMG_TYPE_ENUM)wombatvarptr->selectedobject.type)) + "</td></tr>";
            wombatvarptr->htmlcontent += "<tr><td class='property'>size:</td><td class='pvalue'>";
            wombatvarptr->htmlcontent += QLocale::system().toString((int)wombatvarptr->selectedobject.size) + " bytes</td></tr>";
            wombatvarptr->htmlcontent += "<tr><td class='property'>sector size:</td><td class='pvalue'>";
            wombatvarptr->htmlcontent += QLocale::system().toString(wombatvarptr->selectedobject.sectlength) + " bytes</td></tr>";
            wombatvarptr->htmlcontent += "<tr><td class='property'>sector count:</td><td class='pvalue'>";
            wombatvarptr->htmlcontent += QLocale::system().toString((int)((float)wombatvarptr->selectedobject.size/(float)wombatvarptr->selectedobject.sectlength));
            wombatvarptr->htmlcontent += " sectors</td></tr>";
            // might not want to do the volume type one if there's no volume. have to think on it.
            //wombatvarptr->htmlcontent += " sectors</td></tr><tr><td class='property'>volume type</td><td class='pvalue'>";
            //wombatvarptr->htmlcontent += wombatvarptr->volumeobject.name + "</td></tr>";
            wombatframework->GetBootCode(curidx); // determine boot type in this function and populate html string information into wombatvarptr value
            QWebElement tmpelement = ui->webView->page()->currentFrame()->documentElement().lastChild();
            tmpelement.appendInside(wombatvarptr->htmlcontent);

                // check for partition table and populate the values accordingly.
                // the fs stuff i find at jump and oem and the others are for the filesystem/partition boot sector. this isn't valid when there is an mbr.
                // need to determine if there is an mbr and then pull the partition table information from it. otherwise simply display the image info
                // and have no mbr present in first sector.
                // when you click on the partition, this is where the partition boot sector information will go.
                //tmpelement.appendInside("<br/><br/><div class='tabletitle'>boot sector</div>");
                //tmpelement.appendInside("<br/><table><tr><th>byte offset</th><th>value</th><th>description</th></tr><tr class='odd'><td>0-2</td><td class='bvalue'>" + wombatvarptr->bootsectorlist[0] + "</td><td class='desc'>Jump instruction to the boot code</td></tr><tr class='even'><td>3-10</td><td class='bvalue'>" + wombatvarptr->bootsectorlist[1] + "</td><td class='desc'>OEM name string field. This field is ignored by Microsoft operating systems</td></tr><tr class='odd'><td>11-12</td><td class='bvalue'>" + wombatvarptr->bootsectorlist[2] + " bytes</td><td class='desc'>Bytes per sector</td></tr><tr class='even'><td>13-13</td><td class='bvalue'>" + wombatvarptr->bootsectorlist[3] + " sectors</td><td class='desc'>Seectors per cluster</td></tr><tr class='odd'><td colspan='3' class='bot'></td></tr></table>");
        }
        else if(wombatvarptr->selectedobject.objtype == 2) // volume file (it should never be a volume since i don't add it to the image tree)
        {
        }
        else if(wombatvarptr->selectedobject.objtype == 3) // partition file
        {
        }
        else if(wombatvarptr->selectedobject.objtype == 4) // file system file
        {
        }
        else // implement for files, directories etc.. as i go.
        {
        }
    }
}

void WombatForensics::OpenParentImage(int imgid)
{
    wombatdatabase->GetEvidenceObjects();
    int curidx = 0;
    for(int i=0; i < wombatvarptr->evidenceobjectvector.count(); i++)
    {
        if(imgid == wombatvarptr->evidenceobjectvector[i].id)
            curidx = i;
    }
    tskobjptr->imagepartspath = (const char**)malloc(wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size()*sizeof(char*));
    tskobjptr->partcount = wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size();
    for(uint i=0; i < wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size(); i++)
    {
        tskobjptr->imagepartspath[i] = wombatvarptr->evidenceobjectvector[curidx].fullpathvector[i].c_str();
    }
    tskobjptr->readimginfo = tsk_img_open(tskobjptr->partcount, tskobjptr->imagepartspath, TSK_IMG_TYPE_DETECT, 0);
    if(tskobjptr->readimginfo == NULL)
    {
        //qDebug() << "print image error here";
    }
    free(tskobjptr->imagepartspath);
}

void WombatForensics::OpenParentFileSystem()
{
    tskobjptr->readfsinfo = tsk_fs_open_img(tskobjptr->readimginfo, 0, TSK_FS_TYPE_DETECT);
}

void WombatForensics::OpenFileSystemFile()
{
    tskobjptr->readfileinfo = tsk_fs_file_open_meta(tskobjptr->readfsinfo, NULL, tskobjptr->address);
}

void WombatForensics::RemEvidence()
{
    threadvector.clear();
    wombatvarptr->evidencenamelist.clear();
    wombatdatabase->ReturnEvidenceNameList();
    bool ok;
    wombatvarptr->evidremovestring = QInputDialog::getItem(this, tr("Remove Existing Evidence"), tr("Select the Evidence to Remove: "), wombatvarptr->evidencenamelist, 0, false, &ok);
    if(ok && !wombatvarptr->evidremovestring.isEmpty()) // remove selected evidence
    {
        wombatvarptr->evidremoveid = wombatvarptr->evidremovestring.split(".").at(0).toInt();
        if(wombatvarptr->evidremoveid > 0)
        {
            treemodel->RemEvidence(wombatvarptr->evidremoveid);
            remfuture = QtConcurrent::run(wombatdatabase, &WombatDatabase::RemoveEvidence);
            remwatcher.setFuture(remfuture);
            threadvector.append(remfuture);
 
        }
    }
}

void WombatForensics::GetExportData(Node* curnode, FileExportData* exportdata)
{
    if(curnode->nodevalues.at(4).toInt() == 5)
    {
        QVariant tmpvariant;
        if(exportdata->filestatus == FileExportData::checked)
        {
            if(curnode->checkstate == 2)
            {
                TskObject tmpobj;
                tmpobj.address = curnode->nodevalues.at(5).toInt();
                tmpobj.length = curnode->nodevalues.at(3).toInt();
                tmpobj.type = curnode->nodevalues.at(12).toInt();
                tmpobj.objecttype = 5;
                tmpobj.offset = 0;
                tmpobj.readimginfo = NULL;
                tmpobj.readfsinfo = NULL;
                tmpobj.readfileinfo = NULL;
                curlist.append(tmpobj);
                exportdata->exportcount = totalchecked;
                exportdata->id = curnode->nodevalues.at(0).toInt();
                exportdata->name = curnode->nodevalues.at(1).toString().toStdString();
                exportdata->fullpath = exportdata->exportpath;
                exportdata->fullpath += "/";
                exportdata->fullpath += currentevidencename.toStdString();
                exportdata->fullpath += "/";
                if(exportdata->pathstatus == FileExportData::include)
                    exportdata->fullpath += curnode->nodevalues.at(2).toString().toStdString();
                exportdata->fullpath += "/";
                exportfilelist.push_back(*exportdata);
            }
        }
        else
        {
            TskObject tmpobj;
            tmpobj.address = curnode->nodevalues.at(5).toInt();
            tmpobj.length = curnode->nodevalues.at(3).toInt();
            tmpobj.type = curnode->nodevalues.at(12).toInt();
            tmpobj.objecttype = 5;
            tmpobj.offset = 0;
            tmpobj.readimginfo = NULL;
            tmpobj.readfsinfo = NULL;
            tmpobj.readfileinfo = NULL;
            curlist.append(tmpobj);
            exportdata->exportcount = totalchecked;
            exportdata->id = curnode->nodevalues.at(0).toInt();
            exportdata->name = curnode->nodevalues.at(1).toString().toStdString();
            exportdata->fullpath = exportdata->exportpath;
            exportdata->fullpath += "/";
            exportdata->fullpath += currentevidencename.toStdString();
            exportdata->fullpath += "/";
            if(exportdata->pathstatus == FileExportData::include)
                exportdata->fullpath += curnode->nodevalues.at(2).toString().toStdString();
            exportdata->fullpath += "/";
            exportfilelist.push_back(*exportdata);
        }
    }
    if(curnode->haschildren)
    {
        for(int i=0; i < curnode->children.count(); i++)
            GetExportData(curnode->children[i], exportdata);
    }
};

void WombatForensics::ExportEvidence()
{
    totalcount = 0;
    totalchecked = 0;
    exportcount = 0;
    ((TreeModel*)ui->dirTreeView->model())->GetModelCount(rootnode);
    exportdialog = new ExportDialog(this, totalchecked, totalcount);
    connect(exportdialog, SIGNAL(FileExport(FileExportData*)), this, SLOT(FileExport(FileExportData*)), Qt::DirectConnection);
    exportdialog->show();
}

void WombatForensics::FileExport(FileExportData* exportdata)
{
    exportfuture = QtConcurrent::run(this, &WombatForensics::ExportFiles, exportdata);
}

void WombatForensics::FinishRemoval()
{
    if(ProcessingComplete())
    {
        filesprocessed = filesprocessed - wombatvarptr->evidrowsremoved;
        filesfound = filesfound - wombatvarptr->evidrowsremoved;
        processcountlabel->setText("Processed: " + QString::number(filesprocessed));
        filecountlabel->setText("Files: " + QString::number(filesfound));
        statuslabel->setText("Evidence Removal of " + QString::number(wombatvarptr->evidrowsremoved) + " completed.");
        //statuslabel->setText("");

        qDebug() << "removal of files is complete.";
    }
    else
    {
        qDebug() << "still removing files.";
    }
}

void WombatForensics::FinishExport()
{
    if(ProcessingComplete())
    {
        statuslabel->setText("Exporting completed");
        //statuslabel->setText("");
        qDebug() << "export of files is complete.";
    }
    else
    {
        qDebug() << "still exporting files.";
    }
}

void WombatForensics::ExportFiles(FileExportData* exportdata)
{
    threadvector.clear();
    exportfilelist.clear();
    curlist.clear();
    if(exportdata->filestatus == FileExportData::selected)
    {
        exportdata->exportcount = 1;
        exportdata->id = wombatvarptr->selectedobject.id;
        exportdata->name = wombatvarptr->selectedobject.name.toStdString();
        exportdata->fullpath = exportdata->exportpath;
        exportdata->fullpath += "/";
        exportdata->fullpath += currentevidencename.toStdString();
        exportdata->fullpath += "/";
        if(exportdata->pathstatus == FileExportData::include)
            exportdata->fullpath += selectedindex.sibling(selectedindex.row(), 2).data().toString().toStdString();
        exportdata->fullpath += "/";
        exportfilelist.push_back(*exportdata);
        TskObject tmpobj;
        tmpobj.address = selectedindex.sibling(selectedindex.row(), 5).data().toInt();
        tmpobj.offset = 0;
        tmpobj.length = selectedindex.sibling(selectedindex.row(), 3).data().toInt();
        tmpobj.type = selectedindex.sibling(selectedindex.row(), 12).data().toInt();
        tmpobj.objecttype = 5;
        tmpobj.readimginfo = NULL;
        tmpobj.readfsinfo = NULL;
        tmpobj.readfileinfo = NULL;
        curlist.append(tmpobj);
    }
    else
        GetExportData(rootnode, exportdata);
    int curprogress = (int)((((float)exportcount)/(float)curlist.count())*100);
    statuslabel->setText("Exported: " + QString::number(exportcount) + " of " + QString::number(curlist.count()) + " " + QString::number(curprogress) + "%");
    for(int i=0; i < curlist.count(); i++)
    {
        QFuture<void> tmpfuture = QtConcurrent::run(this, &WombatForensics::ProcessExport, curlist.at(i), exportfilelist[i].fullpath, exportfilelist[i].name);
        exportwatcher.setFuture(tmpfuture);
        threadvector.append(tmpfuture);
    }
}

void WombatForensics::ProcessExport(TskObject curobj, std::string fullpath, std::string name)
{
    if(curobj.readimginfo != NULL)
        tsk_img_close(curobj.readimginfo);
    if(curobj.readfsinfo != NULL)
        tsk_fs_close(curobj.readfsinfo);
    if(curobj.readfileinfo != NULL)
        tsk_fs_file_close(curobj.readfileinfo);

    int curidx = 0;
    for(int i=0; i < wombatvarptr->evidenceobjectvector.count(); i++)
    {
        if(wombatvarptr->currentevidenceid == wombatvarptr->evidenceobjectvector[i].id)
            curidx = i;
    }
    curobj.imagepartspath = (const char**)malloc(wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size()*sizeof(char*));
    curobj.partcount = wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size();
    for(uint i=0; i < wombatvarptr->evidenceobjectvector[curidx].fullpathvector.size(); i++)
    {
        curobj.imagepartspath[i] = wombatvarptr->evidenceobjectvector[curidx].fullpathvector[i].c_str();
    }
    curobj.readimginfo = tsk_img_open(curobj.partcount, curobj.imagepartspath, TSK_IMG_TYPE_DETECT, 0);
    if(curobj.readimginfo == NULL)
    {
        //qDebug() << "print image error here";
    }
    free(curobj.imagepartspath);


    curobj.readfsinfo = tsk_fs_open_img(curobj.readimginfo, 0, TSK_FS_TYPE_DETECT);
    curobj.readfileinfo = tsk_fs_file_open_meta(curobj.readfsinfo, NULL, curobj.address);
    if(curobj.type == 3) // directory
    {
        bool tmpdir = (new QDir())->mkpath(QString::fromStdString(fullpath));
        if(!tmpdir)
            qDebug() << "creation of export dirtree for file: " << QString::fromStdString(name) << "failed.";
    }
    else
    {
        int retval = 0;
        char* contentbuffer = new char[curobj.length];
        retval = tsk_fs_file_read(curobj.readfileinfo, curobj.offset, contentbuffer, curobj.length, TSK_FS_FILE_READ_FLAG_SLACK);
        if(retval > 0)
        {
            bool tmpdir = (new QDir())->mkpath(QDir::cleanPath(QString::fromStdString(fullpath)));
            if(tmpdir == true)
            {
                std::string filepath = fullpath + "/" + name;
                QFile tmpfile(QString::fromStdString(filepath));
                if(tmpfile.open(QIODevice::WriteOnly))
                {
                    QDataStream outbuffer(&tmpfile);
                    outbuffer.writeRawData(contentbuffer, curobj.length);
                    tmpfile.close();
                }
            }
        }
    }
    exportcount++;
    int curprogress = (int)((((float)exportcount)/(float)curlist.count())*100);
    StatusUpdate(QString("Exported " + QString::number(exportcount) + " of " + QString::number(curlist.count()) + " " + QString::number(curprogress) + "%"));
}

void WombatForensics::UpdateProgress(int filecount, int processcount)
{
    //qDebug() << "Global Class Called This to AutoUpdate!!!";
    int curprogress = (int)((((float)processcount)/(float)filecount)*100);
    processcountlabel->setText("Processed: " + QString::number(filesprocessed));
    filecountlabel->setText("Files: " + QString::number(filesfound));
    statuslabel->setText("Processed: " + QString::number(curprogress) + "%");
    if(curprogress == 100 && ProcessingComplete())
    {
        statuslabel->setText("Processing Complete");
        //statuslabel->setText("");
    }
}

void WombatForensics::DisplayError(QString errorNumber, QString errorType, QString errorValue)
{
    QString tmpString = errorNumber;
    tmpString += ". Error: ";
    tmpString += errorType;
    tmpString += " Returned ";
    tmpString += errorValue;
    QMessageBox::warning(this, "Error", tmpString, QMessageBox::Ok);
}

void WombatForensics::ResizeColumns(void)
{
    for(int i=0; i < ((TreeModel*)ui->dirTreeView->model())->columnCount(QModelIndex()); i++)
    {
        ui->dirTreeView->resizeColumnToContents(i);
    }
}

void WombatForensics::SetupHexPage(void)
{
    // hex editor page
    QBoxLayout* mainlayout = new QBoxLayout(QBoxLayout::TopToBottom, ui->hexPage);
    QHBoxLayout* hexLayout = new QHBoxLayout();
    hexwidget = new HexEditor(ui->hexPage, tskobjptr);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    hexwidget->setObjectName("bt-hexview");
    hexwidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    hexLayout->addWidget(hexwidget);
    hexvsb = new QScrollBar(hexwidget);
    hexLayout->addWidget(hexvsb);
    hexvsb->setRange(0, 0);
    mainlayout->addLayout(hexLayout);

    connect(hexwidget, SIGNAL(rangeChanged(off_t,off_t)), this, SLOT(setScrollBarRange(off_t,off_t)));
    connect(hexwidget, SIGNAL(topLeftChanged(off_t)), this, SLOT(setScrollBarValue(off_t)));
    connect(hexwidget, SIGNAL(offsetChanged(off_t)), this, SLOT(setOffsetLabel(off_t)));
    connect(hexvsb, SIGNAL(valueChanged(int)), hexwidget, SLOT(setTopLeftToPercent(int)));
    connect(hexwidget, SIGNAL(selectionChanged(const QString &)), this, SLOT(UpdateSelectValue(const QString&)));
}

void WombatForensics::SetupToolbar(void)
{
    // setup actiongroups and initial settings here.
    viewgroup = new QActionGroup(this);
    viewgroup->addAction(ui->actionViewHex);
    viewgroup->addAction(ui->actionViewTxt);
    viewgroup->addAction(ui->actionViewOmni);
    ui->actionViewHex->setChecked(true);
    connect(viewgroup, SIGNAL(triggered(QAction*)), this, SLOT(ViewGroupTriggered(QAction*)));
}

int WombatForensics::ReturnVisibleViewerID(void)
{
    return ui->viewerstack->currentIndex();
}

WombatForensics::~WombatForensics()
{
    delete ui;
}

void WombatForensics::closeEvent(QCloseEvent* event)
{
    // possibly will need to set dirtreeview model to nothing. or the treemodel to nothing to clear values so it'll close properly.
    if(ui->dirTreeView->model() != NULL)
    {
        //event->ignore();
    }
    
    propertywindow->close();
    RemoveTmpFiles(); // can get rid of this function right now. I don't need to make temporary files to read.
    if(ProcessingComplete())
    {
        event->accept();
        qDebug() << "All threads are done";
    }
    else
    {
        event->ignore();
        qDebug() << "All threads aren't done yet.";
    }
    wombatdatabase->CloseCaseDB();
    wombatdatabase->CloseAppDB();
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
}

void WombatForensics::on_actionNew_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(wombatvarptr->caseobject.id > 0)
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        InitializeCaseStructure();
    }
}

void WombatForensics::on_actionOpen_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(wombatvarptr->caseobject.id > 0)
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        InitializeOpenCase();
    }
}

void WombatForensics::ViewGroupTriggered(QAction* selaction)
{
    wombatvarptr->visibleviewer = ReturnVisibleViewerID();
    if(wombatvarptr->visibleviewer > 1) // if an omniviewer is visible, set the current omnivalue.
        wombatvarptr->omnivalue = wombatvarptr->visibleviewer - 1; // may not need this since i can simply -1 it.

    // READ RESPECTIVE SIGNATURE HERE TO SET OMNIVALUE
    // GET CURRENTLY SELECTED FILE AND IF IT'S A VALUE, THEN LOAD THE RESPECTIVE CONTENT.
    // wombatvarptr->omnivalue = 1;
    if(selaction == ui->actionViewHex)
    {
        ui->viewerstack->setCurrentIndex(0); // hex
        // show the correct viewer page from stacked widget
    }
    else if(selaction == ui->actionViewTxt)
    {
        ui->viewerstack->setCurrentIndex(1); // txt
    }
    else if(selaction == ui->actionViewOmni) // omni 3,4,5
    {
        ui->viewerstack->setCurrentIndex(wombatvarptr->omnivalue + 1);
    }
    UpdateViewer();
}

void WombatForensics::on_actionView_Properties_triggered(bool checked)
{
    if(!checked)
        propertywindow->hide();
    else
    {
        propertywindow->show();
        if(ui->dirTreeView->selectionModel()->hasSelection())
            UpdateProperties();
    }
}

void WombatForensics::on_actionView_Progress_triggered(bool checked) // modify this to be the logviewer.
{
    if(!checked) // hide logviewer
        qDebug() << "hide logviewer here.";
    else// show logviewer
        qDebug() << "show logviewer here.";
}

void WombatForensics::UpdateOmniValue()
{
    if(wombatvarptr->selectedobject.objtype > 0 && wombatvarptr->selectedobject.objtype < 4)
    {
        wombatvarptr->omnivalue = 1;
    }
    else // if image - omnivalue = 2, video - omnivalue = 3.
    {
    }
}

int WombatForensics::DetermineOmniView(QString currentSignature)
{
    int retvalue = 0;
    QString tmppath = wombatvarptr->settingspath;
    tmppath += "/tsk-magicview.xml";
    QFile magicfile(tmppath);
    if(magicfile.exists()) // if the xml exists, read it.
    {
        if(magicfile.open(QFile::ReadOnly | QFile::Text))
        {
            QXmlStreamReader reader(&magicfile);
            while(!reader.atEnd())
            {
                reader.readNext();
                if(reader.isStartElement() && reader.name() == "signature")
                {
                    if(currentSignature.toLower().compare(reader.readElementText().toLower()) == 0)
                    {
                        retvalue = reader.attributes().value("viewer").toString().toInt();
                    }
                }
            }
            if(reader.hasError())
                fprintf(stderr, "Reader Error: %s\n", reader.errorString().toStdString().c_str());
            magicfile.close();
        }
        else
            fprintf(stderr, "Couldn't Read the omni file\n");
    }
    else
        fprintf(stderr, "Couldn't find the omni file\n");
    return retvalue;
}

void WombatForensics::UpdateSelectValue(const QString &txt)
{
    // set hex (which i'll probably remove anyway since it's highlighted in same window)
    int sellength = txt.size()/2;
    QString tmptext = "Length: " + QString::number(sellength);
    selectedhex->setText(tmptext);
    // get initial bytes value and then update ascii
    std::vector<uchar> bytes;
    Translate::HexToByte(bytes, txt);
    QString ascii;
    Translate::ByteToChar(ascii, bytes);
    tmptext = "Ascii: " + ascii;
    //selectedascii->setText(tmptext);
    QString strvalue;
    uchar * ucharPtr;
    // update the int entry:
    // pad right with 0x00
    int intvalue = 0;
    ucharPtr = (uchar*) &intvalue;
    memcpy(&intvalue,&bytes.begin()[0], min(sizeof(int),bytes.size()));
    strvalue.setNum(intvalue);
    tmptext = "Int: " + strvalue;
    //selectedinteger->setText(tmptext);
    // update float entry;
    float fvalue;
    ucharPtr = (uchar*)(&fvalue);
    if(bytes.size() < sizeof(float) )
    {
        for(unsigned int i= 0; i < sizeof(float); ++i)
        {
            if( i < bytes.size() )
            {
                *ucharPtr++ = bytes[i];
            }
            else
            {
                *ucharPtr++ = 0x00;
            }
        }
    }
    else
    {
        memcpy(&fvalue,&bytes.begin()[0],sizeof(float));
    }
    strvalue.setNum( fvalue );
    tmptext = "Float: " + strvalue;
    //selectedfloat->setText(tmptext);
    // update double
    double dvalue;
    ucharPtr = (uchar*)&dvalue;
    if(bytes.size() < sizeof(double) )
    {
        for(unsigned int i= 0; i < sizeof(double); ++i)
        {
            if( i < bytes.size() )
            {
                *ucharPtr++ = bytes[i];
            }
            else
            {
                *ucharPtr++ = 0x00;
            }
        }
    }
    else
    {
        memcpy(&dvalue,&bytes.begin()[0],sizeof(double));
    }
    strvalue.setNum( dvalue );
    tmptext = "Double: " + strvalue;
    //selecteddouble->setText(tmptext);
}

void WombatForensics::setOffsetLabel(off_t pos)
{
  QString label;
  label = "Offset: ";
  char    buffer[64];
#if _LARGEFILE_SOURCE
  sprintf(buffer,"0x%lx",pos);
#else
  sprintf(buffer,"0x%x",pos);
#endif
  label += buffer;
  selectedoffset->setText(label);
}

void WombatForensics::setScrollBarRange(off_t low, off_t high)
{
   (void)low;(void)high;
   // range must be contained in the space of an integer, just do 100
   // increments
   hexvsb->setRange(0,100);
}

void WombatForensics::setScrollBarValue(off_t pos)
{
  // pos is the topLeft pos, set the scrollbar to the
  // location of the last byte on the page
  // Note: offsetToPercent now rounds up, so we don't
  // have to worry about if this is the topLeft or bottom right
  hexvsb->setValue(hexwidget->offsetToPercent(pos));
}