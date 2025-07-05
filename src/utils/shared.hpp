#pragma once


// Sets up the JFP directories and copies .jfpt files to the themes directory if needed.
// If bypass is true, always copy .jfpt files.
void setupJFPDirectories(bool bypass = false);
void setupJFPMusic();

extern GJGameLevel* commonLevel;