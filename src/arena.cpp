#include "arena.h"

#include <iostream>

#include "character.h"
#include "color_rgb.h"

#include "tinyxml2.h"

class Arena::Impl {
    public:
        float height_;
        float width_;
        float depth_;

        Boundary *boundaries_;
        std::vector<Platform> platforms_;
        std::list<Character> foes_;
        std::vector<Character> players_; // only one player
        std::list<Bullet> bullets_;

    Impl(float height) : height_(height) {}
    ~Impl() {
        delete boundaries_;
    }
};

Arena::Arena(float height) : pimpl(std::make_unique<Impl>(height)) {}
Arena::~Arena() = default;

const Boundary &Arena::boundaries() const { return *pimpl->boundaries_; }
const float &Arena::height() const { return pimpl->height_; }
const float &Arena::width() const { return pimpl->width_; }
const float &Arena::depth() const { return pimpl->depth_; }

const std::vector<Platform> &Arena::platforms() const { return pimpl->platforms_; }
std::list<Character> &Arena::foes() { return pimpl->foes_; }
const Character &Arena::player() const { return pimpl->players_.front(); }
const std::vector<Character> &Arena::players() const { return pimpl->players_; }
std::list<Bullet> &Arena::bullets() { return pimpl->bullets_; }

void Arena::addBullet(Bullet &&bullet) { pimpl->bullets_.push_back(std::move(bullet)); }

void Arena::draw(bool draw_axes) const {
    //pimpl->boundaries_->draw(draw_axes);

    const static Box plane(
        {
            { 0.2f, 0.2f, 0.2f, 1.0 },   
            { 0.1f, 0.1f, 0.4f, 1.0 }, // rgb(20, 20, 143)
            { 0.1f, 0.1f, 0.4f, 1.0 },
            { 0.1f, 0.1f, 0.1f, 1.0 },   
            { 32 },       
        },
        pimpl->width_,
        1,
        pimpl->depth_
    );

    // floor
    glPushMatrix();
        glTranslatef(width() / 2, -.5, 0);
        plane.draw(draw_axes);
    glPopMatrix();

    // ceiling
    glPushMatrix();
        glTranslatef(width() / 2, pimpl->height_ + .5f, 0);
        plane.draw(draw_axes);
    glPopMatrix();

    // win wall
    glPushMatrix();
        glTranslatef(pimpl->width_ + .5f, pimpl->height_ / 2, 0);
        glRotatef(90, 0, 0, 1);
        //glRotatef(90, 1, 0, 0);
        plane.draw(draw_axes);
    glPopMatrix();

    // right wall
    glPushMatrix();
        glTranslatef(width() / 2, pimpl->depth_, pimpl->depth_ / 2 + .5f);
        glRotatef(90, 1, 0, 0);
        glScalef(2, 2, 2);
        plane.draw(draw_axes);
    glPopMatrix();

    // left wall
    glPushMatrix();
        glTranslatef(width() / 2, pimpl->depth_, -pimpl->depth_ / 2 - .5f);
        glRotatef(90, 1, 0, 0);
        glScalef(2, 2, 2);
        plane.draw(draw_axes);
    glPopMatrix();
    

    for (const Platform &platform : pimpl->platforms_)
        platform.draw(draw_axes);
    
    for (const Character &foe : pimpl->foes_)
        foe.draw(draw_axes);
    
    for (const auto &bullet : pimpl->bullets_)
        bullet.draw(draw_axes);

    player().draw(draw_axes);
}

float Arena::loadFrom(const char *path) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError fileLoaded = doc.LoadFile(path);

    if (fileLoaded != tinyxml2::XML_SUCCESS)
        return .0f;

    const tinyxml2::XMLElement *svg = doc.FirstChildElement("svg");
    if (svg == nullptr)
        return 0;

    float backgroundSvgX = .0;
    float backgroundSvgY = .0;
    float backgroundSvgHeight = .0;

    float backgroundSvgToWindowHeightFactor = .0;

    // find background
    for (const tinyxml2::XMLElement *element = svg->FirstChildElement();
         element != nullptr; element = element->NextSiblingElement()) {

        const char *elementName = element->Name();
        
        if (strcmp(elementName, "rect") == 0 && element->Attribute("fill", "blue")) {
            float backgroundSvgWidth = element->FloatAttribute("width");
            backgroundSvgHeight = element->FloatAttribute("height");
            backgroundSvgX = element->FloatAttribute("x");
            backgroundSvgY = element->FloatAttribute("y");

            backgroundSvgToWindowHeightFactor = (float)pimpl->height_ / backgroundSvgHeight;

            pimpl->width_ = backgroundSvgWidth * backgroundSvgToWindowHeightFactor;
            pimpl->depth_ = pimpl->height_ / 2.0f;

            pimpl->boundaries_ = new Boundary(
                0, 0, 0,
                pimpl->width_,
                pimpl->height_,
                pimpl->depth_
            );

            std::cout
                << "Found BACKGROUND"
                << " at svg ("
                << backgroundSvgX
                << ", "
                << backgroundSvgY 
                << "), with dimensions ("
                << backgroundSvgWidth
                << ", "
                << backgroundSvgHeight
                << ")"
                << " Transformed to (0, 0, 0)bl with dimensions ("
                << pimpl->width_
                << ", "
                << pimpl->height_
                << ", "
                << pimpl->depth_
                << ")"
            << std::endl << std::endl;

            break;
        }
    }

    for (const tinyxml2::XMLElement *element = svg->FirstChildElement();
         element != nullptr; element = element->NextSiblingElement()) {

        const char *elementName = element->Name();
        
        if (strcmp(elementName, "rect") == 0 && !element->Attribute("fill", "blue")) {
            float x = element->FloatAttribute("x");
            float y = element->FloatAttribute("y");
            float w = element->FloatAttribute("width");
            float h = element->FloatAttribute("height");

            // because arena's origin is (0, 0) as the instance of Background Platform
            float svgToArenaX = (x - backgroundSvgX) * backgroundSvgToWindowHeightFactor;
            float svgToArenaY = (backgroundSvgHeight - (y - backgroundSvgY + h)) * backgroundSvgToWindowHeightFactor;
            float svgToArenaZ = .0;
            
            pimpl->platforms_.push_back(Platform(
                svgToArenaX, svgToArenaY, svgToArenaZ,
                w * backgroundSvgToWindowHeightFactor,
                h * backgroundSvgToWindowHeightFactor,
                pimpl->depth_
            ));

            std::cout
                << "Found PLATFORM"
                << " at svg ("
                << x
                << ", "
                << y
                << "), with dimensions ("
                << w
                << ", "
                << h
                << "), Transformed to ("
                << svgToArenaX
                << ", "
                << svgToArenaY
                << ")bl with dimensions ("
                << w * backgroundSvgToWindowHeightFactor
                << ", "
                << h * backgroundSvgToWindowHeightFactor
                << ")"
            << std::endl << std::endl;

        } else if (strcmp(elementName, "circle") == 0) {
            float cx = element->FloatAttribute("cx");
            float cy = element->FloatAttribute("cy");
            float r = element->FloatAttribute("r");

            float svgToArenaX = (cx - backgroundSvgX - r) * backgroundSvgToWindowHeightFactor;
            float svgToArenaY = (backgroundSvgHeight - (cy - backgroundSvgY + r)) * backgroundSvgToWindowHeightFactor;

            float characterHeight = 2 * r * backgroundSvgToWindowHeightFactor;
            float characterWidth = characterHeight * .88f;

            if (element->Attribute("fill", "green"))
                pimpl->players_.push_back(Character(
                    svgToArenaX, svgToArenaY, 0,
                    characterWidth, characterHeight, 3 * characterWidth / 4,
                    ColorRgb((uint8_t)15, 189, 15)
                ));
            else
                pimpl->foes_.push_back(Character(
                    svgToArenaX, svgToArenaY, 0,
                    characterWidth, characterHeight, 3 * characterWidth / 4,
                    ColorRgb((uint8_t)189, 15, 15)
                ));

            std::cout
                << "Found CHARACTER ["
                << element->Attribute("fill")
                << "] at ("
                << cx
                << ", "
                << cy
                << ") with radius "
                << r
                << ", Transformed to ("
                << svgToArenaX
                << ", "
                << svgToArenaY
                << ")bl with dimensions ("
                << characterWidth
                << ", "
                << characterHeight
                << ")"
            << std::endl << std::endl;
        }
    }

    return backgroundSvgToWindowHeightFactor;
}
