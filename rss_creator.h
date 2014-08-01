#include <QDir>
#include <QDataStream>
#include <QEventLoop>
#include <QtNetwork>

class Downloader: public QObject {
    Q_OBJECT
private:
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QByteArray data;
    QDataStream *dataStream;

private slots:
    void httpReadyRead() {
        (*dataStream) << reply->readAll();
    }
public:
    QString download(QUrl url) {
        while (1) {
            reply = qnam.get(QNetworkRequest(url));
            dataStream = new QDataStream(&data, QIODevice::WriteOnly);
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            delete dataStream;

            QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            if (reply->error())
                return QString();
            else if (!redirectionTarget.isNull()) {
                url = url.resolved(redirectionTarget.toUrl());
                // restart
            }
            else {
                for(int i=0; i<data.size(); ++i)
                    if (!data[i])
                        data[i] = '\n';
                QString ret = QString(data);
                data = QByteArray();
                return ret;
            }
            delete reply;
        }
    }
};
