#ifndef FILTRE_HH_
# define FILTRE_HH_

#include <string>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <sstream>

struct zone {
    int max_i;
    int max_j;
    int min_i;
    int min_j;
};

class filtre {
public:

    filtre(std::string filename);

    void contrast(int contrast);
    void binarize_red();
    void binarize_white();
    void erode();
    void dilate();
    void save_filtered(std::string filename);
    void gradient_graph();
    void connex_graph();
    void graph_color();
    void find_red_zones();
    zone follow_path(int i, int j);
    void draw_zones(std::vector<zone> zones);

private:
    cv::Mat image;
    cv::Mat filtered;
    int width;
    int height;
    int **graph;
};




#endif
