#ifndef GAME_H
#define GAME_H

#include <string>
#include "../lib/Vector.h"

/// Game data structure
struct Game
{
    int id;
    std::string name;
    int minPlayers;
    int maxPlayers;
    int minPlaytime;
    int maxPlaytime;
    int yearPublished;
    bool isDeleted;

    Game() : id(0), name(""), minPlayers(0), maxPlayers(0),
             minPlaytime(0), maxPlaytime(0), yearPublished(0), isDeleted(false) {}

    Game(int id, const std::string &name, int minPlayers, int maxPlayers,
         int minPlaytime, int maxPlaytime, int yearPublished, bool isDeleted = false)
        : id(id), name(name), minPlayers(minPlayers), maxPlayers(maxPlayers),
          minPlaytime(minPlaytime), maxPlaytime(maxPlaytime),
          yearPublished(yearPublished), isDeleted(isDeleted) {}

    static std::string csvHeader();
    static Game fromCSVRow(const Vector<std::string> &row);
    static Vector<std::string> toCSVRow(const Game &game);
};

#endif
