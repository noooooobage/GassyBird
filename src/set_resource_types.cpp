/**
 * Sets the TYPE attribute of all classes which inherit from Resource.
 */

#include "Resource.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/TextureResource.hpp"

const ResourceType TextureResource::TYPE = (ResourceType)&TextureResource::TYPE;
const ResourceType SpriteResource::TYPE = (ResourceType)&SpriteResource::TYPE;
