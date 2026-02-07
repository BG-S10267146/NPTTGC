#include "Game.h"
#include "../lib/CSVHelper.h"
#include <cstdlib>

std::string Game::csvHeader()
{
    return "name,minplayers,maxplayers,maxplaytime,minplaytime,yearpublished,isdeleted";
}

Game Game::fromCSVRow(const Vector<std::string>& row)
{
    static int nextId = 1;
    Game game;
    game.id = nextId++;
    game.name = trim(row.get(0));
    game.minPlayers = atoi(row.get(1).c_str());
    game.maxPlayers = atoi(row.get(2).c_str());
    game.maxPlaytime = atoi(row.get(3).c_str());
    game.minPlaytime = atoi(row.get(4).c_str());
    game.yearPublished = atoi(row.get(5).c_str());
    game.isDeleted = (row.getSize() >= 7) ? (atoi(row.get(6).c_str()) != 0) : false;
    return game;
}

Vector<std::string> Game::toCSVRow(const Game& game)
{
    Vector<std::string> row;
    row.append(game.name);
    row.append(std::to_string(game.minPlayers));
    row.append(std::to_string(game.maxPlayers));
    row.append(std::to_string(game.maxPlaytime));
    row.append(std::to_string(game.minPlaytime));
    row.append(std::to_string(game.yearPublished));
    row.append(std::to_string(game.isDeleted ? 1 : 0));
    return row;
}
