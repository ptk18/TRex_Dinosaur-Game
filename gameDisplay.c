#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

extern int checkCollisionX(int r1x, int r1w, int r2x, int r2w ); // Assembly function for checking horizontal collision
extern int checkCollisionY(int r1y , int r1h, int r2y , int r2h); // Assembly function for checking vertical collision

int gameDisplay()
{
    srand(time(NULL)); // Random seed

    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* win = SDL_CreateWindow("T-Rex Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1250, 900, SDL_WINDOW_SHOWN);
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND); // Allow transparency

    int close = 0; // Game loop termination signal
    const int fps = 120; // FPS limit
    int distance = 0;

    // Background image loading
    SDL_Surface* bgSurface = IMG_Load("/home/tee/asm/trex/background.jpg");
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(rend, bgSurface);
    SDL_FreeSurface(bgSurface);

    // Calculate background dimensions
    int bgWidth, bgHeight;
    SDL_QueryTexture(bgTexture, NULL, NULL, &bgWidth, &bgHeight);

    // Initialize background positions
    SDL_Rect background1 = {0, 0, bgWidth, bgHeight};
    SDL_Rect background2 = {bgWidth, 0, bgWidth, bgHeight};

    // Dino image loading
    SDL_Surface* surface1 = IMG_Load("/home/tee/asm/trex/dino1.png");
    SDL_Surface* surface2 = IMG_Load("/home/tee/asm/trex/dino2.png");

    SDL_Texture* dino = SDL_CreateTextureFromSurface(rend, surface1); // Temp variable
    SDL_Texture* dino1 = SDL_CreateTextureFromSurface(rend, surface1); // Dino left leg sprite
    SDL_Texture* dino2 = SDL_CreateTextureFromSurface(rend, surface2); // Dino right leg sprite

    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);

    // Cactus image loading
    SDL_Surface* surface3 = IMG_Load("/home/tee/asm/trex/cactus.png");

    SDL_Texture* cactus = SDL_CreateTextureFromSurface(rend, surface3);

    SDL_FreeSurface(surface3);

    // Calculate character dimensions
    SDL_Rect character;
    SDL_QueryTexture(dino, NULL, NULL, &character.w, &character.h);

    const int yDefault = 370; // Ground level

    character.x = 100;
    character.y = yDefault;
    character.w = 100;
    character.h = 100;

    // Variables for character movement
    int jumpingStatus = 0;  // 0 for yDefault, 1 for jumping, -1 for falling
    int jumpHeight = 0; // Current jump height
    const int maxJumpHeight = 220;
    const int jumpSpeed = 5;
    int speed = 5; // Background scrolling speed

    // Variables for character sprite switching
    int currentCharacterTexture = 0; // 0 for left leg sprite, 1 for right leg sprite
    int frameCounter = 0;
    int framesPerTextureSwitch = 100 / speed; // Dino leg switching speed based on background scrolling speed

    // Create arrays to manage cactus
    const int maxCactus = 10; // Maximum number of cactus on screen for array allocation size
    int currentCactus = 0; // Current numberof cactus on screen
    int cactusExists[maxCactus]; // Store the status of cactus on screen, 1 for visible, 0 for inactive
    SDL_Rect cactusPositions[maxCactus]; // Store the positions of each cactus
    for (int i = 0; i < maxCactus; i++) {
        cactusExists[i] = 0; // Initialize all array slots to 0 because no cactus at first
    }
    int cactusSpawnTime = 200; // Frames until first cactus spawns

    // Game loop
    while (!close) {
        // Set the background color to white RGBA
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        // Scroll the background to the left
        background1.x -= speed;
        background2.x -= speed;

        // If the first background has scrolled off-screen, reset its position
        if (background1.x + bgWidth <= fps) {
            background1.x = background2.x + bgWidth;
        }

        // If the second background has scrolled off-screen, reset its position
        if (background2.x + bgWidth <= fps) {
            background2.x = background1.x + bgWidth;
        }

        // Keyboard input and mouse click events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT: // Click close button
                    close = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_SPACE:
                            if (jumpingStatus == 0) { // If not jumping and not falling
                                jumpingStatus = 1; // Start jump
                                jumpHeight = maxJumpHeight;
                            }
                            break;
                        case SDL_SCANCODE_RETURN: // Press enter to close
                            close = 1;
                            break;
                        default:
                            break;
                    }
            }
        }

        // Character texture switching
        frameCounter++;
        if (frameCounter >= framesPerTextureSwitch) {
            if (currentCharacterTexture == 0) {
                dino = dino1; // Switch to left leg sprite
                currentCharacterTexture = 1;
            } else {
                dino = dino2; // Switch to right leg sprite
                currentCharacterTexture = 0;
            }
            frameCounter = 0; // Reset the frame counter
        }
        framesPerTextureSwitch = 100 / speed; // Dino leg switching speed based on background scrolling speed

        // Jumping logic
        if (jumpingStatus == 1) { // If jumping
            character.y -= jumpSpeed;
            jumpHeight -= jumpSpeed;
            if (jumpHeight <= 0) {
                jumpingStatus = -1; // Start falling
            }
        } else if (jumpingStatus == -1) { // If falling
            character.y += jumpSpeed;
            if (character.y > yDefault) {
                character.y = yDefault;
                jumpingStatus = 0; // Returned to normal position
            }
        }

        // Render new frame
        SDL_RenderClear(rend); // Clear previous frame
        SDL_RenderCopy(rend, bgTexture, NULL, &background1); // Render background1
        SDL_RenderCopy(rend, bgTexture, NULL, &background2); // Render background2
        SDL_RenderCopy(rend, dino, NULL, &character); // Render dino

        // Create a new cactus object
        cactusSpawnTime--;
        if (cactusSpawnTime <= 0 && currentCactus < maxCactus) {
            // Find an empty slot in the cactus array
            for (int i = 0; i < maxCactus; i++) {
                if (!cactusExists[i]) { // If array slot i is available
                    // Set the cactus initial position
                    cactusPositions[i].x = bgWidth + 150; // Start from the right edge
                    cactusPositions[i].y = yDefault; // Ground level
                    cactusPositions[i].w = 70; // Set the cactus width
                    cactusPositions[i].h = 100; // Set the cactus height

                    // Sets the wait time before next cactus object is created
                    cactusSpawnTime = 100 + rand() % 200; // Random number between 100 to 300 frames

                    cactusExists[i] = 1; // Mark array slot as unavailable
                    currentCactus++; // Increment the count of cactus on the screen
                    break;
                }
            }
        }

        // Update position and draw cactus
        for (int i = 0; i < maxCactus; i++) {
            if (cactusExists[i]) {
                // Move the cactus to the left
                cactusPositions[i].x -= speed;

                // Check if the cactus is off the screen
                if (cactusPositions[i].x + cactusPositions[i].w < 0) {
                    // Remove the cactus
                    cactusExists[i] = 0; // Mark slot as available again
                    currentCactus--; // Decrement the count of cactus on screen
                }

                // Draw the cactus at new positions
                SDL_RenderCopy(rend, cactus, NULL, &cactusPositions[i]);
            }
        }

        SDL_RenderPresent(rend); // Render objects to the screen
        SDL_Delay(1000 / fps); // Limit FPS
        distance++;

        // Slowly increase speed
        if (speed < 15 && distance > speed * 600)
            speed++;

        // Check collision
        for (int i = 0; i < maxCactus; i++) {
            if (cactusExists[i]) {
                SDL_Rect rect; // New rect to offset transparent border of cactus for a closer collision
                rect.x = cactusPositions[i].x;
                rect.y = cactusPositions[i].y + 70;
                rect.h = cactusPositions[i].h;
                rect.w = cactusPositions[i].w / 2;
                if (checkCollisionX(character.x, character.w, rect.x, rect.w ) && checkCollisionY(character.y, character.h, rect.y, rect.h )) { // Check vertical and horizontal collision
                    close = 1; // Game over, stop game loop
                    break;
                }
            }
        }
    }

    // Free memory and clean up
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(dino);
    SDL_DestroyTexture(dino1);
    SDL_DestroyTexture(dino2);
    SDL_DestroyTexture(cactus);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
