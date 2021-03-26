#ifndef _RESOURCE_CACHE_HPP_
#define _RESOURCE_CACHE_HPP_

#include <unordered_map>
#include <memory>
#include <cassert>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Resource.hpp"
#include "Resources/TextureResource.hpp"

/**
 * Stores game resources, e.g. textures, fonts, etc. These resources can be accessed via a unique
 * string ID. As of now, all game resources are loaded into memory; we can change this later if we
 * end up having too many resources.
 */
class ResourceCache {

public:

    /**
     * Default constructor
     */
    ResourceCache();

    /**
     * Destructor; frees memory owned by resources.
     */
    ~ResourceCache();

    /**
     * Stores all resources.
     */
    // TODO: Store resources in a cleaner way; don't just cram them all in init().
    void init();

    /**
     * Returns a pointer to the resource which has the specified id. The given id must be a resource
     * that has already been stored. Template parameter T describes the type to which the resource
     * is cast before being returned.
     * 
     * E.g. to get a SpriteResource with id "BIRD_SPRITE", you would use:
     * 
     * getResource<SpriteResource>("BIRD_SPRITE");
     * 
     * IMPORTANT: The type specified by T must match the resource's actual type. This is checked
     * when compiling in DEBUG mode, but it is not checked in RELEASE mode.
     */
    template <typename T>
    T* const getResource(const std::string& id) const {

        assert(_initialized);

        // make sure that the resource exists
        assert(_resources.find(id) != _resources.end());

        // get resource
        Resource* res = _resources.at(id).get();

        // make sure that the template type matches the actual type
        assert(res->getType() == T::TYPE);

        return dynamic_cast<T*>(res);
    }

private:

    /**
     * Loads and stores a TextureResource with the given id from the given filename, which should be
     * an image.
     */
    void loadTextureResource(const std::string& id, const std::string& filename);

    /**
     * Loads and stores a SpriteResource with the given id. The underlying sprite is created from
     * the given textureResource. "Frames" of the sprite's animation are given by textureRects,
     * which must have at least one entry.
     */
    void loadSpriteResource(const std::string& id, const TextureResource& textureResource,
            const std::vector<sf::IntRect>& textureRects);

    void loadFontResource(const std::string& id, const std::string& filename);
    
    bool _initialized;

    // resources are stored in this maps
    std::unordered_map<std::string, std::shared_ptr<Resource>> _resources;
};

#endif // _RESOURCE_CACHE_HPP_