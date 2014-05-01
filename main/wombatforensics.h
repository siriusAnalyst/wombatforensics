#ifndef WOMBATFORENSICS_H
#define WOMBATFORENSICS_H

#include "wombatinclude.h"
#include "wombatvariable.h"
#include "wombatdatabase.h"
#include "wombatframework.h"
#include "wombatfunctions.h"
#include "ui_wombatforensics.h"
#include "progresswindow.h"
#include "exportdialog.h"
#include "globals.h"
#include "checkableproxymodel.h"

namespace Ui {
class WombatForensics;
}

//InterfaceSignals* isignals;

class WombatForensics : public QMainWindow
{
    Q_OBJECT

public:
    explicit WombatForensics(QWidget *parent = 0);
    ~WombatForensics();
    WombatDatabase* wombatdatabase;
    WombatVariable wombatvariable;
    WombatVariable* wombatvarptr;
    TskObject tskobject;
    TskObject* tskobjptr;
    WombatFramework* wombatframework;
    ProgressWindow* wombatprogresswindow;
    ExportDialog* exportdialog;
    CheckableProxyModel* checkableproxy;

signals:
    void LogVariable(WombatVariable* wombatVariable);
private slots:
    void AddEvidence();
    void RemEvidence();
    void ExportEvidence();
    void on_actionNew_Case_triggered();
    void on_actionOpen_Case_triggered();
    void on_actionView_Progress_triggered(bool checked);
    void UpdateProgress(int count, int processcount);
    void UpdateMessageTable();
    void dirTreeView_selectionChanged(const QModelIndex &index);
    void HideProgressWindow(bool checkstate);
    void DisplayError(QString errorNumber, QString errorType, QString errorValue);
    void PopulateProgressWindow(WombatVariable* wvariable);
    void ResizeColumns(void);
    void OpenParentImage(int imgid);
    void ResizeViewColumns(const QModelIndex &index)
    {
        ResizeColumns();
    }
    void FileExport(FileExportData* exportdata);
    void setScrollBarRange(off_t low, off_t high);
    void setScrollBarValue(off_t pos);
    void setOffsetLabel(off_t pos);
    void UpdateSelectValue(const QString &txt);
    void ViewGroupTriggered(QAction* curaction);
    void LoadComplete(bool isok);
    void InitializeQueryModel(void);
    void UpdateTree(void);

protected:
    void closeEvent(QCloseEvent* event);
private:
    Ui::WombatForensics *ui;

    void SetupHexPage(void);
    void SetupToolbar(void);
    void InitializeAppStructure(void);
    void InitializeCaseStructure(void);
    void InitializeEvidenceStructure(void);
    void InitializeOpenCase(void);
    void InitializeWombatFramework(void);
    void InitializeDirModel(void);
    //void InitializeQueryModel(void);
    void UpdateViewer(void);
    void UpdateOmniValue(void);
    void LoadHexContents(void);
    void LoadTxtContents(void);
    void LoadWebContents(void);
    void LoadImgContents(void);
    void LoadVidContents(void);

    void RemoveTmpFiles(void);

    //QStandardItem* GetCurrentImageDirectoryTree(QObject *plugin, QString imageDbPath, QString imageName);
    //QThreadPool* threadpool;
    int ReturnVisibleViewerID();
    int StandardItemCheckState(QStandardItem* tmpitem, int checkcount);
    int StandardItemListCount(QStandardItem* tmpitem, int listcount);
    QVector<FileExportData> SetFileExportProperties(QStandardItem* tmpitem, FileExportData* tmpexport, QVector<FileExportData>);
    QVector<FileExportData> SetListExportProperties(QStandardItem* tmpitem, FileExportData* tmpexport, QVector<FileExportData>);
    int DetermineOmniView(QString currentSignature);
    QModelIndex curselindex;

    QFuture<void> sqlfuture;
    QFutureWatcher<void> sqlwatcher;
    //QFuture<void> openfuture;
    //QFutureWatcher<void> openwatcher;

    off_t offset() const;
    HexEditor* hexwidget;
    QActionGroup* viewgroup;
    QScrollBar* hexvsb;
    QStatusBar* hstatus;
    QLabel* selectedoffset;
    QLabel* selectedhex;
    QLabel* selectedascii;
    QLabel* selectedinteger;
    QLabel* selectedfloat;
    QLabel* selecteddouble;
    QProgressBar* mainprogress;
    QLabel* filecountlabel;

    // NEED TO REMOVE THESE ONCE I GET MY FUNCTIONS DONE AND REMOVE THE OLD STRUCTURE
    //QStandardItemModel* currenttreemodel;
    //QStandardItemModel* wombatdirmodel;
    //QStandardItemModel* wombattypmodel;
};



/*
 *class MyProxy : public QAbstractProxyModel {
public: 
MyProxy() : QAbstractProxyModel(){} 
QModelIndex mapToSource(const QModelIndex &proxyIndex) const {
if(!proxyIndex.isValid()) return QModelIndex(); 
int c = columnFromIndex(proxyIndex); 
int r = rowFromIndex(proxyIndex); 
return sourceModel()->index(r,c); 
} 

QModelIndex mapFromSource(const QModelIndex &sourceIndex) const {
if(!sourceIndex.isValid()) return QModelIndex(); 
if(sourceIndex.column()==0) 
return createIndex(sourceIndex.row(), 0, calculateId(sourceIndex));
return createIndex(0, 0, calculateId(sourceIndex)); 
} 
int columnCount(const QModelIndex &parent = QModelIndex()) const { 
return 1; 
} 
int rowCount(const QModelIndex &parent = QModelIndex()) const { 
if(!parent.isValid()) 
return qMin(0x10000, sourceModel()->rowCount()); 
int c = mapToSource(parent).column(); 
if(c==sourceModel()->columnCount()-1) 
return 0; 
return 1; 
} 
QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const {
if(parent.isValid()) { 
// if parent is valid then in the source model 
// we want to receive the same row but the next column, provided that row==0 && col==0
// otherwise the index is not valid 
if(row!=0 || column!=0) return QModelIndex(); 
return createIndex(row, column, (int)parent.internalId()+1); 
} 
if(column!=0) return QModelIndex(); 
// parent is not valid thus we can calculate the id the same way as for the source model 
return createIndex(row, 0, calculateId(row, 0)); 
} 
QModelIndex parent(const QModelIndex &child) const { 
if(!child.isValid()) 
return QModelIndex(); 
// parent of an index in the source model is the same row but previous column 
int c = mapToSource(child).column(); 
int r = mapToSource(child).row(); 
if(c==0){ 
// if original column == 0 then there is no parent 
return QModelIndex(); 
} 
c -= 1; 
if(c==0){ 
return createIndex(r, 0, calculateId(r, c)); 
} 
return createIndex(0, 0, calculateId(r, c)); 
} 
private: 
int columnFromIndex(const QModelIndex &proxyIndex) const {
quint32 id = proxyIndex.internalId();
int c = (id & 0xffff);
return c;
}
int rowFromIndex(const QModelIndex &proxyIndex) const {
quint32 id = proxyIndex.internalId();
int r = (id & 0xffff0000) >> 16;
return r;
}
int calculateId(const QModelIndex &sourceIndex) const {
quint32 r = sourceIndex.row();
quint32 c = sourceIndex.column();
return calculateId(r, c);
}
int calculateId(quint32 r, quint32 c) const{
return (((r & 0xffff) << 16) | (c & 0xffff));
}
};

int main(int argc, char **argv) {
QApplication app(argc, argv);
QTreeView tv;
QStandardItemModel model(40,20);
for(int i=0; i<model.rowCount(); i++){
for(int j=0; j<model.columnCount(); j++){
model.setData(model.index(i,j), qrand() % 100, Qt::DisplayRole);
}
}
MyProxy proxy;
proxy.setSourceModel(&model);
tv.setModel(&proxy);
tv.show();
QTableView orig;
orig.setModel(&model);
orig.show();
return app.exec();
}
 *
 *
 */ 



class FileViewSqlModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    FileViewSqlModel(QObject* parent = 0) : QSqlQueryModel(parent) {};

    //QSet<QPersistentModelIndex> checklist;

    QVariant data(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        if(value.isValid() && role == Qt::DisplayRole)
        {
            if(index.column() >= 6 && index.column() <= 9)
            {
                char buf[128];
                QString tmpstr = QString(tsk_fs_time_to_str(value.toInt(), buf));

                return tmpstr;
            }
        }

        return value;
    };
};
/*
 *C++
 *
     virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
     //virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
     //virtual void setEditorData ( QWidget * editor, const QModelIndex & index );
 *
 *Qt::ItemFlags checkablesqlquerymodel::flags ( const QModelIndex & index ) const
{
    if(!index.isValid())
        return 0;
 
    if(index.column()==0)
    {
        return  Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return QAbstractItemModel::flags(index);
    }
 *
 *
 *
 * You probably need to add Qt::ItemIsUserCheckable to your flags for that column.

You should implement data() and setData() in your model for the Qt::CheckStateRole of the first column. Return (or accept) a Qt::CheckState value: code needs to worry about getting it in/out of the database.

The standard delegate will then draw a check box in the cell with the check state specified by the data(Qt::CheckStateRole) value.
 *
 * PYTHON
 *class CheckboxSqlModel(QtSql.QSqlQueryModel):
    def __init__(self, column):
        super(CheckboxSqlModel, self).__init__()
        self.column = column
        self.checkboxes = list() #List of checkbox states
        self.first = list() #Used to initialize checkboxes

    #Make column editable
    def flags(self, index):
        flags = QtSql.QSqlQueryModel.flags(self, index)
        if index.column() == self.column:
            flags |= QtCore.Qt.ItemIsUserCheckable
        return flags

    def data(self, index, role=QtCore.Qt.DisplayRole):
        row = index.row()
        if index.column() == self.column and role == QtCore.Qt.CheckStateRole:
            #Used to initialize
            if row not in self.first :
                index = self.createIndex(row, self.column)
                self.first.append(row)
                self.checkboxes.append(False)
                return QtCore.Qt.Unchecked
            #if checked
            elif self.checkboxes[row]:
                return QtCore.Qt.Checked
            else:
                return QtCore.Qt.Unchecked
        else:
            return QtSql.QSqlQueryModel.data(self, index, role)

    def setData(self, index, value, role=QtCore.Qt.DisplayRole):
        row = index.row()
        if index.column() == self.column and role == QtCore.Qt.CheckStateRole:
            if value.toBool():
                self.checkboxes[row] = True
            else:
                self.checkboxes[row] = False
            self.dataChanged.emit(index, index)
            return True
        else:
            return False;
 *
 */ 
/*
class ThreadRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThreadRunner(QObject* object, QString input, WombatVariable* wVariable)
    {
        method = input;
        //caller = (SleuthKitPlugin*)object;
        wombatvariable = wVariable;
    };
    void run()
    {
        if(method.compare("initialize") == 0)
            caller->Initialize(wombatvariable);
        if(method.compare("openevidence") == 0)
            caller->OpenEvidence(wombatvariable);
        if(method.compare("populatecase") == 0)
            caller->PopulateCase(wombatvariable);
        if(method.compare("showfile") == 0)
            caller->ShowFile(wombatvariable);
        if(method.compare("refreshtreeviews") == 0)
            caller->RefreshTreeViews(wombatvariable);
        if(method.compare("exportfiles") == 0)
            caller->ExportFiles(wombatvariable);
    };
private:
    QString method;
    //SleuthKitPlugin* caller;
    WombatVariable* wombatvariable;
};
*/

#endif // WOMBATFORENSICS_H
