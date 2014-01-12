#ifndef DROPBOXAPI_H
#define DROPBOXAPI_H

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QUrl>
#include <QVariantMap>

class QNetworkAccessManager;
class QNetworkReply;

/* Class implementing (parts of) the Dropbox Core API.
 */
class DropboxApi : public QObject
{
    Q_OBJECT
public:
    enum ErrorCode
    {
        AuthorizationError,
        InvalidAuthorizationUri,
        InvalidAuthorizationState
    };

    enum DropboxRoot
    {
        Dropbox,
        Sandbox
    };

    struct AccountInfo
    {
        QString displayName;
        QString uid;
        QString country;
        quint64 quota;
        quint64 quotaShared;
        quint64 quoteNormal;
    };

    struct Metadata
    {
        QString path;
        quint64 bytes;
        bool isDir;
        QString mimeType;
        QString rev;
        QString icon;
        QString thumb;
        QDateTime mtime;
        QList<Metadata> contents;
    };

    DropboxApi(DropboxRoot root = Dropbox,
               QObject* parent = 0);

    /* Performs an OAuth2 authorization. If uri is empty, the signal
     * authorizationRequest will be emitted with an URL to take the user to
     * for authorizing, and a redirection URI where the user will be sent to,
     * once authorized.
     * If uri is not empty, it should be the full redirection URI with its
     * parameters from the server.
     */
    void authorize(const QUrl& uri = QUrl());

    /* Returns the access token of a successful authorization.
     */
    QString accessToken() const { return myAccessToken; }
    /* Returns the user ID of a successful authorization.
     */
    QString userId() const { return myUserId; }

    /* Sets the access token and user ID. This method must be invoked manually
     * if you can skip the authorization step because you already have a valid
     * token and user ID.
     */
    void setAccessToken(const QString& accessToken, const QString& userId);

    /* Requests the account information. Requesting may be useful to obtain
     * current quota values.
     * Emits signal accountInfoReceived.
     */
    void requestAccountInfo();

    /* Requests the meta data of the given path.
     * Emits signal metadataReceived.
     */
    void requestMetadata(const QString& path);

    /* Creates a folder at the given path.
     */
    void createFolder(const QString& path);

    /* Moves a file.
     */
    void moveFile(const QString& oldPath, const QString& newPath);

    /* Deletes a file.
     */
    void deleteFile(const QString& path);

signals:
    void authorizationRequest(const QUrl& url, const QUrl& redirectUri);
    void authorized();

    void accountInfoReceived(const DropboxApi::AccountInfo& info);
    void metadataReceived(const DropboxApi::Metadata& metadata);
    /* Gets emitted with the new folder's path when a folder was created.
     * Gets emitted with an empty string, if the action failed.
     */
    void folderCreated(const QString& path);
    void fileMoved(const QString& newPath);
    void fileDeleted(const QString& path);

    void error(DropboxApi::ErrorCode error);

private:
    enum RequestMethod
    {
        DELETE,
        GET,
        POST,
        PUT
    };

    QNetworkReply* sendRequest(RequestMethod method,
                               const QUrl& url,
                               const QByteArray& payload = QByteArray());
    QVariantMap getReplyMap(QObject* sender);
    Metadata parseMetadata(const QVariantMap& map) const;

private slots:
    void slotRequestFinished(QNetworkReply* reply);

    void slotAccountInfoReceived();
    void slotMetadataReceived();
    void slotFolderCreated();
    void slotFileMoved();
    void slotFileDeleted();

private:
    QString myAppKey;
    QString myAppSecret;
    QString myAuthorizeState;

    QString myAccessToken;
    QString myUserId;

    QString myRoot;
    QSharedPointer<QNetworkAccessManager> myNetworkAccessManager;

    static QMap<QString, QString> thePathHashes;
    static QMap<QString, QByteArray> theHashCache;
};

#endif // DROPBOXAPI_H
