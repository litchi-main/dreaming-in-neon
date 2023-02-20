#pragma once

const int WINDOW_WIDTH = 1200;	// ekrano plotis
const int WINDOW_HEIGHT = 800;	// ekrano aukstis
const int CAMERA_WIDTH = 720;
const int CAMERA_HEIGHT = 480;

const float round_start = 150;
const float ground = WINDOW_HEIGHT - 80;
enum side {left , right, none};
enum Nstate {neutral, air};
enum Astate {hitstun, block, airblock, attack, recovery, airatt, airrec, rest};

