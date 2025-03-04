#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "playlist.h"
#include <QtMultimedia/QMediaPlayer>
#include "QTimer"
#include "QPalette"
#include "vector"
#include "QKeyEvent"
#include "QLineEdit"
#include "musicitem.h"
#include "QFileSystemModel"
#include "QProcess"
#include "QListWidgetItem"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showMaximized();
private slots:

    void on_add_clicked();
    void selectImprovePath();
    void controlImprove();
    void improveDblClick();
    void monitoring();
    void improveNext();
    void improvePre();
    void improveSeek(int pos);
    void improveVolumeChange(int pos);
    void selectImproveItem();
    void selectPlace();
    void clearSearchBox();
    void improveClick();
    void on_searchBar_textChanged(const QString &arg1);
    void onMusicListItemClicked(QListWidgetItem* item);
    void on_upgradeBtn_clicked();
    void playersliderPressed();
    void playersliderMoved(int position);
    void on_convertAllBtn_clicked();
    void on_btnPlay_clicked();
    void update();
    void on_onRadio_clicked();
    void on_offRadio_clicked();

private:
    QStringList invalidList;
    void updateList();
    void loading();
    int lCounter = 0;
    int playnum = -1;
    Playlist playlist;
    QString improvePath;
    QString workingPath;
    Ui::MainWindow *ui;
    QTimer *updater = new QTimer(this);
    bool autoplay = true;
    bool emptyval = true;

    //QTimer *monitor;

    MusicItem **musicItems;
    QMediaPlayer *improvePlayer = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    QMediaPlayer *mainPlayer = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
};

#endif // MAINWINDOW_H
