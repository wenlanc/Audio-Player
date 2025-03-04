#include "globals.h"

#include "globals.h"
#include <QString>
#include <QTimer>
#include <QMediaPlayer>

// ALL THE GLOBAL DEFINITIONS

QString search_string = "";
int improveStatus = QMediaPlayer::StoppedState;
int playerStatus = QMediaPlayer::StoppedState;
QTimer *monitor = new QTimer();
int selectedMusicIndex = -1;
QMap<int, QString> musicPosInfo; //[id, position]
QMap<int, int> musicDurInfo; //[id, duration]
QMap<int, int> musicChkInfo; //[id, checked]
QMap<int, int> musicPlayed;
bool loaded = false;
int processedCounts = 0;
int totalDuration = 0;
int remainDuration = 0;
