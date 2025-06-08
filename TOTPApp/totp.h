#ifndef TOTP_H
#define TOTP_H

#include <QString>
#include <QByteArray>

QByteArray base32Decode(const QString &base32);
QByteArray hmacSha1(const QByteArray &key, const QByteArray &message);
QString generateTOTP(const QString &base32Secret, int timeStepSeconds = 60, int digits = 6);

#endif // TOTP_H
