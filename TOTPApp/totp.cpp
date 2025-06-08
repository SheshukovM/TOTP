#include "totp.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QtMath>
#include <QMessageBox>



QByteArray base32Decode(const QString &base32)
{
    static const char base32Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    QByteArray result;
    int buffer = 0;
    int bitsLeft = 0;

    for (QChar c : base32.toUpper()) {
        if (c == '=') break;

        int val = -1;
        for (int i = 0; i < 32; ++i) {
            if (base32Chars[i] == c.toLatin1()) {
                val = i;
                break;
            }
        }
        if (val == -1) continue;

        buffer <<= 5;
        buffer |= val;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            bitsLeft -= 8;
            result.append((buffer >> bitsLeft) & 0xFF);
        }
    }
    return result;
}

QByteArray hmacSha1(const QByteArray &key, const QByteArray &message)
{
    const int blockSize = 64;

    QByteArray key0 = key;
    if (key0.size() > blockSize)
        key0 = QCryptographicHash::hash(key0, QCryptographicHash::Sha1);

    if (key0.size() < blockSize)
        key0.append(QByteArray(blockSize - key0.size(), char(0x00)));

    QByteArray o_key_pad(blockSize, char(0x5c));
    QByteArray i_key_pad(blockSize, char(0x36));

    for (int i = 0; i < blockSize; ++i) {
        o_key_pad[i] ^= key0[i];
        i_key_pad[i] ^= key0[i];
    }

    QByteArray innerHash = QCryptographicHash::hash(i_key_pad + message, QCryptographicHash::Sha1);
    return QCryptographicHash::hash(o_key_pad + innerHash, QCryptographicHash::Sha1);
}

static quint32 dynamicTruncate(const QByteArray &hmacResult)
{
    int offset = hmacResult.at(hmacResult.size() - 1) & 0x0F;
    quint32 binary = ((quint8)hmacResult[offset] & 0x7F) << 24
                     | ((quint8)hmacResult[offset + 1] & 0xFF) << 16
                     | ((quint8)hmacResult[offset + 2] & 0xFF) << 8
                     | ((quint8)hmacResult[offset + 3] & 0xFF);
    return binary;
}

QString generateTOTP(const QString &base32Secret, int timeStepSeconds, int digits)
{
    QByteArray key = base32Decode(base32Secret);

    qint64 unixTime = QDateTime::currentSecsSinceEpoch();
    quint64 counter = unixTime / timeStepSeconds;

    QByteArray msg(8, 0);
    for (int i = 7; i >= 0; --i) {
        msg[i] = static_cast<char>(counter & 0xFF);
        counter >>= 8;
    }

    QByteArray hmacResult = hmacSha1(key, msg);
    quint32 binCode = dynamicTruncate(hmacResult);
    quint32 otp = binCode % static_cast<quint32>(qPow(10, digits));

    return QString("%1").arg(otp, digits, 10, QChar('0'));
}

