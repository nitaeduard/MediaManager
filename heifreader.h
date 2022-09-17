#ifndef HEIFREADER_H
#define HEIFREADER_H

#include <QtGui/QImage>

#include "exif.h"

#include <libheif/heif.h>

class HEIFReader
{
public:
    enum class Format
    {
        None,
        Heif,
        HeifSequence,
        Heic,
        HeicSequence,
    };

    easyexif::EXIFInfo info;

    HEIFReader(QString fileName);

    bool read(QImage* destImage);

private:
    QString fileName;
    struct ReadState
    {
        ReadState(QByteArray&& data,
                  std::shared_ptr<heif_context>&& ctx,
                  std::vector<heif_item_id>&& ids,
                  int index);

        const QByteArray fileData;
        const std::shared_ptr<heif_context> context;
        const std::vector<heif_item_id> idList;
        int currentIndex{};
    };

    std::unique_ptr<ReadState> _readState;  // non-null iff context is loaded

    /**
     * Reads data from device. Creates read state.
     */
    void loadContext();
};

#endif // HEIFREADER_H
