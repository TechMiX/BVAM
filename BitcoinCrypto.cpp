#include "BitcoinCrypto.h"

BitcoinCrypto::BitcoinCrypto()
{
    // init secp256k1 that is an specific curve used in Bitcoin
    // defined as T = (p,a,b,G,n,h)
    keyPairs = NULL;
    keyPairs = EC_KEY_new_by_curve_name(NID_secp256k1);
}

void BitcoinCrypto::generateKeyPairs() {
    // Initiate the curve
    BitcoinCrypto();

    // This is where point G is multiplied by a big number
    // Q = d * G
    // 'Q' is the public key, which is a point
    // and 'd' is the private key, which is a big number
    EC_KEY_generate_key(keyPairs);

    makePrivateKeyString();

    makePublicKeyString();
}

void BitcoinCrypto::mergeWithPrivateKey(QString solution, bool multiply) {
    if(!isHex(solution))
        return;

    BIGNUM bn, groupOrder, finalBn;
    BIGNUM *pointerToBn;
    BN_CTX *bnctx;

    BN_init(&bn);
    BN_init(&finalBn);
    BN_init(&groupOrder);

    pointerToBn = &bn;
    BN_hex2bn(&pointerToBn, solution.toStdString().c_str());

    bnctx = BN_CTX_new();
    EC_GROUP_get_order(EC_KEY_get0_group(keyPairs), &groupOrder, NULL);

    if (multiply == false)
        BN_mod_add(&finalBn, EC_KEY_get0_private_key(keyPairs), &bn, &groupOrder, bnctx);
    else
        BN_mod_mul(&finalBn, EC_KEY_get0_private_key(keyPairs), &bn, &groupOrder, bnctx);

    EC_KEY_set_private_key(keyPairs, &finalBn);

    EC_POINT* pubKey = EC_POINT_new(EC_KEY_get0_group(keyPairs));
    EC_POINT_mul(EC_KEY_get0_group(keyPairs), pubKey, &finalBn, NULL, NULL, NULL);
    EC_KEY_set_public_key(keyPairs, pubKey);

    BN_clear_free(&bn);
    BN_clear_free(&groupOrder);
    BN_clear_free(&finalBn);
    BN_CTX_free(bnctx);

    makePrivateKeyString();
    makePublicKeyString();
}

void BitcoinCrypto::mergeWithPublicKey(QString solution, bool multiply) {
    if(!isHex(solution))
        return;

    BIGNUM bn;
    BIGNUM *pointerToBn;

    BN_init(&bn);

    pointerToBn = &bn;
    BN_hex2bn(&pointerToBn, solution.toStdString().c_str());

    EC_POINT* pubKeyFromSolution = EC_POINT_new(EC_KEY_get0_group(keyPairs));
    EC_POINT* finalPubKey = EC_POINT_new(EC_KEY_get0_group(keyPairs));

    EC_POINT_mul(EC_KEY_get0_group(keyPairs), pubKeyFromSolution, &bn, NULL, NULL, NULL);

    if (multiply == false)
        EC_POINT_add(EC_KEY_get0_group(keyPairs), finalPubKey, pubKeyFromSolution, EC_KEY_get0_public_key(keyPairs), NULL);
    else
        EC_POINT_mul(EC_KEY_get0_group(keyPairs), finalPubKey, NULL, EC_KEY_get0_public_key(keyPairs), &bn, NULL);

    EC_KEY_set_public_key(keyPairs, finalPubKey);

    BN_clear_free(&bn);

    makePublicKeyString();
    privateKeyString = "";
}

void BitcoinCrypto::makePrivateKeyString() {
    privateKeyString = "";
    privateKeyString = QString(QByteArray(BN_bn2hex(EC_KEY_get0_private_key(keyPairs)))).toUpper();
}

void BitcoinCrypto::makePublicKeyString() {
    int len = -1;
    unsigned char publicKeyChars[1024]; // actual size of key is 520bit
    unsigned char* pointerToPublicKeyChars = &publicKeyChars[0];
    publicKeyString = "";

    len = i2o_ECPublicKey(keyPairs, &pointerToPublicKeyChars);
    publicKeyString = QString(QByteArray::fromRawData(reinterpret_cast<const char*>(publicKeyChars), len).toHex()).toUpper();
}

QString BitcoinCrypto::getPublicKey() {
    return publicKeyString;
}

QString BitcoinCrypto::getPrivateKey(bool base58encoded) {
    QString result;
    if (!base58encoded)
        result = privateKeyString;
    else {
        unsigned char eckey_buf[128];
        const BIGNUM *bn;
        int nbytes;

        bn = EC_KEY_get0_private_key(keyPairs);

        eckey_buf[0] = 128;
        nbytes = BN_num_bytes(bn);

        if (nbytes < 32)
            memset(eckey_buf + 1, 0, 32 - nbytes);

        BN_bn2bin(bn, &eckey_buf[33 - nbytes]);
        result = encodeToBase58String(eckey_buf, 33);
    }
    return result;
}

void BitcoinCrypto::setPublicKey(QString pk) {
    if(!isHex(pk))
        return;

    EC_POINT* pubKey;
    pubKey = EC_POINT_new(EC_KEY_get0_group(keyPairs));
    EC_POINT_hex2point(EC_KEY_get0_group(keyPairs), pk.toStdString().c_str(), pubKey, NULL);

    if (!EC_POINT_is_on_curve(EC_KEY_get0_group(keyPairs), pubKey, NULL))
        return;

    EC_KEY_set_public_key(keyPairs, pubKey);

    makePublicKeyString();
    privateKeyString = "";
}

void BitcoinCrypto::setPrivateKey(QString prk) {
    if(!isHex(prk))
        return;

    BIGNUM bn;
    BIGNUM *pointerToBn;

    BN_init(&bn);
    pointerToBn = &bn;
    BN_hex2bn(&pointerToBn, prk.toStdString().c_str());
    EC_KEY_set_private_key(keyPairs, pointerToBn);

    EC_POINT* pubKey = EC_POINT_new(EC_KEY_get0_group(keyPairs));
    EC_POINT_mul(EC_KEY_get0_group(keyPairs), pubKey, pointerToBn, NULL, NULL, NULL);
    EC_KEY_set_public_key(keyPairs, pubKey);

    BN_clear_free(&bn);

    makePrivateKeyString();
    makePublicKeyString();
}

bool BitcoinCrypto::isHex(QString s) {
    int pos = 0;
    bool result = true;

    QRegExp rx("((\\d)*([A-F])*)*");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    QRegExpValidator* hexValidator = new QRegExpValidator(rx, 0);

    if (hexValidator->validate(s, pos) == QValidator::Invalid)
        result = false;

    return result;
}

QString BitcoinCrypto::getBitcoinAddress() {
    unsigned char eckey_buf[128], *pend;
    unsigned char binres[21] = {0,};
    unsigned char hash1[32];
    int len;
    void* buf;

    pend = eckey_buf;

    EC_POINT_point2oct(EC_KEY_get0_group(keyPairs),
                       EC_KEY_get0_public_key(keyPairs),
                       POINT_CONVERSION_UNCOMPRESSED,
                       eckey_buf,
                       sizeof(eckey_buf),
                       NULL);
    pend = eckey_buf + 0x41;
    binres[0] = 0;
    SHA256(eckey_buf, pend - eckey_buf, hash1);
    RIPEMD160(hash1, sizeof(hash1), &binres[1]);

    len = sizeof(binres);
    buf = &binres;

    return encodeToBase58String(buf, len);
}

// Thanks to samr7 <samr7@cs.washington.edu>
QString BitcoinCrypto::encodeToBase58String(void* buf, int len) {
    unsigned char *binres2;
    unsigned char hash1[32];
    unsigned char hash2[32];
    char b58_alphabet[59] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    char result[128];
    int brlen, zpfx, d, p;

    BN_CTX *bnctx;
    BIGNUM *bn, *bndiv, *bntmp;
    BIGNUM bna, bnb, bnbase, bnrem;

    bnctx = BN_CTX_new();
    BN_init(&bna);
    BN_init(&bnb);
    BN_init(&bnbase);
    BN_init(&bnrem);
    BN_set_word(&bnbase, 58);

    bn = &bna;
    bndiv = &bnb;

    brlen = (2 * len) + 4;
    binres2 = (unsigned char*) malloc(brlen);
    memcpy(binres2, buf, len);

    SHA256(binres2, len, hash1);
    SHA256(hash1, sizeof(hash1), hash2);
    memcpy(&binres2[len], hash2, 4);

    BN_bin2bn(binres2, len + 4, bn);

    for (zpfx = 0; zpfx < (len + 4) && binres2[zpfx] == 0; zpfx++);

    p = brlen;
    while (!BN_is_zero(bn)) {
        BN_div(bndiv, &bnrem, bn, &bnbase, bnctx);
        bntmp = bn;
        bn = bndiv;
        bndiv = bntmp;
        d = BN_get_word(&bnrem);
        binres2[--p] = b58_alphabet[d];
    }

    while (zpfx--) {
        binres2[--p] = b58_alphabet[0];
    }

    memcpy(result, &binres2[p], brlen - p);
    result[brlen - p] = '\0';

    free(binres2);
    BN_clear_free(&bna);
    BN_clear_free(&bnb);
    BN_clear_free(&bnbase);
    BN_clear_free(&bnrem);
    BN_CTX_free(bnctx);

    return QString::fromAscii(result);

}
