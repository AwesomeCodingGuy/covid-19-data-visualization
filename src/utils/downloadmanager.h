#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QUrl>
#include <QFile>
#include <QNetworkReply>

class DownloadManager : public QObject
{
    Q_OBJECT

    QNetworkAccessManager manager;
    QVector<QNetworkReply*> currentDownloads;

public:
    DownloadManager(const QString &folder, QObject *parent = nullptr);
    ~DownloadManager();

    void setDownloadFolder(const QString &folder);
    void appendFiles(QStringList files);

    void startDownload();

private slots:
    void startNextDownload();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();
    void sslErrors(const QList<QSslError> &sslErrors);

private:
    static QString saveFilename(const QUrl &url);
    bool isHttpRedirect() const;
    void reportRedirect();

signals:
    void fileFinished();
    void fileProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadsFinished();

private:
    QString downloadFolder;
    QQueue<QUrl> downloadQueue;
    QFile outputFile;
    QNetworkReply *currentDownload;
};

#endif // DOWNLOADMANAGER_H
