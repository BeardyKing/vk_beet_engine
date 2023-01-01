#pragma once

#include <beet/gfx/types.h>

namespace beet::gfx {

struct RawImage {
    void* data;  // MUST CHECK IF THIS IS NULLPTR / CALL FREE WHEN FINISHED WITH THIS DATA
    VkDeviceSize size;
    VkFormat format;
    VkExtent3D extent;

    void free_image_data() {
        free(data);
        data = nullptr;
    }
};

struct Texture {
    RawImage rawImage;  // INFO:    Consider taking this out of the texture struct and only using it
    //                              when the texture is being created as it makes any CPU copy more expensive
    //                              but having this info here will make any editor code simpler to write
    VkImageView imageView;
    AllocatedImage image;
};

}  // namespace beet::gfx