#include "heifreader.h"

#include <QtCore/QSize>
#include <QtCore/QVariant>

#include <libheif/heif.h>
#include "exif.h"

#include <QtCore/QFile>

namespace {

static_assert(heif_error_Ok == 0, "heif_error_Ok assumed to be 0");

template<class T, class D>
std::unique_ptr<T, D> wrapPointer(T* ptr, D deleter)
{
    return std::unique_ptr<T, D>(ptr, deleter);
}

template<class... As>
heif_error readContext(As... as)
{
#if LIBHEIF_NUMERIC_VERSION >= 0x01030000
    return heif_context_read_from_memory_without_copy(as...);
#else
    return heif_context_read_from_memory(as...);
#endif
}

}  // namespace

HEIFReader::HEIFReader(QString fileName): fileName(fileName)
{

}

bool HEIFReader::read(QImage* destImage)
{
    if (!destImage) {
        qWarning("QHeifHandler::read() QImage to read into is null");
        return false;
    }

    loadContext();

    if (!_readState) {
        qWarning("QHeifHandler::read() failed to create context");
        return false;
    }

    int idIndex = _readState->currentIndex;
    Q_ASSERT(idIndex >= 0 && static_cast<size_t>(idIndex) < _readState->idList.size());

    auto id = _readState->idList[idIndex];

    // get image handle
    heif_image_handle* handlePtr = nullptr;
    auto error = heif_context_get_image_handle(_readState->context.get(), id, &handlePtr);

    auto handle = wrapPointer(handlePtr, heif_image_handle_release);
    if (error.code || !handle) {
        qDebug("QHeifHandler::read() failed to get image handle: %s", error.message);
        return false;
    }

    // decode image
    heif_image* srcImagePtr = nullptr;
    error = heif_decode_image(handle.get(),
                              &srcImagePtr,
                              heif_colorspace_RGB,
                              heif_chroma_interleaved_RGBA,
                              nullptr);

    heif_colorspace colorSpace = heif_image_get_colorspace(srcImagePtr);
    qDebug("QHeifHandler colorSpace: %d", colorSpace);

    int metadataCount = heif_image_handle_get_number_of_metadata_blocks(handlePtr, NULL);
    heif_item_id metadataIds[metadataCount];
    heif_image_handle_get_list_of_metadata_block_IDs(handlePtr, NULL, metadataIds, metadataCount);

    // read meta data (exif)
    for (int index = 0; index<metadataCount; index++) {
        heif_item_id id = metadataIds[index];
        const char* metadataType = heif_image_handle_get_metadata_type(handlePtr, id);
        if (strcmp(metadataType, "Exif") == 0) {
            size_t metadataSize = heif_image_handle_get_metadata_size(handlePtr, id);
            unsigned char metadata[metadataSize];
            heif_image_handle_get_metadata(handlePtr, id, metadata);
            // For Exif data, you probably have to skip the first four bytes of the data, since they
            // indicate the offset to the start of the TIFF header of the Exif data.
            unsigned char *exifSegment = metadata + 4;
            info.parseFromEXIFSegment(exifSegment, metadataSize-4);
        }
    }

    auto srcImage = wrapPointer(srcImagePtr, heif_image_release);
    if (error.code || !srcImage) {
        qDebug("QHeifHandler::read() failed to decode image: %s", error.message);
        return false;
    }

    auto channel = heif_channel_interleaved;
    QSize imgSize(heif_image_get_width(srcImage.get(), channel),
                  heif_image_get_height(srcImage.get(), channel));

    if (!imgSize.isValid()) {
        qWarning("QHeifHandler::read() invalid image size: %d x %d",
                 imgSize.width(), imgSize.height());
        return false;
    }

    int stride = 0;
    const uint8_t* data = heif_image_get_plane_readonly(srcImage.get(), channel, &stride);

    if (!data) {
        qWarning("QHeifHandler::read() pixel data not found");
        return false;
    }

    if (stride <= 0) {
        qWarning("QHeifHandler::read() invalid stride: %d", stride);
        return false;
    }

    // move data ownership to QImage
    heif_image* dataImage = srcImage.release();

    *destImage = QImage(
        data, imgSize.width(), imgSize.height(),
        stride, QImage::Format_RGBA8888,
        [](void* img) { heif_image_release(static_cast<heif_image*>(img)); },
        dataImage
    );

    return true;
}

HEIFReader::ReadState::ReadState(QByteArray&& data,
                                 std::shared_ptr<heif_context>&& ctx,
                                 std::vector<heif_item_id>&& ids,
                                 int index) :
    fileData(std::move(data)),
    context(std::move(ctx)),
    idList(std::move(ids)),
    currentIndex(index)
{
}

void HEIFReader::loadContext()
{
    QFile file = QFile(fileName);
    if (!file.open(QIODeviceBase::OpenModeFlag::ReadOnly | QIODeviceBase::OpenModeFlag::ExistingOnly)) {
        return;
    }

    if (_readState) {
        // context already loaded
        return;
    }

    // read file
    auto fileData = file.readAll();

    if (fileData.isEmpty()) {
        qDebug("QHeifHandler::loadContext() failed to read file data");
        return;
    }

    // set up new context
    std::shared_ptr<heif_context> context(heif_context_alloc(), heif_context_free);
    if (!context) {
        qDebug("QHeifHandler::loadContext() failed to alloc context");
        return;
    }

    auto error = readContext(context.get(),
                             fileData.constData(), fileData.size(), nullptr);
    if (error.code) {
        qDebug("QHeifHandler::loadContext() failed to read context: %s", error.message);
        return;
    }

    int numImages = heif_context_get_number_of_top_level_images(context.get());
    qDebug("QHeifHandler::loadContext() top level images count: %d", numImages);
    std::vector<heif_item_id> idList(numImages, 0);
    int numIdsStored = heif_context_get_list_of_top_level_image_IDs(context.get(),
                                                                    idList.data(),
                                                                    numImages);
    Q_UNUSED(numIdsStored);
    Q_ASSERT(numIdsStored == numImages);

    // find primary image in sequence; no ordering guaranteed for id values
    heif_item_id id{};
    error = heif_context_get_primary_image_ID(context.get(), &id);
    if (error.code) {
        qDebug("QHeifHandler::loadContext() failed to get primary ID: %s", error.message);
        return;
    }

    auto iter = std::find(idList.begin(), idList.end(), id);
    if (iter == idList.end()) {
        qDebug("QHeifHandler::loadContext() primary image not found in id list");
        return;
    }

    int currentIndex = static_cast<int>(iter - idList.begin());

    _readState.reset(new ReadState{std::move(fileData),
                                   std::move(context),
                                   std::move(idList),
                                   currentIndex});
}
