#ifndef MUSICITEM_H
#define MUSICITEM_H

#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QTimer>
#include <QMediaPlayer>
#include <track.h>
#include <QMouseEvent>
#include <QWidget>
#include <QProcess>


class MusicItem : public QFrame
{
    Q_OBJECT
public:
    explicit MusicItem(QFrame *parent = 0, Track *music = nullptr, int index = 0);
    void setMusic(Track *music = nullptr);

public slots:
    void chk_clicked();

public:
    bool selected;
    QVBoxLayout *vLayout1;
    QCheckBox *chk;
    QSpacerItem *spacer1, *spacer2, *spacer3;

    QVBoxLayout *vLayout2;
    QVBoxLayout *vLayout3;
    QLabel *lblTitle;
    QHBoxLayout *hLayout1;

    QLabel *lblPosition;

    QHBoxLayout *hLayout;

    QMediaPlayer *player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    Track * music;
    int music_index;
};

#endif // MUSICITEM_H
