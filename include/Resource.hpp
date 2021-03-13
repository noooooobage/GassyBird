#ifndef _RESOURCE_HPP_
#define _RESOURCE_HPP_

typedef void* ResourceType;

/**
 * Superclass for all resources (SpriteResource, TextureResource, etc.). This exists just so that
 * type checking can be done when accessing a resource from the resource cache.
 */
class Resource {

public:

    virtual ~Resource() {}

    virtual const ResourceType& getType() const = 0;
};

#endif // _RESOURCE_HPP_
