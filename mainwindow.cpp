#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDesktopServices"
#include "algorithm"
#include "iostream"
#include "string"
#include "QGraphicsOpacityEffect"
#include "QGraphicsColorizeEffect"
#include "musicitem.h"
#include "QCollator"
#include "QFileSystemModel"
#include "QFileIconProvider"
#include "QSortFilterProxyModel"
#include "QMessageBox"
#include "QThread"
#include "QProcess"
#include "QMovie"
#include "QSettings"
#include "QInputDialog"
#include "globals.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDate>
#include <QDialogButtonBox>

qint64 getFileSize(const QString &path)
{
    qint64 size = 0;
    QFileInfo fileInfo(path);

    if(fileInfo.isSymLink() && fileInfo.size() == QFileInfo(fileInfo.symLinkTarget()).size())
    {
        // Try this approach first
        QFile file(path);
        if(file.exists() && file.open(QIODevice::ReadOnly))
            size = file.size();
        file.close();

        // If that didn't work, try this
        if(size == 0)
        {
            QString tmpPath = path+".tmp";
            for(int i=2; QFileInfo().exists(tmpPath); ++i) // Make sure filename is unique
                tmpPath = path+".tmp"+QString::number(i);

            if(QFile::copy(path, tmpPath))
            {
                size = QFileInfo(tmpPath).size();
                QFile::remove(tmpPath);
            }
        }
    }
    else size = fileInfo.size();

    return size;
}

QString getMacAddress()
{
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (!interface.flags().testFlag(QNetworkInterface::IsLoopBack))
            foreach (QNetworkAddressEntry entry, interface.addressEntries())
            {
                if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".")
                     && !interface.humanReadableName().contains("VM") && !interface.humanReadableName().contains("VPN"))
                        return interface.hardwareAddress();
            }
    }
    return "";
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int xxx = 0;
    QSettings settings;
    QString request = settings.value("request").toString().replace("-","");
    bool ok;
    QString device_id = "";
    for (int i = 0;i<request.length();i+=2) {
        if (request.mid(i,2).toInt(&ok, 10)>=50){
            device_id +=QString::number(("1"+request.mid(i+1,1)).toInt(), 16).toUpper();
        }else
            device_id +=request.mid(i+1,1);
    }

    QString serial_code=settings.value("serial").toString();
    serial_code.replace("-","");
    QString serial_code_decrypt="";
    for (int i = 0;i<serial_code.length();i+=3) {
        serial_code_decrypt += serial_code.mid(i+1,2);
    }

    QString new_device_id = "";QString trial="";
    for (int i=0;i<serial_code_decrypt.length();i++) {
        int n = serial_code_decrypt[i].unicode()-'A';
        if (i % 4 == 1)
            trial += QString::number(n,16).toUpper();
        else
            new_device_id += QString::number(n,16).toUpper();
    }

    QDate curDate = QDate::currentDate();
    QDate baseDate = QDate::fromString("2010-10-14","yyyy-MM-dd");
    int activate_days = trial.toInt()-(curDate.toJulianDay()-baseDate.toJulianDay());

    activate_days = 10; // for preactivated on v5.0.0
    if ((activate_days<0)||(device_id!=new_device_id)){
        device_id = getMacAddress().replace(":","-");
        request="";
        foreach (QChar i, device_id)
        {
            QString c(i);
            if (c=="-") {
                request += c;
                continue;
            }
            bool ok;
            int v=c.toInt(&ok, 16);
            if (v>9)
                request += QString::number(qrand()%5+5)+QString::number(v%10);
            else
                request += QString::number(qrand()%5)+c;

        }

        QDialog * d = new QDialog();
        QVBoxLayout * vbox = new QVBoxLayout();
        QLabel * labelA = new QLabel("Activation Instructions\n"
                                    "Please copy the Request Code or Take a Photo and\n"
                                    "email it to us. Note: Allow up to 24hrs\n"
                                    "Email mu.serialrequest@gmail.com\n"
                                     "\nRequest Code");
        QLabel * labelB = new QLabel("\nSerial Code");
        QLabel * labelC = new QLabel("Serial codes are valid for a given number of days only. You can\n"
                                     "request additional days by contacting us on the above details.\n");
        QLineEdit * lineEditA = new QLineEdit(request);
        QLineEdit * lineEditB = new QLineEdit();

        QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                            | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), d, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), d, SLOT(reject()));

        vbox->addWidget(labelA);
        vbox->addWidget(lineEditA);
        vbox->addWidget(labelB);
        vbox->addWidget(lineEditB);
        vbox->addWidget(labelC);
        vbox->addWidget(buttonBox);

        d->setLayout(vbox);

        int result = d->exec();
        if(result == QDialog::Accepted)
        {
            QString serial_code=lineEditB->text();
            serial_code.replace("-","");
            QString serial_code_decrypt="";
            for (int i = 0;i<serial_code.length();i+=3) {
                serial_code_decrypt += serial_code.mid(i+1,2);
            }

            QString new_device_id = "";QString trial="";
            for (int i=0;i<serial_code_decrypt.length();i++) {
                int n = serial_code_decrypt[i].unicode()-'A';
                if (i % 4 == 1)
                    trial += QString::number(n,16).toUpper();
                else
                    new_device_id += QString::number(n,16).toUpper();
            }

            QDate curDate = QDate::currentDate();
            QDate baseDate = QDate::fromString("2010-10-14","yyyy-MM-dd");
            activate_days = trial.toInt()-(curDate.toJulianDay()-baseDate.toJulianDay());
            if ((activate_days<0)||(device_id.replace("-","")!=new_device_id)){
                QMessageBox::critical(
                    this,
                    tr("Invalid License"),
                    tr("Please contact with owner.\nTry to get valid license code.") );
                 exit(0);
            }
            settings.setValue("request",request);
            settings.setValue("serial",serial_code);
            settings.sync();
            QMessageBox::information (0, "Registered Success",
                                              QString("Program has been activated for %1 days.").arg(QString::number(activate_days+1)));

        }else
            exit(0);
    }
    ui->setupUi(this);
    this->setStyleSheet("color: silver; "
                        "background-color: #302F2F; "
                        "selection-background-color:#3d8ec9;"
                        "selection-color: black;"
                        "background-clip: border;"
                        "border-image: none;"
                        "outline: 0;"
                        "QMessageBox { background-color: rgb(51, 51, 51);}"
                        "QMessageBox QLabel { color: rgb(200, 200, 200);}"
                        "QInputDialog { background-color: rgb(51, 51, 51);}"
                        "QInputDialog QLabel { color: rgb(200, 200, 200);}"
                        );
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
                                   Qt::WindowMinimizeButtonHint |
                                   Qt::WindowMaximizeButtonHint |
                                   Qt::WindowCloseButtonHint);

    QString title ="MU Player by Eventshd Version 6.0.1";
    this->setWindowTitle(title);

    updateList();
    ui->listWidget->setStyleSheet("QListWidget::item {color:transparent;background-color:transparent;}");
    ui->searchBar->setStyleSheet("QLineEdit {border: 2px solid white;}");
    ui->add->setStyleSheet("QPushButton {color: white;background-color: #e85700;border: 1px solid white;padding:5px;}");
    ui->selectImprovePathBtn->setStyleSheet("QPushButton {color: white;background-color: #8660a9;border: 1px solid white;padding:5px;}");
    ui->btn_select->setStyleSheet("QPushButton {color: black;background-color: #4fe25a;border: 1px solid white;padding:5px;}");
    ui->upgradeBtn->setStyleSheet("QPushButton {background-color: #999999; color : #FFFFFF; }");
    ui->upgradeBtn->hide();
    ui->add->setEnabled(false);
    //monitor = new QTimer(this);
    monitor->setInterval(200);
    connect(monitor, SIGNAL(timeout()), this, SLOT(monitoring()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(onMusicListItemClicked(QListWidgetItem*)));
    updater->setInterval(200);
    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    connect(ui->slider, SIGNAL(sliderMoved(int)), this, SLOT(playersliderMoved(int)));
    connect(ui->slider, SIGNAL(sliderPressed()), this, SLOT(playersliderPressed()));
    monitor->start();

    ui->improveVolumeBtn->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->improveSlider->setStyleSheet(
                "QSlider::groove:horizontal {"
                    "border: 1px solid #999999;"
                    "border-radius: 5px; "
                    "background-color:#ff585f;"
                    "height: 10px;"
                    "margin: 0 0 0 0;}"


                "QSlider::handle:horizontal { "
                    "background-color: #ecedec; "
                    "border: 2px solid #302F2F;"
                    "width: 16px; "
                    "height: 20px; "
                    "line-height: 20px; "
                    "margin-top: -5px; "
                    "margin-bottom: -5px; "
                    "border-radius: 10px; "
                "}"

                "QSlider::handle:horizontal:hover { "
                "    border-radius: 10px;"
                "}"

            "QSlider::add-page:horizontal {"
            "background-color: #72858f;"
            "border-radius: 5px; "
            "}"

            "QSlider::sub-page:horizontal {"
            "background-color: #b3de5d;"
            "border-radius: 5px; "
            "}"
                );
    ui->improveVolumeSlider->setStyleSheet(
                "QSlider::groove:horizontal {"
                    "border: 1px solid #999999;"
                    "border-radius: 5px; "
                    "background-color:#ff585f;"
                    "height: 10px;"
                    "margin: 0 0 0 0;}"


                "QSlider::handle:horizontal { "
                    "background-color: #ecedec; "
                    "border: 2px solid #302F2F;"
                    "width: 16px; "
                    "height: 20px; "
                    "line-height: 20px; "
                    "margin-top: -5px; "
                    "margin-bottom: -5px; "
                    "border-radius: 10px; "
                "}"

                "QSlider::handle:horizontal:hover { "
                "    border-radius: 10px;"
                "}"

            "QSlider::add-page:horizontal {"
            "background-color: #72858f;"
            "border-radius: 5px; "
            "}"

            "QSlider::sub-page:horizontal {"
            "background-color: #b3de5d;"
            "border-radius: 5px; "
            "}"
                );
    ui->improveVolumeSlider->setValue(100);
    ui->improveVolumeBtn->setStyleSheet("QPushButton {border-style: none;}");
    ui->improvePreBtn->setStyleSheet("QPushButton {background-color: #92a6af; border-style: solid; border-width:3px; border-radius:13px; border-color: #302F2F; color: #302F2F; max-width:20px; max-height:20px; min-width:20px; min-height:20px;}");
    ui->improveCtlBtn->setStyleSheet("QPushButton {background-color: #92a6af; border-style: solid; border-width:3px; border-radius:16px; border-color: #302F2F; color: #302F2F; max-width:26px; max-height:26px; min-width:26px; min-height:26px;}");
    ui->improveNextBtn->setStyleSheet("QPushButton {background-color: #92a6af; border-style: solid; border-width:3px; border-radius:13px; border-color: #302F2F; color: #302F2F; max-width:20px; max-height:20px; min-width:20px; min-height:20px;}");

    ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->improvePreBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->improveNextBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->mediaWidget->setStyleSheet("background-color: #8498a3");
    ui->convertAllBtn->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectPlace()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Select audio place"));
    QFileSystemModel *model = new QFileSystemModel();
    model->setRootPath(dir);
    QStringList filter;
    filter <<"*.mp3" << "*.WAV" << "*.aif" << "*.wma" << "*.m4a" ;
    model->setNameFilters(filter);
    model->setNameFilterDisables(false);

    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(dir));
    ui->treeView->setAnimated(false);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->add->setEnabled(true);
}

void MainWindow::selectImprovePath()
{
    improvePath =QFileDialog::getExistingDirectory(this,tr("Select improvisation place"));

    if (improvePath == "") return;
    //tableview
    ui->improveList->clear();
    ui->improveList->horizontalHeader()->show();
    ui->improveList->setRowCount(0);
    ui->improveList->setColumnCount(1);
    ui->improveList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->improveList->horizontalHeader()->setStretchLastSection(true);
    ui->improveList->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Folder Name"),QTableWidgetItem::Type));
    ui->improveList->setStyleSheet("QHeaderView::section {background-color:#302F2F}");
    ui->improveList->setShowGrid(false);
    ui->improveList->horizontalHeader()->hide();

    QDir directory(improvePath);
    QStringList files = directory.entryList(QStringList() << "*.mp3" << "*.m4a",QDir::Files);
    directory.setSorting(QDir::NoSort);  // will sort manually with std::sort

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(
        files.begin(),
        files.end(),
        [&collator](const QString &file1, const QString &file2)
        {
            return collator.compare(file1, file2) < 0;
        });

    foreach(QString filename, files) {
        QFileInfo  file(filename);
        ui->improveList->insertRow(ui->improveList->rowCount());
        QTableWidgetItem *itemField = new QTableWidgetItem();
        itemField->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        itemField->setText(file.fileName().trimmed());
        ui->improveList->setItem( ui->improveList->rowCount()-1, 0, itemField);
    }
    improveStatus = QMediaPlayer::StoppedState;
}
void MainWindow::playersliderMoved(int position)
{
   mainPlayer->setPosition(mainPlayer->duration() / 1000 * position);
}

void MainWindow::playersliderPressed()
{
   mainPlayer->setPosition(mainPlayer->duration() * ui->slider->value() / 1000);
}
void MainWindow::clearSearchBox()
{
     ui->searchBar->clear();
}
void MainWindow::improveClick()
{
     improvePlayer->setPosition(improvePlayer->duration() / 1000 * ui->improveSlider->value());
}
void MainWindow::on_add_clicked()
{
    musicPosInfo.clear();
    musicDurInfo.clear();
    musicChkInfo.clear();
    musicPlayed.clear();
    processedCounts = 0;
    selectedMusicIndex = -1;
    loaded = false;
    playerStatus = QMediaPlayer::StoppedState;

    loading();
}
void MainWindow::controlImprove()
{
    if (playerStatus == QMediaPlayer::PlayingState) {
        QMessageBox::warning(this,"Warning","Please stop/pause audio player and try again");
        return;
    }
    if (ui->selectedImproveLbl->text()=="") {
        QMessageBox::warning(this,"Warning","Please select improve audio file");
        return;
    }
    if(improvePlayer->state() == QMediaPlayer::PlayingState){
        ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        improvePlayer->pause();
        if (playerStatus == QMediaPlayer::StoppedState && monitor->isActive()) monitor->stop();
    }else{
        ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        improvePlayer->play();
        if (!monitor->isActive()) monitor->start();
    }
    improveStatus = improvePlayer->state();
}
void MainWindow::selectImproveItem()
{
    ui->selectedImproveLbl->setText(ui->improveList->item(ui->improveList->currentRow(), 0)->text().trimmed());
    improvePlayer->setMedia(QUrl::fromLocalFile(improvePath+"/"+ui->selectedImproveLbl->text()));
}
void MainWindow::improveDblClick()
{
    if (playerStatus == QMediaPlayer::PlayingState) {
        QMessageBox::warning(this,"Warning","Please stop/pause audio player and try again");
        return;
    }
    ui->selectedImproveLbl->setText(ui->improveList->item(ui->improveList->currentRow(), 0)->text().trimmed());
    improvePlayer->setMedia(QUrl::fromLocalFile(improvePath+"/"+ui->selectedImproveLbl->text()));
    improvePlayer->play();
    if (!monitor->isActive()) monitor->start();
    improveStatus = improvePlayer->state();
}
void MainWindow::loading()
{
    loaded = false;
    if (ui->treeView == nullptr){
        QMessageBox::about(this,"Alert","Please select folder that contains audio files.");
        return;
    }

    QModelIndexList indexs = ui->treeView->selectionModel()->selectedIndexes();

    if (indexs.empty()){
        QMessageBox::about(this,"Alert","Please select folder that contains audio files.");
        return;
    }

    QString filename = ui->treeView->model()->data(indexs.first()).toString();

    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(ui->treeView->model());
    workingPath = model->filePath(indexs.first());

    QFileInfo check_file(workingPath);
        // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.isFile()) {
        QMessageBox::about(this,"Alert","Please select folder that contains audio files.");
    }else{

        for (int i=0; i < int(playlist.tracks.size());i++)
        {
            this->musicItems[i]->player->stop();
        }
        QDir recoredDir(workingPath);
        QStringList nameFilter;
        nameFilter << "*.mp3" << "*.WAV" << "*.aif" << "*.wma" << "*.m4a" ;
        recoredDir.setSorting(QDir::NoSort);  // will sort manually with std::sort
        auto files = recoredDir.entryList( nameFilter, QDir::Files );

        QCollator collator;
        collator.setNumericMode(true);

        std::sort(
            files.begin(),
            files.end(),
            [&collator](const QString &file1, const QString &file2)
            {
                return collator.compare(file1, file2) < 0;
            });

        QStringList filePaths;

        if(!files.empty())
        {
            int idx = 0;
            for(QString f:files)
            {
                f = workingPath + '/' + f;
                filePaths.push_back(f);
                musicPosInfo.insert(idx, 0);
                musicDurInfo.insert(idx, 0);
                musicChkInfo.insert(idx, 1);
                musicPlayed.insert(idx, 0);
                idx ++;
            }

            ui->slider->setEnabled(true);
            ui->slider->setValue(0);
            ui->lblPosition->setStyleSheet("QLabel {color : #88c365; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");
            ui->btnPlay->setStyleSheet("width:50px; height:50px; border-image:url(:/play.png);");
            ui->btnPlay->setEnabled(true);
            mainPlayer->stop();
            monitor->stop();
            updater->stop();

            playlist.tracks.clear();
            playlist.add(filePaths);
            updateList();
            if (!monitor->isActive())
            {
                qDebug() << "Starting Monitor...";
                monitor->start();
            }
        }else {
            QMessageBox::about(this,"Alert","Please select folder that contains audio files.");
        }
    }
    ui->lblFolderName->setText("Session - " + filename);
    playerStatus = QMediaPlayer::StoppedState;
}

void MainWindow::showMaximized()
{
    setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen))
                   | Qt::WindowMaximized);
    show();
}
void MainWindow::updateList()
{
    ui->listWidget->clear();
    this->musicItems = new MusicItem*[int(playlist.tracks.size())];

    for (int i=0; i < int(playlist.tracks.size());i++)
    {
        this->musicItems[i] = new MusicItem(new QFrame(), & playlist.tracks[i], i);

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromStdString(this->musicItems[i]->music->getName()));
        item->setForeground(Qt::transparent);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
        ui->listWidget->addItem(item);

        item->setSizeHint(this->musicItems[i]->minimumSizeHint());
        ui->listWidget->setItemWidget(item, this->musicItems[i]);
    }
}
QString getTimeString(int remained){
    int seconds = (remained / 1000) % 60;
    int minutes = (remained / (1000*60)) % 60;
    int hours   = (remained / (1000*60*60)) % 24;
    seconds = seconds<0 ? 0:seconds;
    QString txtSec = QString::number(seconds);
    QString txtMin = QString::number(minutes);
    QString txtHou = QString::number(hours);

    if(txtHou.length() == 1) txtHou = '0'+txtHou;
    if(txtMin.length() == 1) txtMin = '0'+txtMin;
    if(txtSec.length() == 1) txtSec = '0'+txtSec;

    return txtHou+':'+txtMin+':'+txtSec;
}
void MainWindow::on_searchBar_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        this->musicItems[i]->selected = false;
    }
    if(ui->searchBar->text() == "") return;

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        int start = ui->listWidget->item(i)->text().indexOf(arg1, 0, Qt::CaseInsensitive);
        if(start >= 0 )
        {
            ui->listWidget->setCurrentRow(i);
            this->musicItems[i]->selected = true;
            ui->listWidget->scrollToItem(ui->listWidget->item(i),QAbstractItemView::PositionAtTop);
            break;
        }
    }
    search_string = arg1;
}

void MainWindow::monitoring()
{
    improveStatus = improvePlayer->state();
    int total = 0;
    int remain = 0;
    int playing = 0;
    int procUnit = 10;
    int lastIdx = processedCounts;
    if (lastIdx + procUnit < int(playlist.tracks.size())) lastIdx += procUnit;
    else lastIdx = int(playlist.tracks.size());
    for (int i=processedCounts; i < lastIdx; i++)
    {
        total+=this->musicItems[i]->music->getDuration();
        int remained = this->musicItems[i]->music->getDuration() - this->musicItems[i]->player->position();     
        musicPosInfo[i] = getTimeString(remained);
        this->musicItems[i]->lblPosition->setText(musicPosInfo[i]);
        musicDurInfo[i] = total;

        remain += remained;
        if (this->musicItems[i]->player->state() == QMediaPlayer::PlayingState){
            if (this->musicItems[i]->lblPosition->text()!="0:00:00")
                playing ++;
        }
    }
    if (total > 0) {
        processedCounts = lastIdx;
        totalDuration += total;
        remainDuration += remain;
    }

    if (playing>0){
        playerStatus = QMediaPlayer::PlayingState;
    }else{
        playerStatus = QMediaPlayer::StoppedState;
    }
    if (totalDuration > 0 && processedCounts == int(playlist.tracks.size()))
    {
        loaded = true;
        this->musicItems[0]->setStyleSheet("QFrame {"
                                           "color: white; "
                                           "background-color: blue;"
                                           "border: 1px solid white; "
                                           "}");
        ui->lblPosition->setText(this->musicItems[0]->lblPosition->text());
        mainPlayer->setMedia(QUrl::fromLocalFile(QString::fromStdString(this->musicItems[0]->music->getLocation())));
        ui->lblTitle->setText(ui->listWidget->item(0)->text());
        selectedMusicIndex = 0;

        ui->lblTotalTime->setText("Session Total Time "+getTimeString(totalDuration));
        ui->lblRemainTime->setText("Session Time Remaining "+getTimeString(remainDuration));

        remain=improvePlayer->duration()-improvePlayer->position();
        ui->improveDurationLbl->setText(getTimeString(remain));
        remain=improvePlayer->position();
        ui->improveProgressLbl->setText(getTimeString(remain));
        ui->improveSlider->setValue((double)improvePlayer->position()/improvePlayer->duration() * 1000);

        if (playerStatus == QMediaPlayer::StoppedState && monitor->isActive()) monitor->stop();
    }

    if(improvePlayer->state() == QMediaPlayer::StoppedState)
    {
        ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
//        if (playerStatus == QMediaPlayer::StoppedState && monitor->isActive()) monitor->stop();
    }else if (improvePlayer->state() == QMediaPlayer::PlayingState) {
        ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }else if (improvePlayer->state() == QMediaPlayer::PausedState) {
        ui->improveCtlBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        if (playerStatus == QMediaPlayer::StoppedState && monitor->isActive()) monitor->stop();
    }

    qDebug()<<"monitor updating....." << processedCounts;
}

void MainWindow::improveSeek(int pos)
{
    improvePlayer->setPosition(improvePlayer->duration() / 1000 * pos);
}

void MainWindow::improveNext()
{
    if (ui->improveList->rowCount()>0){
        int index = ui->improveList->currentRow();
        index++;
        if (index >= ui->improveList->rowCount() || playerStatus == QMediaPlayer::PlayingState) {
            return;
        }
        ui->improveList->setCurrentIndex(ui->improveList->model()->index(index, 0));
        ui->selectedImproveLbl->setText(ui->improveList->item(index, 0)->text().trimmed());
        improvePlayer->setMedia(QUrl::fromLocalFile(improvePath+"/"+ui->selectedImproveLbl->text()));
        improvePlayer->play();
        if (!monitor->isActive()) monitor->start();
        improveStatus = improvePlayer->state();
    }
}

void MainWindow::improvePre()
{
    if (ui->improveList->rowCount()>0){
        int index = ui->improveList->currentRow();
        index--;
        if (index < 0 || playerStatus == QMediaPlayer::PlayingState) {
            return;
        }
        ui->improveList->setCurrentIndex(ui->improveList->model()->index(index, 0));
        ui->selectedImproveLbl->setText(ui->improveList->item(index, 0)->text().trimmed());
        improvePlayer->setMedia(QUrl::fromLocalFile(improvePath+"/"+ui->selectedImproveLbl->text()));
        improvePlayer->play();        
        if (!monitor->isActive()) monitor->start();
        improveStatus = improvePlayer->state();
    }
}

void MainWindow::improveVolumeChange(int pos)
{
    improvePlayer->setVolume(pos);
}
void MainWindow::on_upgradeBtn_clicked()
{
    QString device_id = getMacAddress().replace(":","-");
    QString request="";
    foreach (QChar i, device_id)
    {
        QString c(i);
        if (c=="-") {
            request += c;
            continue;
        }
        bool ok;
        int v=c.toInt(&ok, 16);
        if (v>9)
            request += QString::number(qrand()%5+5)+QString::number(v%10);
        else
            request += QString::number(qrand()%5)+c;

    }

    QDialog * d = new QDialog();
    QVBoxLayout * vbox = new QVBoxLayout();
    QLabel * labelA = new QLabel("Activation Instructions\n"
                                    "Please copy the Request Code or Take a Photo and\n"
                                    "email it to us. Note: Allow up to 24hrs\n"
                                    "Email mu.serialrequest@gmail.com\n"
                                 "\nRequest Code");
    QLabel * labelB = new QLabel("\nSerial Code");
    QLabel * labelC = new QLabel("Serial codes are valid for a given number of days only. You can\n"
                                 "request additional days by contacting us on the above details.\n");
    QLineEdit * lineEditA = new QLineEdit(request);
    QLineEdit * lineEditB = new QLineEdit();

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                        | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), d, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), d, SLOT(reject()));

    vbox->addWidget(labelA);
    vbox->addWidget(lineEditA);
    vbox->addWidget(labelB);
    vbox->addWidget(lineEditB);
    vbox->addWidget(labelC);
    vbox->addWidget(buttonBox);

    d->setLayout(vbox);

    int result = d->exec();
    if(result == QDialog::Accepted)
    {
        QString serial_code=lineEditB->text();
        serial_code.replace("-","");
        QString serial_code_decrypt="";
        for (int i = 0;i<serial_code.length();i+=3) {
            serial_code_decrypt += serial_code.mid(i+1,2);
        }

        QString new_device_id = "";QString trial="";
        for (int i=0;i<serial_code_decrypt.length();i++) {
            int n = serial_code_decrypt[i].unicode()-'A';
            if (i % 4 == 1)
                trial += QString::number(n,16).toUpper();
            else
                new_device_id += QString::number(n,16).toUpper();
        }

        QDate curDate = QDate::currentDate();
        QDate baseDate = QDate::fromString("2010-10-14","yyyy-MM-dd");
        int activate_days = trial.toInt()-(curDate.toJulianDay()-baseDate.toJulianDay());
        if ((activate_days<0)||(device_id.replace("-","")!=new_device_id)){
            QMessageBox::critical(
                this,
                tr("Invalid License"),
                tr("Please contact with owner.\nTry to get valid license code.") );
             exit(0);
        }
        QSettings settings;
        settings.setValue("request",request);
        settings.setValue("serial",serial_code);
        settings.sync();
        QMessageBox::information (0, "Registered Success",
                                          QString("Program has been activated for %1 days.").arg(QString::number(activate_days+1)));
        QString title ="MU Player by Eventshd Version 3.1.9 --- (Your software will expire in " + QString::number(activate_days+1)+" days)";
        this->setWindowTitle(title);
    }
}

void MainWindow::on_convertAllBtn_clicked()
{
    for (int i=0; i < int(playlist.tracks.size());i++)
    {
//        if (this->musicItems[i]->player->duration()<=0)
//            this->musicItems[i]->converter->start();
    }
}

void MainWindow::onMusicListItemClicked(QListWidgetItem* item)
{
    if(mainPlayer->state() != QMediaPlayer::PlayingState && loaded == true)
    {
        int rownum = item->listWidget()->currentRow();
        ui->lblTitle->setText(ui->listWidget->item(rownum)->text());
        if (rownum != selectedMusicIndex)
        {
            if (selectedMusicIndex > -1)
            {
                this->musicItems[selectedMusicIndex]->setStyleSheet("QFrame {"
                                                                    "color: white; "
                                                                    "background-color: #2e2f33;"
                                                                    "border: 1px solid white; "
                                                                            "}");
            }
            selectedMusicIndex = rownum;
//            if (musicPlayed[selectedMusicIndex])
//            {
//                ui->btnPlay->setStyleSheet("width:50px; height:50px; border-image:url(:/played.png);");
//                //ui->btnPlay->setEnabled(false);
//            } else {
                this->musicItems[rownum]->setStyleSheet("QFrame {"
                                                        "color: white; "
                                                        "background-color: blue;"
                                                        "border: 1px solid white; "
                                                                "}");


                mainPlayer->setMedia(QUrl::fromLocalFile(QString::fromStdString(this->musicItems[rownum]->music->getLocation())));
                updater->start();
          //  }

            ui->slider->setValue(0);
        }
    }
}

void MainWindow::on_btnPlay_clicked()
{
    if (selectedMusicIndex > -1)
    {
        if(!this->musicItems[selectedMusicIndex]->chk->isChecked() && mainPlayer->state() != QMediaPlayer::PlayingState){
                return;
            }
            if (improveStatus == QMediaPlayer::PlayingState){
                QMessageBox msgBox(QMessageBox::Information,
                               "Warning",
                               "Please stop/pause improve player and try again");
                msgBox.setStyleSheet("color: silver; "
                                     "background-color: #302F2F; "
                                     "selection-background-color:#3d8ec9;"
                                     "selection-color: black;"
                                     "background-clip: border;"
                                     "border-image: none;"
                                     "outline: 0;"
                            "QMessageBox { background-color: rgb(51, 51, 51);}"
                                    "QMessageBox QLabel { color: rgb(200, 200, 200);}");
                msgBox.exec();
                msgBox.show();
                return;
            }
            if(mainPlayer->state() == QMediaPlayer::PlayingState)
            {
                mainPlayer->pause();
            }
            else
            {
                if (playerStatus == QMediaPlayer::PlayingState) {
                    QMessageBox msgBox(QMessageBox::Information,
                                   "Warning",
                                   "Only 1 Track can be played at a time");
                    msgBox.setStyleSheet("color: silver; "
                                         "background-color: #302F2F; "
                                         "selection-background-color:#3d8ec9;"
                                         "selection-color: black;"
                                         "background-clip: border;"
                                         "border-image: none;"
                                         "outline: 0;"
                                "QMessageBox { background-color: rgb(51, 51, 51);}"
                                        "QMessageBox QLabel { color: rgb(200, 200, 200);}");
                    msgBox.exec();
                    msgBox.show();
                    return;
                }
//                if (musicPlayed[selectedMusicIndex])
//                {
//                    return;
//                }
                this->musicItems[selectedMusicIndex]->music->setPlayed(false);
                mainPlayer->play();
                if (mainPlayer->duration()>0 && !updater->isActive()) updater->start();
                if (monitor->isActive()) monitor->stop();
            }
    }
}

void MainWindow::update()
{
    int remain = mainPlayer->duration()-mainPlayer->position();
    int totalremain = remainDuration - mainPlayer->position();

    ui->lblRemainTime->setText("Session Time Remaining "+getTimeString(totalremain));

    if (this->musicItems[selectedMusicIndex]->chk->isChecked()){
        ui->btnPlay->setEnabled(true);
    }else{
        ui->btnPlay->setEnabled(false);
    }
    if (mainPlayer->duration()<=0){
        ui->btnPlay->setEnabled(false);
    }else {
        ui->btnPlay->setEnabled(true);
    }

    if(mainPlayer->state() == QMediaPlayer::PlayingState)
    {
        ui->slider->setValue((double)mainPlayer->position()/mainPlayer->duration() * 1000);
        ui->lblPosition->setStyleSheet("QLabel {color : orange; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");
        ui->btnPlay->setStyleSheet("width:50px; height:50px; border-image:url(:/pause.png);");
        ui->slider->setEnabled(false);
    }
    else
    {
        ui->slider->setEnabled(true);
        ui->lblPosition->setStyleSheet("QLabel {color : #88c365; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");
        ui->btnPlay->setStyleSheet("width:50px; height:50px; border-image:url(:/play.png);");
    }

    QString timeString = getTimeString(remain);

    ui->lblPosition->setText(timeString);
    this->musicItems[selectedMusicIndex]->lblPosition->setText(timeString);

    if (remain <= 50 && mainPlayer->state() == QMediaPlayer::StoppedState)
    {

        if (ui->slider->value() ==0 && mainPlayer->duration()>0 && updater->isActive()) updater->stop();
        ui->lblPosition->setStyleSheet("QLabel {color : #f8363f; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");
        ui->btnPlay->setStyleSheet("width:50px; height:50px; border-image:url(:/played.png);");
        this->musicItems[selectedMusicIndex]->setStyleSheet("QFrame {"
                                                            "color: gray; "
                                                            "background-color: #1a1a1c;"
                                                            "border: 1px solid white; "
                                                                    "}");
        this->musicItems[selectedMusicIndex]->lblPosition->setStyleSheet("QLabel {color : red; font-size:40pt; font-weight:bold; padding:2px; border: 0px;}");
        this->musicItems[selectedMusicIndex]->lblPosition->setText(getTimeString(mainPlayer->duration()));
        musicPlayed[selectedMusicIndex] = 1;
        remainDuration -= mainPlayer->duration();
        if (autoplay == true)
        {
            int nextIndex = selectedMusicIndex + 1;
            if (nextIndex < musicChkInfo.size())
            {
                while (musicChkInfo[nextIndex] == 0 || musicPlayed[nextIndex] == 1)
                {
                    nextIndex ++;
                }
                this->musicItems[nextIndex]->setStyleSheet("QFrame {"
                                                           "color: white; "
                                                           "background-color: blue;"
                                                           "border: 1px solid white; "
                                                                   "}");
                selectedMusicIndex = nextIndex;
                ui->lblTitle->setText(ui->listWidget->item(nextIndex)->text());
                mainPlayer->setMedia(QUrl::fromLocalFile(QString::fromStdString(this->musicItems[nextIndex]->music->getLocation())));
            }
        }
        ui->slider->setValue(0);
    }
}


void MainWindow::on_onRadio_clicked()
{
    autoplay = true;
}

void MainWindow::on_offRadio_clicked()
{
    autoplay = false;
}

