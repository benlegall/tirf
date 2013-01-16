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
    void dilate2();
    void save_filtered(std::string filename);
    void gradient_graph();
    void connex_graph();
    void graph_color();
    std::vector<zone> find_red_zones();
    std::vector<zone> find_white_zones(std::vector<zone> zones,
                                       int enlarger);
    void explore_white_zone(int i, int j, zone *current, zone red);
    void clean_red_zone(zone red);
    bool dismiss_red_zone(zone red);

    bool is_white(cv::Vec3b pix);
    bool is_black(cv::Vec3b pix);
    bool is_blue(cv::Vec3b pix);
    bool is_green(cv::Vec3b pix);
    bool is_red(cv::Vec3b pix);


    std::vector<zone> clean_white_zone(std::vector<zone> zones);
    bool inter_zone(zone z1, zone z2);
    std::vector<zone> join_white_zones(std::vector<zone> zones);
    zone join_zones(zone z1, zone z2);
    void evaluate_white_zone(std::vector<zone>& zones);
    zone follow_path(int i, int j);
    void draw_zones_green(std::vector<zone> zones);
    void draw_zones_blue(std::vector<zone> zones);

    int calc_proj_hor(float x1, float y1, float x2, float y2);
    int calc_proj_vert(float x1, float y1, float x2, float y2);

    void reread(std::string filename);

    void inc_red_coef();
private:
    cv::Mat image;
    cv::Mat filtered;
    int width;
    int height;
    int **graph;
    int red_coef;
};




#endif
