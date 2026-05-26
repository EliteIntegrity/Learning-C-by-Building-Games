#include "Pathfind.h"
#include "Map.h"

#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>

namespace
{
    struct Node
    {
        Point p;
        int   f;        // estimated total cost = g + heuristic
    };

    struct NodeGreater
    {
        bool operator()(const Node& a, const Node& b) const
        {
            return a.f > b.f;   // min-heap on `f`
        }
    };

    int manhattan(Point a, Point b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
}

namespace Pathfind
{
    std::vector<Point> astar(const Map& map, Point from, Point to)
    {
        std::vector<Point> result;
        if (from == to) return result;
        if (map.isBlocked(to.x, to.y)) return result;

        std::priority_queue<Node, std::vector<Node>, NodeGreater> open;
        std::unordered_map<Point, Point> cameFrom;
        std::unordered_map<Point, int>   gScore;

        open.push({ from, manhattan(from, to) });
        gScore[from] = 0;

        const int dx[4] = { 0, 0, -1,  1 };
        const int dy[4] = { -1, 1, 0,  0 };

        while (!open.empty())
        {
            Node cur = open.top();
            open.pop();

            if (cur.p == to) break;

            int curG = gScore[cur.p];

            for (int i = 0; i < 4; ++i)
            {
                Point n { cur.p.x + dx[i], cur.p.y + dy[i] };
                if (!map.inBounds(n.x, n.y)) continue;
                if (map.isBlocked(n.x, n.y)) continue;

                int tentativeG = curG + 1;
                auto it = gScore.find(n);
                if (it == gScore.end() || tentativeG < it->second)
                {
                    gScore[n]   = tentativeG;
                    cameFrom[n] = cur.p;
                    open.push({ n, tentativeG + manhattan(n, to) });
                }
            }
        }

        // Reconstruct the path back from `to` to (just after) `from`.
        if (cameFrom.find(to) == cameFrom.end()) return result;   // unreachable
        Point step = to;
        while (step != from)
        {
            result.push_back(step);
            step = cameFrom[step];
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
}
