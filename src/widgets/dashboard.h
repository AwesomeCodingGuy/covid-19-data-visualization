#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;

class AppSettings;
class DownloadManager;

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    explicit Dashboard(AppSettings &settings, QWidget *parent = nullptr);

private:
    void initWidgets();

    QString getFileUpdateString(const QString &filename) const;

signals:
    void sigLoadData();

private slots:
    void updateFileInfos();
    void downloadFiles();

private:
    AppSettings &appSettings;
    DownloadManager *downloadManager;

    QGridLayout *gLayout;
    QGridLayout *gGermanyFileLayout;
    QGridLayout *gJhuFileLayout;

    QLabel *germanyRepoUrlLabel;
    QLabel *jhuRepoUrlLabel;
    QLabel* germanyFileInfoLabels[5];
    QLabel* jhuFileInfoLabels[4];

    // download gui
    QPushButton *loadDataButton;
    QPushButton *downloadDataButton;
};

#endif // DASHBOARD_H
