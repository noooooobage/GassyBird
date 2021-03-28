/**
 * Sets the TYPE attribute of all classes which inherit from Resource.
 */

#include "Resource.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/TextureResource.hpp"
#include "Resources/FontResource.hpp"
#include "Resources/PolygonResource.hpp"

const ResourceType TextureResource::TYPE = (ResourceType)&TextureResource::TYPE;
const ResourceType SpriteResource::TYPE =  (ResourceType)&SpriteResource::TYPE;
const ResourceType FontResource::TYPE =    (ResourceType)&FontResource::TYPE;
const ResourceType PolygonResource::TYPE = (ResourceType)&PolygonResource::TYPE;
