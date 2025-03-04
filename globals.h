#ifndef GLOBALS_H
#define GLOBALS_H

#include <QtGlobal>

// ALL THE GLOBAL DECLARATIONS

// don't use #include <QString> here, instead do this:

QT_BEGIN_NAMESPACE
class QString;
class QTimer;
QT_END_NAMESPACE

// that way you aren't compiling QString into every header file you put this in...
// aka faster build times.

#define MAGIC_NUM 42

extern QTimer *monitor;
extern QString search_string;
extern int improveStatus;
extern int playerStatus;
extern int selectedMusicIndex;
extern QMap<int, QString> musicPosInfo; //[id,position]
extern QMap<int, int> musicDurInfo; //[id, duration]
extern QMap<int, int> musicChkInfo; //[id,checked]
extern QMap<int, int> musicPlayed;
extern bool loaded;
extern int processedCounts;
extern int totalDuration;
extern int remainDuration;

#endif // GLOBALS_H
