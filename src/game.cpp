#include "game.h"

#include <iostream>
#include <limits>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "srutils.h"

#include "tinyxml2.h"

enum GameStatus { RUNNING, WON, OVER };

class Game::Impl {
  public:
    const char *path_;
    size_t windowSideLength_;
    bool keyboardKeyPressed_[256] = {0};
    bool mouseButtonPressed_[3] = {0};

    const GLfloat gravity_px_s_ = 42.f + 28.f; // 294.f;
    const GLfloat characters_speed_px_s_ = srutils::CHARACTER_SPEED_PX_S;
    const GLfloat characters_jump_px_s_ = 126.f + 28.f; // 630.f;

    GameStatus status_ = GameStatus::RUNNING;

    Arena *arena_;

    Impl(size_t windowSideLength)
        : path_(nullptr), windowSideLength_(windowSideLength),
            arena_(new Arena(windowSideLength)) { }

    ~Impl() { delete arena_; }

    void reset() {
        for (auto &key : keyboardKeyPressed_)
            key = false;
        
        for (auto &button : mouseButtonPressed_)
            button = false;

        delete arena_;

        auto *narena = new Arena(windowSideLength_);
        if (!narena->loadFrom(path_))
            throw std::runtime_error("Failed to load arena");

        arena_ = narena;

        status_ = GameStatus::RUNNING;        
    }

    void init(void) {
        /* selecionar cor de fundo (azul rgb(158 158 255)) */
        glClearColor(0.620f, 0.620f, 1.0f, 1.0f);

        //glShadeModel (GL_FLAT);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, (GLsizei)windowSideLength_, (GLsizei)windowSideLength_);
        
        GLfloat light_ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        GLfloat light_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };  
        GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
    
        for (int i = 0; i < 3; i++) {
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
        }

        glEnable(GL_NORMALIZE);

        /* inicializar sistema de visualizacao */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glOrtho(0.0, windowSideLength_, 0.0, windowSideLength_, -100.0, 100.0);
        gluPerspective(60, (GLfloat)windowSideLength_ / (GLfloat)windowSideLength_, .1, 1000000);
    }

    bool checkEndGame() {
        if (keyboardKeyPressed_['r'] || keyboardKeyPressed_['R']) {
            reset();

            return true;
        }

        if (arena_->player().ttl() < 0.0) // a.k.a. dead
            status_ = GameStatus::OVER;

        return status_ != GameStatus::RUNNING;
    }

    bool moveEntityNCheckCollisions(const Entity &entity, GLdouble dt) {
        entity.colisions_reset();

        auto min_deltas_check_all_zero = [&entity](const auto &entities, GLfloat &dx, GLfloat &dy) {
            for (auto &obstacle : entities) {                
                if (dynamic_cast<const Entity*>(&entity) == dynamic_cast<const Entity*>(&obstacle)) continue;

                if (!entity.aabb_isoverlapping_delta(obstacle, dx, dy))
                    continue;

                //
                // BUG:
                //   Se um character bater na quina de uma platform (leia-se, se
                //   uma entity qualquer se deslocar diagonalmente em direção a
                //   um eixo de outra entidade qualquer), por algum motivo ela
                //   fica garrada lá até haver um vetor de força horizontalmente
                //   contrario a entidade.
                //
                //   att: 2025-01-26
                //     Antes, as funcoes aabb_distanceof_x e aabb_distanceof_y retornavam 0
                //     quando havia overlapping (justamente o caso da quina), quanto
                //     removida essa condicao, o personagem nao fica mais garrado
                //     mas sim atravessa a plataforma.
                //

                if (entity.aabb_isoverlapping_dx(obstacle, dx)) {
                    entity.colisions_set_last_x(obstacle);
                    //character.jump_end();
                    dx = srutils::minabsf(dx, entity.aabb_distanceof_x(obstacle));
                    //dx = .0f;
                }
                if (entity.aabb_isoverlapping_dy(obstacle, dy)) {
                    entity.colisions_set_last_y(obstacle);
                    //entity.jump_end();
                    dy = srutils::minabsf(dy, entity.aabb_distanceof_y(obstacle));
                    //dy = .0f;
                }
            }

            if (dx == .0f && dy == .0f)
                return true;

            return false;
        };

        GLfloat seconds = dt / 1000.0f;
        
        GLfloat dx = entity.vector_current().calc_dx_dt(seconds);
        GLfloat dy = entity.vector_current().calc_dy_dt(seconds);

        // colision entity
        //min_deltas_check_all_zero(arena_->platforms(), dx, dy);
        min_deltas_check_all_zero(arena_->platforms(), dx, dy);
        min_deltas_check_all_zero(arena_->foes(), dx, dy);
        min_deltas_check_all_zero(arena_->players(), dx, dy);

        // colision walls
        auto &wall = arena_->boundaries();
        if (!entity.aabb_isinsideof_dx(wall, dx)) {
            entity.colisions_set_last_x(wall);
            //character.jump_end();
            dx = srutils::minabsf(dx, entity.aabb_insideof_x(wall, dx));
            //dx = .0f;
        }
        if (!entity.aabb_isinsideof_dy(wall, dy)) {
            entity.colisions_set_last_y(wall);
            //entity.jump_end();
            dy = srutils::minabsf(dy, entity.aabb_insideof_y(wall, dy));
            //dy = .0f;
        }

        entity.movement_translate(dx, dy);

        return dx != 0.0f || dy != 0.0f;
    }

    void controlBullets(GLdouble dt, GLdouble ct) {
        auto &bullets = arena_->bullets();

        const auto kill = [&bullets](const Entity *colision) {
            if (colision != nullptr) {
                if (dynamic_cast<const Character*>(colision) != nullptr
                || dynamic_cast<const Bullet*>(colision) != nullptr)
                    colision->die();

                return true;
            }
            return false;
        };

        for (auto it = bullets.begin(); it != bullets.end();)
        {
            auto &bullet = *it;

            // remove dead bullets
            if (bullet.ttl() < ct) {
                it = bullets.erase(it);
                continue;
            }

            const auto &bullet_v = bullet.direction();
            bullet.vector_sum(bullet_v.direction_x(), bullet_v.direction_y(), characters_speed_px_s_ * 2);

            // move bullets
            moveEntityNCheckCollisions(bullet, dt);
            bullet.vector_save_current_set_zero();

            const auto &colisions = bullet.colisions_tuple();
            const auto &colisions_x = std::get<0>(colisions);
            const auto &colisions_y = std::get<1>(colisions);
            if (kill(colisions_x) || kill(colisions_y)) {
                it = bullets.erase(it);
                continue;
            }            

            ++it;
        }
    }

    void controlPlayer(GLdouble dt, GLdouble ct) {
        // =============== movement
        // set up player movement vector
        const auto &player = arena_->player();
        if (keyboardKeyPressed_['s'] || keyboardKeyPressed_['s'])
            player.vector_sum(-1.0f, 0.0f, characters_speed_px_s_);
        else if (keyboardKeyPressed_['w'] || keyboardKeyPressed_['W'])
            player.vector_sum(1.0f, 0.0f, characters_speed_px_s_);
        
        if (mouseButtonPressed_[GLUT_RIGHT_BUTTON] || keyboardKeyPressed_[' ']) {
            if (player.jump_can_they()) {
                if (!player.jumping())
                    player.jump_start();

                player.vector_sum(0.0f, 1.0f, characters_jump_px_s_);
            }
        } else {
            player.jump_end();
        }

        // set up player gravity
        player.vector_sum(0.0f, -1.0f, gravity_px_s_);

        // god move
        if (keyboardKeyPressed_['i'] || keyboardKeyPressed_['I']) {
            player.vector_current().set_zero();
            player.vector_sum(0.0f, 1.0f, characters_speed_px_s_ * 5);
        }
        else if (keyboardKeyPressed_['k'] || keyboardKeyPressed_['K']) {
            player.vector_current().set_zero();
            player.vector_sum(0.0f, -1.0f, characters_speed_px_s_ * 5);
        }
        else if (keyboardKeyPressed_['j'] || keyboardKeyPressed_['J']) {
            player.vector_current().set_zero();
            player.vector_sum(-1.0f, 0.0f, characters_speed_px_s_ * 5);
        }
        else if (keyboardKeyPressed_['l'] || keyboardKeyPressed_['L']) {
            player.vector_current().set_zero();
            player.vector_sum(1.0f, 0.0f, characters_speed_px_s_ * 5);
        }

        moveEntityNCheckCollisions(player, dt);
        if (player.colisions_last_top() != nullptr || player.colisions_last_bottom() != nullptr)
            player.jump_end();

        player.vector_save_current_set_zero();

        if (player.colisions_last_right() == &arena_->boundaries())
            status_ = GameStatus::WON;

        // ====================== gun
        if (player.shot_add_dt(dt) && mouseButtonPressed_[GLUT_LEFT_BUTTON]) {
            auto bullet = player.shoot();
            bullet.ttl(ct + 60 * 1000);
            arena_->addBullet(std::move(bullet));
        }
    }

    void controlFoes(GLdouble dt, GLdouble ct) {
        auto &foes = arena_->foes();

        for (auto it = foes.begin(); it != foes.end();)
        {
            auto &foe = *it;

            // remove dead foes
            if (foe.ttl() < 0.0) {
                it = foes.erase(it);
                continue;
            }

            if (foe.direction() == CharacterDirection::LEFT) {
                foe.vector_sum(-1.0f, 0.0f, characters_speed_px_s_);
            } else {
                foe.vector_sum(1.0f, 0.0f, characters_speed_px_s_);
            }

            // set up player gravity
            foe.vector_sum(0.0f, -1.0f, gravity_px_s_);

            if (foe.colisions_last_bottom() != nullptr && foe.colisions_last_bottom() != &arena_->boundaries()) {
                auto dx = foe.vector_current().calc_dx_dt(dt / 1000.0f);
                //auto dy = foe.vector_current().calc_dy_dt(dt / 1000.0f);

                auto platform = foe.colisions_last_bottom();

                auto max_mov = foe.aabb_insideof_x(*platform, dx);
                auto ndx = srutils::minabsf(dx, max_mov);
                auto scale = ndx / dx;

                auto x = foe.vector_current().direction_x() * scale * foe.vector_current().velocity();
                auto y = foe.vector_current().direction_y() * scale * foe.vector_current().velocity();

                //std::cout << "dx: " << dx << " dy: " << dy << " max_mov: " << max_mov << " ndx: " << ndx << " scale: " << scale << std::endl << std::endl;

                foe.vector_current().set_vector(x, y);

                if (fabsf(max_mov) <= fabs(dx)) {
                    if (foe.direction() == CharacterDirection::LEFT)
                        foe.direction(CharacterDirection::RIGHT);
                    else
                        foe.direction(CharacterDirection::LEFT);
                }
            }

            moveEntityNCheckCollisions(foe, dt);
            foe.vector_save_current_set_zero();

            foe.aim(arena_->player().o_x() - foe.o_x(), arena_->player().o_y() - foe.o_y());
            if (foe.shot_add_dt(dt)) {
                auto bullet = foe.shoot();
                bullet.ttl(ct + 60 * 1000);
                arena_->addBullet(std::move(bullet));
            }

            if (foe.direction() == CharacterDirection::LEFT && foe.colisions_last_left() != nullptr)
                foe.direction(CharacterDirection::RIGHT);
            else if (foe.direction() == CharacterDirection::RIGHT && foe.colisions_last_right() != nullptr)
                foe.direction(CharacterDirection::LEFT);

            ++it;
        }
    }

    void lookAtPlayer() {
        const auto &player = arena_->player();

        gluLookAt(
            player.o_x() - 3 * player.height(), player.o_y() + player.width(), player.o_z(),
            player.o_x() + player.depth(), player.o_y() + player.height() / 2, player.o_z(),
            0, 1, 0
        );
    }

    void lightUp() {
        const int light_count = 3;

        const auto gap = this->arena_->width() / (light_count + 1);
        const GLfloat light_position[][4] = {
            { gap, this->arena_->height() - 10, 0, 1.0 },
            { gap * 2, this->arena_->height() - 10, 0, 1.0 },
            { gap * 3, this->arena_->height() - 10, 0, 1.0 }
        };
        
        for (int i = 0; i < light_count; i++) {
            glPushMatrix();
                // Position light before any other transformations
                glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position[i]);
                
                glDisable(GL_LIGHTING);
                    glPushMatrix();
                        glTranslatef(light_position[i][0], light_position[i][1], light_position[i][2]);
                        glColor3f(1.0, 1 * 0.8, 1 * 0.4); // Red-orange flash
                        glutSolidCube(.2);
                    glPopMatrix();
                glEnable(GL_LIGHTING);
            glPopMatrix();
        }
    }
};

Game::Game(size_t windowSideLength)
    : pimpl(new Impl(windowSideLength)) { }

Game::~Game() = default;

bool Game::loadArena(const char *path) {
    pimpl->path_ = path;

    float factor = pimpl->arena_->loadFrom(path);
    if (factor == .0f)
        return false;

    return true;
}

bool Game::start() {
    pimpl->init();

    return true;
}

void Game::display(void) {
    /* selecionar cor de fundo (azul rgb(158,158,255)) */
    glClearColor(0.620f, 0.620f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (pimpl->status_ != GameStatus::RUNNING) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(0, pimpl->windowSideLength_, 0, pimpl->windowSideLength_, -1, 1);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            // Disable lighting for text drawing
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

                if (pimpl->status_ == GameStatus::WON) {
                    glColor3f(.059f, .741f, .059f); // rgb(15, 189, 15)
                    glRasterPos2f(pimpl->windowSideLength_/2.0f - 40, pimpl->windowSideLength_/2.0f);
                    const char* twink = "You win!";
                    for (const char* c = twink; *c != '\0'; c++)
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

                    glColor3f(1.f, 1.f, 1.f); // rgb(255, 255, 255)
                    glRasterPos2f(pimpl->windowSideLength_/2.0f - 60, 10);
                    const char* tr = "Press R to restart";
                    for (const char* c = tr; *c != '\0'; c++)
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
                }
                else if (pimpl->status_ == GameStatus::OVER) {
                    /* selecionar cor de fundo (azul rgb(46, 42, 67)) */
                    glClearColor(0.180f, 0.165f, .263f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);

                    glColor3f(.741f, .059f, .059f); // rgb(189, 15, 15)
                    glRasterPos2f(pimpl->windowSideLength_/2.0f - 40, pimpl->windowSideLength_/2.0f);
                    const char* tgameover = "SE FODEU";
                    for (const char* c = tgameover; *c != '\0'; c++)
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

                    glColor3f(1.f, 1.f, 1.f); // rgb(255, 255, 255)
                    glRasterPos2f(pimpl->windowSideLength_/2.0f - 60, 10);
                    const char* tr = "Press R to restart";
                    for (const char* c = tr; *c != '\0'; c++)
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
                }

            glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
            
            glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glutSwapBuffers();

        return;
    }

    pimpl->lookAtPlayer();

    pimpl->lightUp();

    pimpl->arena_->draw();

    glutSwapBuffers();
}

void Game::keyboard(unsigned char key, int x, int y) {
    if (x) { }
    if (y) { }

    pimpl->keyboardKeyPressed_[key] = true;

    glutPostRedisplay();
}

void Game::keyboardUp(unsigned char key, int x, int y) {
    if (x) { }
    if (y) { }

    pimpl->keyboardKeyPressed_[key] = false;

    glutPostRedisplay();
}

void Game::idle() {
    static GLdouble previousTime = glutGet(GLUT_ELAPSED_TIME);

    GLdouble currentTime = glutGet(GLUT_ELAPSED_TIME);

    // interval in milliseconds since last idle callback
    GLdouble dt = currentTime - previousTime;
    previousTime = currentTime;

    if (pimpl->checkEndGame()) {
        glutPostRedisplay();

        return;
    }

    pimpl->controlBullets(dt, currentTime);

    pimpl->controlPlayer(dt, currentTime);

    pimpl->controlFoes(dt, currentTime);

    glutPostRedisplay();
}

void Game::mouse(int button, int state, int x, int y) {
    if (x) { }
    if (y) { }

    pimpl->mouseButtonPressed_[button] = state == GLUT_DOWN;

    glutPostRedisplay();
}

void Game::motion(int x, int y) {
    auto gl_x = x;
    auto gl_y = pimpl->windowSideLength_ - y;

    auto player_x = pimpl->windowSideLength_ / 2.0f;
    auto player_y = pimpl->arena_->player().height() / 2.0f + pimpl->arena_->player().o_y();

    pimpl->arena_->player().aim(gl_x - player_x, gl_y - player_y);

    glutPostRedisplay();
}
