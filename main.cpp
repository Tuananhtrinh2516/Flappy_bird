#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PIPE_WIDTH = 50;
const int PIPE_GAP = 200;
const int PIPE_SPEED = 3;
const int GRAVITY = 1;
const int JUMP_VELOCITY = -15;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

int score = 0;

struct Bird {
    int x, y;
    int width, height;
    int velocity;
    SDL_Texture* texture; // Ảnh con chim

    Bird() : x(100), y(SCREEN_HEIGHT / 2), width(40), height(40), velocity(0), texture(nullptr) {}

    void loadTexture(const std::string& filePath) {
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
            return;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void jump() {
        velocity = JUMP_VELOCITY;
    }

    void update() {
        velocity += GRAVITY;
        y += velocity;
    }

    void render() {
        SDL_Rect rect = { x, y, width, height };
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    bool collidesWith(const SDL_Rect& rect) {
        return x < rect.x + rect.w && x + width > rect.x && y < rect.y + rect.h && y + height > rect.y;
    }

    ~Bird() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
};

struct Pipe {
    int x, y;
    int width, height;
    bool passed;

    Pipe(int x, int y, int width, int height) : x(x), y(y), width(width), height(height), passed(false) {}

    void update() {
        x -= PIPE_SPEED;
    }

    void render() {
        SDL_Rect rect = { x, y, width, height };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

std::vector<Pipe> pipes;
Bird bird;

void spawnPipe() {
    int gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP);
    pipes.push_back(Pipe(SCREEN_WIDTH, 0, PIPE_WIDTH, gapY));
    pipes.push_back(Pipe(SCREEN_WIDTH, gapY + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - gapY - PIPE_GAP));
}

void update() {
    bird.update();

    for (auto& pipe : pipes) {
        pipe.update();

        if (!pipe.passed && bird.x > pipe.x + pipe.width) {
            pipe.passed = true;
            score++;
        }
    }

    if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - 300) {
        spawnPipe();
    }

    for (auto& pipe : pipes) {
        SDL_Rect pipeRect = { pipe.x, pipe.y, pipe.width, pipe.height };
        if (bird.collidesWith(pipeRect)) {
            std::cout << "Game Over! Score: " << score << std::endl;
            exit(0);
        }
    }
    

    if (bird.y + bird.height > SCREEN_HEIGHT || bird.y < 0) {
std::cout << "Game Over! Score: " << score << std::endl;
        exit(0);
    }
}

void renderScore() {
    if (!font) {
        font = TTF_OpenFont("arial.ttf", 24);
        if (!font) {
            std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }
    }

    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect renderQuad = { 10, 10, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    bird.render();

    for (auto& pipe : pipes) {
        pipe.render();
    }

    renderScore();

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bird.loadTexture("bird.png"); // Load ảnh con chim

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                bird.jump();
            }
        }

        update();
        render();
        SDL_Delay(20);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
