#ifndef SHORTEST_ALGORITHM_H
#define SHORTEST_ALGORITHM_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

template<class _KT, class _VT>
class ShortestAlgorithm
{
    std::unordered_map<_KT, std::unordered_map<_KT, _VT>> vertices;
public:
    static constexpr _VT INVALID = std::numeric_limits<_VT>::max();
    void clear(){
        vertices.clear();
    }
    std::unordered_map<_KT, std::unordered_map<_KT, _VT>> &Vertices(){
        return vertices;
    }
    //插入一条边
    //directed = true 有向
    //directed = false 无向
    void update_edge(_KT from, _KT end, _VT value,bool directed)
    {
        vertices[from][end] = value;
        if (directed == false){
            //若为无向，反向插入一条边
            vertices[end][from] = value;
        }
    }

    void add_vertex(_KT name, std::unordered_map<_KT, _VT>& edges)
    {
        vertices.emplace(name, edges);
    }

    std::vector<_KT> shortest_path(_KT start, _KT finish)
    {
        std::unordered_map<_KT, _VT> distances;
        std::unordered_map<_KT, _KT> previous;
        std::vector<_KT> nodes;
        std::vector<_KT> path;

        auto comparator = [&] (_KT left, _KT right) { return distances[left] > distances[right]; };

        for (auto& vertex : vertices)
        {
            if (vertex.first == start)
            {
                distances[vertex.first] = 0;
            }
            else
            {
                distances[vertex.first] = INVALID;
            }

            nodes.push_back(vertex.first);
            push_heap(begin(nodes), end(nodes), comparator);
        }

        while (!nodes.empty())
        {
            pop_heap(begin(nodes), end(nodes), comparator);
            _KT smallest = nodes.back();
            nodes.pop_back();

            if (smallest == finish)
            {
                while (previous.find(smallest) != end(previous))
                {
                    path.push_back(smallest);
                    smallest = previous[smallest];
                }

                break;
            }

            if (distances[smallest] == INVALID)
            {
                break;
            }

            for (auto& neighbor : vertices[smallest])
            {
                _VT alt = distances[smallest] + neighbor.second;
                if (alt < distances[neighbor.first])
                {
                    distances[neighbor.first] = alt;
                    previous[neighbor.first] = smallest;
                    make_heap(begin(nodes), end(nodes), comparator);
                }
            }
        }
        if (!path.empty()){
            path.push_back(start);
        }
        //倒序
        std::reverse(std::begin(path), std::end(path));
        return path;
    }
};

#endif