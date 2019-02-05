#include "ezgraver_v4.h"

#include <QDebug>
#include <QByteArray>
#include <QBuffer>

#include "specifications.h"

namespace Ez {

    void EzGraverV4::reset() {
        qDebug() << "resetting";
        _transmit(QByteArray::fromRawData("\xFF\x04\x01\x00", 4));
    }

    void EzGraverV4::pause() {
        qDebug() << "pausing engrave process";
        _transmit(QByteArray::fromRawData("\xFF\x01\x02\x00", 4));
    }

    void EzGraverV4::home() {
        qDebug() << "moving to home";
        _transmit(QByteArray::fromRawData("\xFF\x0A\x00\x00", 4));
        _transmit(QByteArray::fromRawData("\xFF\x0B\x00\x00", 4));
    }


    void EzGraverV4::center() {
        qDebug() << "moving to center";
        _transmit(QByteArray::fromRawData("\xFF\x02\x01\x00", 4));
    }

    void EzGraverV4::preview() {
        qDebug() << "drawing image preview";
        _transmit(QByteArray::fromRawData("\xFF\x02\x02\x00", 4));
    }

    void EzGraverV4::up() {
        qDebug() << "moving up";
        _transmit(QByteArray::fromRawData("\xFF\x03\x01\x00", 4));
    }

    void EzGraverV4::down() {
        qDebug() << "moving down";
        _transmit(QByteArray::fromRawData("\xFF\x03\x02\x00", 4));
    }

    void EzGraverV4::left() {
        qDebug() << "moving left";
        _transmit(QByteArray::fromRawData("\xFF\x03\x03\x00", 4));
    }

    void EzGraverV4::right() {
        qDebug() << "moving right";
        _transmit(QByteArray::fromRawData("\xFF\x03\x04\x00", 4));
    }


    void EzGraverV4::_setBurnTime(unsigned char const& burnTime) {
        if(burnTime < 0x01 || burnTime > 0xF0) {
            throw new std::out_of_range("burntime out of range");
        }
        qDebug() << "setting burn time to:" << static_cast<int>(burnTime);

        QByteArray payload{"\xFF\x05?\x00", 4};
        payload[2] = burnTime;
        _transmit(payload);
    }

    // ============================================================

    void EzGraverV4::start(unsigned char const& burnTime) {
        if (true) {
            //@@ _setBurnTime(burnTime);
            qDebug() << "requesting double speed";
            _transmit(QByteArray::fromRawData("\xFF\x0E\x00\x01", 4));
            sleep(10);
            setBaudRate(QSerialPort::Baud115200);
            sleep(10);
            qDebug() << "requesting upload mode";
            _transmit(QByteArray::fromRawData("\xFF\x06\x01\x01", 4));
        } else {
            qDebug() << "starting engrave process";
            _transmit(QByteArray::fromRawData("\xFF\x01\x01\x00", 4));
        }
    }

    int EzGraverV4::erase() {
        qDebug() << "erasing EEPROM";
        _transmit(QByteArray::fromRawData("\xFF\x06\x01\x01", 4));
        return 50;
    }

    int EzGraverV4::uploadImage(QImage const& originalImage) {
        qDebug() << "converting image to bitmap";
        QImage image{originalImage
                .scaled(Ez::Specifications::ImageWidth, Ez::Specifications::ImageHeight)
                .mirrored()
                .convertToFormat(QImage::Format_Mono)};
        QByteArray bytes{};
        QBuffer buffer{&bytes};
        image.save(&buffer, "BMP");

        // protocol v4 neither needs the BMP header nor the invertion of the pixels.
        return EzGraver::uploadImage(bytes.mid(62));
    }

    void EzGraverV4::dataRecieved(QByteArray const& data) {
        //qDebug() << "EzGraverV4::received" << data.size() << "bytes:" << data.toHex();
    }

}
