#ifndef WOMBATFRAMEWORK_H
#define WOMBATFRAMEWORK_H

#include <stdlib.h>
#include <stdio.h>
#include "tsk/libtsk.h"
#include <QObject>
#include <QtConcurrent>

#include "wombatvariable.h"

class WombatFramework : public QObject
{
    Q_OBJECT
public:
    WombatFramework(WombatVariable* wombatvariable);
    ~WombatFramework();

    void BuildEvidenceModel(void);
    void OpenEvidenceImage(void);
    void OpenEvidenceImages(void); // might not need this functions, since re-opening a case can pull it's info from the db.
    void OpenVolumeSystem(void);
    void GetVolumeSystemName(void);
    void OpenPartitions(void);
    void OpenFileSystems(void);

private:
    WombatVariable* wombatptr;
};

#endif // WOMBATFRAMEWORK_H
