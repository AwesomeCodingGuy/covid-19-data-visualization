#include "downloadmanager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

DownloadManager::DownloadManager(const QString &folder, QObject *parent)
    : QObject(parent)
    , downloadFolder(folder)
    , currentDownload(nullptr)
{
    qDebug() << "Download Folder: " << folder;
    QDir dir(folder);
    if(!dir.exists()) {
        dir.mkpath(folder);
    }
}

DownloadManager::~DownloadManager()
{

}

void DownloadManager::setDownloadFolder(const QString &folder)
{
    downloadFolder = folder;
}

void DownloadManager::appendFiles(QStringList files)
{
    for(const QString &file : files) {
        downloadQueue.enqueue(QUrl::fromEncoded(file.toLocal8Bit()));
    }
}

void DownloadManager::startDownload()
{
    startNextDownload();
}

void DownloadManager::startNextDownload()
{
    // check if there are any files in the download queue, else emit finished signal
    if(downloadQueue.isEmpty()) {
        emit downloadsFinished();
        return;
    }

    // get next url
    QUrl url = downloadQueue.dequeue();

    // get a filename from url
    QString filename = saveFilename(url);
    outputFile.setFileName(QString("%1/%2").arg(downloadFolder, filename));
    if(!outputFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        // on error this downlaod will be skipped
        qDebug() << "Problem opening file." << qPrintable(outputFile.errorString());
        startNextDownload();
        return;
    }

    QNetworkRequest request(url);
    currentDownload = manager.get(request);
    connect(currentDownload, &QNetworkReply::downloadProgress,
            this, &DownloadManager::downloadProgress);
    connect(currentDownload, &QNetworkReply::finished,
            this, &DownloadManager::downloadFinished);
    connect(currentDownload, &QNetworkReply::readyRead,
            this, &DownloadManager::downloadReadyRead);
    connect(currentDownload, &QNetworkReply::sslErrors,
            this, &DownloadManager::sslErrors);
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit fileProgress(bytesReceived, bytesTotal);
}

void DownloadManager::downloadFinished()
{
    emit fileFinished();
    outputFile.close();

    if(currentDownload->error()) {
        outputFile.remove();
    } else {
        if(isHttpRedirect()) {
            reportRedirect();
            outputFile.remove();
        }
    }

    currentDownload->deleteLater();
    startNextDownload();
}

void DownloadManager::downloadReadyRead()
{
    outputFile.write(currentDownload->readAll());
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
    for(const QSslError &error : sslErrors)
        qDebug() << qPrintable(error.errorString());
}

QString DownloadManager::saveFilename(const QUrl &url)
{
    // retrieve the basename of the file from url
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    // if the basename is empty, just name it download
    if(basename.isEmpty()) {
        basename = "download";
    }

    return basename;
}

bool DownloadManager::isHttpRedirect() const
{
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301
            || statusCode == 302
            || statusCode == 303
            || statusCode == 305
            || statusCode == 307
            || statusCode == 308;
}

void DownloadManager::reportRedirect()
{
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QUrl requesturl = currentDownload->request().url();
    qDebug() << "Request: " << requesturl.toDisplayString()
             << " was redirected with code: " << statusCode;
    QVariant target = currentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(target.isValid()) {
        return;
    }
    QUrl redirectUrl = target.toUrl();
    if(!redirectUrl.isRelative()) {
        redirectUrl = requesturl.resolved(redirectUrl);
    }
    qDebug() << "Redirected to: " << redirectUrl.toDisplayString();
}
