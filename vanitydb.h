#ifndef VANITYDB_H
#define VANITYDB_H

#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

#define DEFAULT_VERSION 3

class VanityDB
{
public:
    enum VanityDBFields {
        networkByte = 0,
        prefix = 1,
        privateKey = 2,
        publicKey = 3,
    };

    VanityDB();
    VanityDB(int);
    void clear();
    bool setData(QString);
    bool setEncryptedData(const QByteArray input, QString password);
    void addRecord(QString networkByte,
                   QString prefix,
                   QString privateKey,
                   QString publicKey);
    void removeAllRecords(QString networkByte = "",
                          QString prefix = "",
                          QString privateKey = "",
                          QString publicKey = "");
    static QByteArray AESCrypt(const QByteArray input,
                               QString password,
                               bool decrypt = false,
                               QString salt = "BVAMBVAMBVAMBVAM");
    QByteArray getEncryptedData(QString password);
    QString getData();
    QString getRecord(int recordNumber, VanityDBFields field = prefix);
    QStringList getRecords();
    QStringList getAllFields();
    int getRecordCount();
    bool isEmpty();
    int getVersion();

private:
    QString data;
    QStringList records;
    QString recordDelim;
    QString fieldDelim;
    QString propertyDelim;
    int version;
    void rebuildData();
    void init();
};

#endif // VANITYDB_H
