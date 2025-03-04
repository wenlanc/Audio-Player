#include "musicitem.h"
#include "QtGui"
#include "QGraphicsOpacityEffect"
#include "globals.h"
#include <QMessageBox>

MusicItem::MusicItem(QFrame *parent, Track *music, int index) : QFrame(parent)
{
    selected = false;
    this->music_index = index;
    vLayout1=new QVBoxLayout();

    chk = new QCheckBox();
    chk->setChecked(true);
    spacer1 = new QSpacerItem(0, 3, QSizePolicy::Fixed, QSizePolicy::Fixed);
    vLayout1->addWidget(chk);
    vLayout1->addItem(spacer1);

    vLayout2=new QVBoxLayout();

    lblTitle = new QLabel("music title");
    lblTitle->setStyleSheet("QLabel {font-size:18pt;font-weight:bold;border: 0px; }");
    lblTitle->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    spacer2 = new QSpacerItem(25, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout1 = new QHBoxLayout();
    hLayout1->addItem(spacer2);
    hLayout1->addWidget(lblTitle);
    vLayout2->addLayout(hLayout1);

    lblPosition=new QLabel("00:00:00");
    lblPosition->setStyleSheet("QLabel {color : #88c365; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");

    hLayout=new QHBoxLayout;

    hLayout->addLayout(vLayout1);
    hLayout->addLayout(vLayout2);

    vLayout3=new QVBoxLayout();
    vLayout3->addWidget(lblPosition);
    spacer3 = new QSpacerItem(0, 3, QSizePolicy::Fixed, QSizePolicy::Fixed);
    vLayout3->addItem(spacer3);
    hLayout->addLayout(vLayout3);

    this->setLayout(hLayout);
    this->setStyleSheet("QFrame {"
                        "color: white; "
                        "background-color: #2e2f33;"
                        "border: 1px solid white; "
                        "}"
                        );

    connect(chk, SIGNAL(clicked()), this, SLOT(chk_clicked()));
    this->setMusic(music);
}

void MusicItem::setMusic(Track *music)
{
    this->music = music;

    connect(player, &QMediaPlayer::durationChanged, this, [&](qint64 dur) {
        this->music->setDuration(dur);
    });
    QString qstr = QString::fromStdString(this->music->getLocation());
    player->setMedia(QUrl::fromLocalFile(qstr));

    qstr = QString::fromStdString(this->music->getName());
    lblTitle->setText(qstr);
}

void MusicItem::chk_clicked()
{
    QGraphicsOpacityEffect* opacity_effect = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(opacity_effect);
    qDebug() << this->music_index;
    if (chk->isChecked()){
        opacity_effect->setOpacity(1);
        musicChkInfo[this->music_index] = 1;
    }else {
        musicChkInfo[this->music_index] = 0;
        opacity_effect->setOpacity(0.5);
    }
}

