#pragma once

const int WINDOW_WIDTH = 1500;	// ekrano plotis
const int WINDOW_HEIGHT = 800;	// ekrano aukstis

const float round_start = 60;
const float ground = WINDOW_HEIGHT - 80;
enum side {left , right, none};
enum Nstate {neutral, air};
enum Astate {hitstun, block, airblock, attack, recovery, airatt, airrec, rest};

