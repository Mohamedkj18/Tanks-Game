// include/TankAlgorithm/TankAlgorithm.h
#pragma once

#include <string>

// Forward declarations to avoid circular includes
class Tank;
class Game;

/**
 * Abstract base class for all tank control algorithms.
 */
class TankAlgorithm
{
public:
    /**
     * Decides the next action the tank should take.
     * This should return a string like:
     * "MOVE_FORWARD", "MOVE_BACKWARD", "ROTATE_LEFT", "ROTATE_RIGHT", "SHOOT", "DO_NOTHING", etc.
     */
    virtual std::string decideAction(Tank *tank, Game *game) = 0;

    /**
     * Virtual destructor (must match derived classes)
     */
    virtual ~TankAlgorithm() noexcept = default;
};
