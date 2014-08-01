#include <QCoreApplication>

#include "rss_creator.h"

// regex :: <li>\s{1,}<a href=[\'"]?([^\'" >]+)"\st
QRegExp item_url("<li>\\s{1,}<a href=[\\'\"]?([^\\'\" >]+)\"\\st");
// regex :: <a class="out_tn" href=[\'"]?([^\'" >]+)"( title=[\'"]?([^\'">]+)")?
QRegExp picture_url("<a class=\"out_tn\" href=[\\'\"]?([^\\'\" >]+)\"( title=[\\'\"]?([^\\'\">]+)\")?");
// regex :: <h1 [^\'>]+>([^\'<]+)<\/h1>
QRegExp picture_url_title("<h1 [^\\'>]+>([^\\'<]+)<\\/h1>");
// regex :: <\/h1>\s{0,}<a href=[\'"]?([^\'" >]+)"\s
QRegExp picture_url2("<\\/h1>\\s{0,}<a href=[\\'\"]?([^\\'\" >]+)\"\\s");
// regex :: \.jpe?g
QRegExp jpeg_ext("\\.jpe?g");

QLocale locale("en_US");

void get_picture_url(const QString& page, QString& url, QString& title) {
    url = QString();
    title = QString();
    if (picture_url.indexIn(page, 0) != -1) {
        //picture_url.capturedTexts();
        url = picture_url.cap(1);
    }
    if (picture_url_title.indexIn(page, 0) != -1) {
        title = picture_url_title.cap(1);
    }
}

void get_picture_url2(const QString& page, QString& url) {
    url = QString();
    if (picture_url2.indexIn(page, 0) != -1) {
        url = picture_url2.cap(1);
    }
}

QStringList get_items(const QString& page) {
    QStringList list;
    int pos = 0;
    while ((pos = item_url.indexIn(page, pos)) != -1) {
         list.push_back(QString("http://hdw.eweb4.com/") + item_url.cap(1));
         pos += item_url.matchedLength();
    }
    return list;
}

void rss_header(QTextStream& out, const QString& title = "Today images") {
    out << QString(
               "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
               "<rss version=\"2.0\"\n"
               "    xmlns:atom=\"http://www.w3.org/2005/Atom\"\n"
               "    xmlns:media=\"http://search.yahoo.com/mrss/\"\n"
               "    xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
               "    xmlns:creativeCommons=\"http://cyber.law.harvard.edu/rss/creativeCommonsRssModule.html\">\n"
               "\n"
               "<channel>\n"
               "<title>") << title << QString("</title>\n");
}

void rss_outer(QTextStream& out) {
    out << QString("</channel>\n"
                   "</rss>");
}

void rss_item(QTextStream& out, const QString& title, const QDateTime& date, const QUrl& url) {
    out << QString("<item><title>") << title << QString("</title>\n") <<
           QString("<pubDate>") << locale.toString(date, "ddd, dd MMM yyyy hh:mm:ss") << QString(" GMT</pubDate>\n") <<
           QString("<enclosure url=\"") << url.toString() << QString("\" type=\"image/jpeg\" /></item>\n");
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
//    QFile output("output.txt");
//    output.open(QIODevice::WriteOnly);
    QTextStream rss(stdout, QIODevice::WriteOnly);

    Downloader dnl;
    rss_header(rss);
    QUrl mainpage_url = QString("http://hdw.eweb4.com/top/");
    if (app.arguments().size() > 1)
        mainpage_url = app.arguments().at(1);

    QStringList items = get_items(dnl.download(mainpage_url));
    int count = 0;
    for(int i=0; count < 20 && i < items.size(); ++i) {
        QUrl page_url = QString(items.at(i));
        QString page = dnl.download(page_url);
        QString url, title;
        get_picture_url(page, url, title);
        if (url.indexOf(jpeg_ext) == -1) {
            if (url.indexOf(QString("pichost.me")) != -1) {
                page = dnl.download(url);
                get_picture_url2(page, url);
            }
        }
        if (url.indexOf(jpeg_ext) == -1)
            continue;
        rss_item(rss, title, QDateTime::currentDateTimeUtc(), url);
        count++;
    }

    rss_outer(rss);
    return 0; //app.exec();
}
