#pragma once

const int WINDOW_WIDTH = 1200;	// ekrano plotis
const int WINDOW_HEIGHT = 720;	// ekrano aukstis
const int CAMERA_WIDTH = 720;   //5:3
const int CAMERA_HEIGHT = 432;

const float round_start = 150;
const float ground = WINDOW_HEIGHT - 40;
enum side {left , right, none};
enum Nstate {neutral, air};
enum Astate {hitstun, block, airblock, attack, recovery, airatt, airrec, rest};

