#ifndef BICTCOINCRYPTO_H
#define BICTCOINCRYPTO_H

#include <QString>
#include <QByteArray>
#include <QRegExpValidator>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>

class BitcoinCrypto
{
public:
    BitcoinCrypto();
    void generateKeyPairs();
    void setPublicKey(QString);
    void setPrivateKey(QString);
    QString getPublicKey();
    QString getPrivateKey(bool base58encoded = false);
    QString getBitcoinAddress();
    bool isHex(QString);

    // useful for split-key vanity address generation
    void mergeWithPrivateKey(QString solution, bool multiply = false);
    void mergeWithPublicKey(QString solution, bool multiply = false);

private:
    EC_KEY *keyPairs;
    void makePrivateKeyString();
    void makePublicKeyString();
    QString encodeToBase58String(void* buf, int len);
    QString publicKeyString;
    QString privateKeyString;
};

#endif // BICTCOINCRYPTO_H
