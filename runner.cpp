#include "raylib.h"
// Struct containing info to help with animating, positioning and hitboxes
struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};
// Checks to see if struct instance is on the ground
bool isOnGround(AnimData data, int windowHeight) {
    return data.pos.y >= windowHeight - data.rec.height;
}
// Animates the struct instance(framerate independent)
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame) {
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime) {
        data.runningTime = 0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame) data.frame = 0;
    }
    return data;
}

int main() {
    // Window width and height
    const int width = 512;
    const int height = 384;
    // Pulling force of gravity
    const int gravity = 1000;
    // Initialized velocity and the velocity player reaches on jumping
    int velocity = 0;
    const int jumpVelocity = 600;
    bool isInAir = false; // Checks if player is in the air
    // Making the window and setting the FPS
    InitWindow(width, height, "Runner");
    SetTargetFPS(60);
    // Loading the player character sprite sheet
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    // Making an instance of the struct that will help with animating, positioning and adding hitboxes to the player
    AnimData scarfyData {
        {0, 0, float(scarfy.width/6), float(scarfy.height)},
        {width/2 - scarfyData.rec.width/2, height - scarfyData.rec.height},
        0,
        1.0 / 12.0,
        0
    };
    // Loading the obstacle sprite sheet and setting its velocity, the higher the velocity, the faster the obstacles
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    int nebVel = 250;
    // Making an array of obstacle instances
    const int sizeOfNebulae = 8;
    AnimData nebulae[sizeOfNebulae] {};
    // Setting the distance between every obstacle that will be spawned in the level
    int distanceBetweenObstacles = 400;
    // Setting the position, size and animation information of every obstacle in array
    for(int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].rec.x = 0;
        nebulae[i].rec.y = 0;
        nebulae[i].rec.width = float(nebula.width/8);
        nebulae[i].rec.height = float(nebula.height/8);
        nebulae[i].pos.y = height - nebulae[i].rec.height;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 1.0 / 12.0;
        nebulae[i].runningTime = 0;
        nebulae[i].pos.x = width + (i * distanceBetweenObstacles);
    }
    // Last obstacle in array will be turned into a finish line
    float finishLine = nebulae[sizeOfNebulae-1].pos.x;
    // Loading textures for the background, midground and foreground
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX = 0;
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX = 0;
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX = 0;
    // Checks if player collided with obstacle, by default it is false
    bool collision = false;
    // Starts the game loop that can be broken out of only by pressing escape or closing the window
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        // Delta time, to make the game framerate independent
        const float dT = GetFrameTime();
        // Makes the background, midground and foreground scroll at different speeds
        bgX -= 20 * dT;
        mgX -= 40 * dT;
        fgX -= 80 * dT;
        // When they reach double their width(because we will render 2), reset their positions to 0
        if(bgX <= -background.width*2) bgX = 0;
        if(mgX <= -midground.width*2) mgX = 0;
        if(fgX <= -foreground.width*2) fgX = 0;
        // Positioning 2 of each
        Vector2 bg1Pos {bgX, 0};
        Vector2 bg2Pos {bgX + background.width * 2, 0};
        Vector2 mg1Pos {mgX, 0};
        Vector2 mg2Pos {mgX + midground.width * 2, 0};
        Vector2 fg1Pos {fgX, 0};
        Vector2 fg2Pos {fgX + foreground.width * 2, 0};
        // Rendering 2 of each
        DrawTextureEx(background, bg1Pos, 0, 2, WHITE);
        DrawTextureEx(background, bg2Pos, 0, 2, WHITE);
        DrawTextureEx(midground, mg1Pos, 0, 2, WHITE);
        DrawTextureEx(midground, mg2Pos, 0, 2, WHITE);
        DrawTextureEx(foreground, fg1Pos, 0, 2, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0, 2, WHITE);
        // Checking if player is on ground, if they are their velocity goes to 0
        if(isOnGround(scarfyData, height)) {
            velocity = 0;
            isInAir = false;
        }
        // If they are not, gravity will pull them down until they are grounded again
        else {
            velocity += gravity * dT;
            isInAir = true;
        }
        // If space is pressed and character is not in air, character will jump up
        if(IsKeyPressed(KEY_SPACE) && !isInAir) {
            velocity -= jumpVelocity;
        }
        // Obstacles getting launched towards the player
        for(int i = 0; i < sizeOfNebulae; i++) {
            nebulae[i].pos.x -= nebVel * dT;
        }
        // Make the finish line go towards the player as well
        finishLine -= nebVel * dT;
        // Player's vertical position being dependant on velocity which is being influenced just a few lines above through if statements
        scarfyData.pos.y += velocity * dT;
        // Animates the player character
        if(!isInAir) {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }
        // Animates all the obstacles
        for(int i = 0; i < sizeOfNebulae; i++) {
            nebulae[i] = updateAnimData(nebulae[i], dT, 8);
        }
        // Setting up hitboxes for both the player and the obstacles and checking for collision
        for(AnimData n : nebulae) {
            float pad = 40;
            Rectangle nebRec {
                n.pos.x + 20,
                n.pos.y + 20,
                n.rec.width - 2 * pad,
                n.rec.height - 2 * pad
            };
            Rectangle scarfyRec {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if(CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;
            }
        }
        // If a collision happens, display a losing message on the screen
        if(collision) {
            DrawText("You lose!", width/3, 150, 24, RED);
        }
        // If the player reaches the finish line, display a winning message on the screen
        else if(scarfyData.pos.x >= finishLine) {
            DrawText("You win!", width/3, 150, 24, BLUE);
        }
        // If neither of these conditions are met, render the player and the obstacles
        else {
            for(int i = 0; i < sizeOfNebulae; i++) {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        EndDrawing();
    }
    // Unloading all game textures
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}