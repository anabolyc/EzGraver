#include "ezgraver.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QBitmap>
#include <QBuffer>

#include <iterator>
#include <algorithm>
#include <functional>

#include "specifications.h"

namespace Ez {

EzGraver::EzGraver(std::shared_ptr<QSerialPort> serial) : _serial{serial} {}

void EzGraver::start(unsigned char const& burnTime) {
    _setBurnTime(burnTime);
    qDebug() << "starting engrave process";
    _transmit(0xF1);
}

void EzGraver::_setBurnTime(unsigned char const& burnTime) {
    if(burnTime < 0x01 || burnTime > 0xF0) {
        throw new std::out_of_range("burntime out of range");
    }
    qDebug() << "setting burn time to:" << static_cast<int>(burnTime);
    _transmit(burnTime);
}

void EzGraver::pause() {
    qDebug() << "pausing engrave process";
    _transmit(0xF2);
}

void EzGraver::reset() {
    qDebug() << "resetting";
    _transmit(0xF9);
}

void EzGraver::home() {
    qDebug() << "moving to home";
    _transmit(0xF3);
}

void EzGraver::center() {
    qDebug() << "moving to center";
    _transmit(0xFB);
}

void EzGraver::preview() {
    qDebug() << "drawing image preview";
    _transmit(0xF4);
}

int EzGraver::erase() {
    qDebug() << "erasing EEPROM";
    _transmit(QByteArray{8, '\xFE'});
    return 6000;
}

int EzGraver::uploadImage(QImage const& originalImage) {
    qDebug() << "converting image to bitmap";
    QImage image{originalImage
            .scaled(Ez::Specifications::ImageWidth, Ez::Specifications::ImageHeight)
            .mirrored()
            .convertToFormat(QImage::Format_Mono)};
    image.invertPixels();

    QByteArray bytes{};
    QBuffer buffer{&bytes};
    image.save(&buffer, "BMP");
    return uploadImage(bytes);
}

int EzGraver::uploadImage(QByteArray const& image) {
    qDebug() << "uploading image";
    // Data is chunked in order to get at least some progress updates
    _transmit(image, 8192);
    return image.size();
}

void EzGraver::awaitTransmission(int msecs) {
    _serial->waitForBytesWritten(msecs);
}

std::shared_ptr<QSerialPort> EzGraver::serialPort() {
    return _serial;
}

void EzGraver::_transmit(unsigned char const& data) {
    _transmit(QByteArray{1, static_cast<char>(data)});
}

void EzGraver::_transmit(QByteArray const& data) {
    qDebug() << "transmitting" << data.size() << "bytes:" << data.toHex();
    _serial->write(data);
    _serial->flush();
}

void EzGraver::_transmit(QByteArray const& data, int chunkSize) {
    qDebug() << "transmitting" << data.size() << "bytes in chunks of size" << chunkSize;
    for(int i{0}; i < data.size(); i += chunkSize) {
        _serial->write(data.mid(i, chunkSize));
        _serial->flush();
    }
}

void EzGraver::dataRecieved(QByteArray const& data) {
    qDebug() << "EzGraver::received" << data.size() << "bytes:" << data.toHex();
}

void EzGraver::sleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

void EzGraver::setBaudRate(qint32 baudRate){
    _serial->setBaudRate(baudRate, QSerialPort::AllDirections);
}

EzGraver::~EzGraver() {
    qDebug() << "EzGraver is being destroyed, closing serial port";
    _serial->close();
}

}

