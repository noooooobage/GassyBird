#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <SFML/Graphics.hpp>

#include "Actor.hpp"
#include "Resources/FontResource.hpp"

/**
 * A clickable button. Has a rectungular shape and contains text.
 */
class Button : public Actor {

public:

    /**
     * Sets values to sensible defaults.
     */
    Button();

    /**
     * Sets the font of the button's text.
     */
    void setFont(const FontResource& fontResource);

    /**
     * Sets the string content of the button and centers it.
     */
    void setString(const std::string& string);

    /**
     * Sets the character height of the inner text. Note that the actual height of the text may not
     * match the given height after setting it; the given height should be a height supported by the
     * font.
     */
    void setCharacterHeight(const int& height);

    /**
     * Sets the size of the base rectangle.
     */
    void setSize(const sf::Vector2f& size);

    /**
     * Sets the position of the top-left of the base rectangle.
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * Sets the opacity of the button to the given opacity value, which is in the range [0, 1]. 
     * Values outside of this range are clamped to the range. 0 means fully transparent, and 1 means
     * fully opaque.
     */
    void setOpacity(const float& opacity);

    /**
     * Returns true if the given point (graphical world coordinates, not pixel coordinates) is
     * inside the bounds of the button. Returns false otherwise.
     */
    bool contains(const sf::Vector2f& point);

    /**
     * Changes the color of the shape to its base color and disappears the outline.
     */
    void toBase();

    /**
     * Sets the color of the shape to its base color and makes the outline of the shape visible.
     */
    void toHovered();

    /**
     * Sets the color of the shape to its clicked color and makes the outline of the shape visible.
     */
    void toClicked();

    /**
     * Draws the shape and text.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    sf::Text _text;
    sf::RectangleShape _shape;

    float _opacity;
    sf::Color _opaqueTextColor;
    sf::Color _opaqueShapeColor;

    const sf::Color _BASE_TEXT_COLOR; // normal fill color of text
    const sf::Color _HOVERED_TEXT_COLOR; // fill color of text when botton is hovered
    const sf::Color _CLICKED_TEXT_COLOR; // fill color of text when button is clicked
    const sf::Color _BASE_FILL_COLOR; // normal shape fill color when not hovered or clicked
    const sf::Color _HOVERED_FILL_COLOR; // fill color when the button is being hovered over
    const sf::Color _CLICKED_FILL_COLOR; // fill color when the button is being clicked};
    const float _BASE_OUTLINE_THICKNESS; // normal outline thickness
    const float _HOVERED_OUTLINE_THICKNESS; // outline thickness when hovered over
};

#endif
