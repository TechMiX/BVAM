#include "vanitydb.h"

VanityDB::VanityDB() {
    init();
    version = DEFAULT_VERSION;
}

VanityDB::VanityDB(int verNum)
{
    init();
    version = verNum;
}

void VanityDB::init() {
    clear();
    recordDelim = "&\n";
    fieldDelim = "|";
    propertyDelim = "!\n";
}

void VanityDB::clear() {
    records.clear();
    data = "";
}

bool VanityDB::isEmpty() {
    return ((getRecordCount() == 0)?true:false);
}

int VanityDB::getRecordCount() {
    return records.count();
}

QStringList VanityDB::getAllFields() {
    return records.join(fieldDelim).split(fieldDelim);
}

QStringList VanityDB::getRecords() {
    return records;
}

QString VanityDB::getData() {
    return data;
}


int VanityDB::getVersion() {
    return version;
}

bool VanityDB::setEncryptedData(const QByteArray input, QString password) {
    QString decryptedData = QString(AESCrypt(input, password, true));
    return setData(decryptedData);
}

QByteArray VanityDB::getEncryptedData(QString password) {
    return AESCrypt(data.toLocal8Bit(), password);
}

QString VanityDB::getRecord(int recordNumber, VanityDBFields field) {
    if (!isEmpty())
        return records.at(recordNumber).split(fieldDelim).at(field);
    else
        return "";
}

void VanityDB::rebuildData() {
    // data [delim] version [delim] hashDigest
    data = records.join(recordDelim); // must be at first
    QByteArray digest = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5);

    data += propertyDelim + QString::number(version);
    data += propertyDelim + QString(digest.toHex()); // must be last property
}

bool VanityDB::setData(QString input) {
    QStringList properties = input.split(propertyDelim);
    if (!properties.size()>1)
        return false;

    QString tempData = properties.first();
    QByteArray digest = QCryptographicHash::hash(tempData.toUtf8(), QCryptographicHash::Md5);
    if (QString(digest.toHex()) != properties.last())
        return false;

    version = properties.at(1).toInt();
    records = tempData.split(recordDelim);
    rebuildData();

    return true;
}

void VanityDB::addRecord(QString networkByte,
                         QString prefix,
                         QString privateKey,
                         QString publicKey) {

    QStringList fields = (QStringList()
                          << networkByte
                          << prefix
                          << privateKey
                          << publicKey);

    records << fields.join(fieldDelim);
    rebuildData();
}

void VanityDB::removeAllRecords(QString networkByte,
                                QString prefix,
                                QString privateKey,
                                QString publicKey) {

    if (isEmpty())
        return;

    QStringList searchFields = (QStringList()
                                << networkByte
                                << prefix
                                << privateKey
                                << publicKey);

    for (int i=0; i<getRecordCount(); i++) {
        QStringList recordFields = records.at(i).split(fieldDelim);
        int cFields = 0, cMatched = 0;
        for (int j=0; j<recordFields.count(); j++) {
            if (searchFields.at(j) != "") {
                cFields++;
                if (recordFields.at(j) == searchFields.at(j))
                    cMatched++;
            }
        }
        if (cFields == cMatched) {
            records.removeAt(i);
            i--;
        }
    }
    rebuildData();
}

QByteArray VanityDB::AESCrypt(const QByteArray input, QString password, bool decrypt, QString salt) {
    QByteArray output;
    unsigned char ckeyHash[32];
    unsigned char ivector[32];
    int cLen = 0, fLen = 0;
    EVP_CIPHER_CTX cipherContext;

    EVP_CIPHER_CTX_init( &cipherContext );
    output.resize(input.length() + AES_BLOCK_SIZE + 100);
    SHA256(reinterpret_cast<const unsigned char*>(password.toStdString().c_str()),
           password.length(),
           ckeyHash);
    SHA256(reinterpret_cast<const unsigned char*>(QString(password + salt).toStdString().c_str()),
           password.length() + salt.length(),
           ivector);

    if (!decrypt) {
        EVP_EncryptInit_ex(&cipherContext,
                           EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(ckeyHash), ivector);

        EVP_EncryptUpdate(&cipherContext,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &cLen,
                          reinterpret_cast<const unsigned char*>(input.constData()),
                          input.length());

        EVP_CipherFinal_ex(&cipherContext,
                           reinterpret_cast<unsigned char*>(output.data()) + cLen,
                           &fLen);

    } else {
        EVP_DecryptInit_ex(&cipherContext,
                           EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(ckeyHash), ivector);

        EVP_DecryptUpdate(&cipherContext,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &cLen,
                          reinterpret_cast<const unsigned char*>(input.constData()),
                          input.length());

        EVP_DecryptFinal_ex(&cipherContext,
                           reinterpret_cast<unsigned char*>(output.data()) + cLen,
                           &fLen);

    }

    EVP_CIPHER_CTX_cleanup(&cipherContext);
    output.resize(cLen + fLen);

    return output;
}
