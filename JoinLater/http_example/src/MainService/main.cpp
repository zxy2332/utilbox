
#include "ShortestEscape.h"
#include "util.h"
#include <iostream>

template<typename T>
void PrintVector(std::vector<T> vec) {
    for (auto it = vec.begin(); it != vec.end(); it++) {
        std::cout << *it << ",";
    }
    std::cout << std::endl;
}

#define try_catch

int main(int argc, char *argv[]) {

#ifdef try_catch
    try {
#endif
        std::string pathdata = "PathData.csv";
        std::string constraint = "约束.json";
        std::string out = "out.json";

        if (argc >= 2) {
            pathdata = argv[1];
        }

        if (argc >= 3) {
            constraint = argv[2];
        }

        if (argc >= 4) {
            out = argv[3];
        }

        ShortestEscape se(true);
        se.ReadPathDataFile(pathdata);

        std::string path;
        if (se.GetShortestPath(util::file_contents(constraint), path)) {
            std::cout << path << std::endl;
        }
        util::write_file(out, path);
#ifdef try_catch
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
#endif
    //system("pause");
    return 0;
}

