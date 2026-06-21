#include <map>

struct LadderDemon {
    int id = 0;
    double tier = 0.0;
    double enjoyment = 0.0;
    int difficulty = 0;
};

namespace ExtremesInBetween {
    extern std::map<int, LadderDemon> gddl;

    LadderDemon* demonForLevel(int levelID);
}
