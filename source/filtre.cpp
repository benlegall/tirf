#include "filtre.hh"

filtre::filtre(std::string filename) {
    image = cv::imread(filename);

    red_coef = 0;
    width = image.rows;
    height = image.cols;
    filtered = cv::Mat::zeros(width, height, image.type());
    graph = new int*[width];
    for (int i = 0; i < width; ++i)
    {
        graph[i] = new int[height];
    }
}


void filtre::reread(std::string filename) {
    image = cv::imread(filename);
}

void filtre::contrast(int contrast) {

    for (int i = 0; i < width; ++i)
    {
     for (int j = 0; j < height; ++j)
     {
         cv::Vec3b pix = image.at<cv::Vec3b>(i, j);
         int new_blue = pix[0] + contrast * (pix[0] - 127) / 100;
         int new_green = pix[1] + contrast * (pix[1] - 127) / 100;
         int new_red = pix[2] + contrast * (pix[2] - 127) / 100;

         if (new_blue < 0)
             new_blue = 0;
         if (new_blue > 255)
             new_blue = 255;

         if (new_green < 0)
             new_green = 0;
         if (new_green > 255)
             new_green = 255;

         if (new_red < 0)
             new_red = 0;
         if (new_red > 255)
             new_red = 255;

         image.at<cv::Vec3b>(i, j)
             = cv::Vec3b(new_blue, new_green, new_red);

     }
    }
}

//////
////// cv::Vec3b est au format BGR pas RGB
//////
void filtre::binarize_red() {

    for (int i = 0; i < width; ++i)
    {
     for (int j = 0; j < height; ++j)
     {
         cv::Vec3b pix = image.at<cv::Vec3b>(i, j);
         if (is_red(pix))
             filtered.at<cv::Vec3b>(i, j)
                 = cv::Vec3b(255, 255, 255);
         else
             filtered.at<cv::Vec3b>(i, j)
                 = cv::Vec3b(0, 0, 0);

     }
    }
}

void filtre::binarize_white() {

    for (int i = 0; i < width; ++i)
    {
     for (int j = 0; j < height; ++j)
     {
         cv::Vec3b pix = image.at<cv::Vec3b>(i, j);
         if (is_white(pix))
             filtered.at<cv::Vec3b>(i, j)
                 = cv::Vec3b(255, 255, 255);
         else
             filtered.at<cv::Vec3b>(i, j)
                 = cv::Vec3b(0, 0, 0);

     }
    }
}

void filtre::save_filtered(std::string filename) {
    cv::imwrite(filename, filtered);
}


void filtre::erode() {

    int erode_factor = 1;
    int nb_pix_eroded = 9;
    cv::Mat eroded = cv::Mat::zeros(width, height, filtered.type());
    int pixel_value = 0;

    for (int i = erode_factor; i < width - erode_factor; ++i)
    {
        for (int j = erode_factor; j < height - erode_factor; ++j)
        {
            for (int k = -erode_factor; k <= erode_factor; ++k)
            {
                for (int l = -erode_factor; l <= erode_factor; ++l)
                {
                    pixel_value += filtered.at<cv::Vec3b>(i+k, j+l)[0];
                }
            }

            if (pixel_value == nb_pix_eroded * 255)
            {
                for (int k = -erode_factor; k <= erode_factor; ++k)
                {
                    for (int l = -erode_factor; l <= erode_factor; ++l)
                    {
                        eroded.at<cv::Vec3b>(i + k, j + l)
                            = cv::Vec3b(255, 255, 255);
                    }
                }
            }
            pixel_value = 0;
        }
    }
    filtered = eroded;
}

void filtre::dilate() {

    int dilate_factor = 2;
    cv::Mat dilated = cv::Mat::zeros(width, height, filtered.type());
    int pixel_value = 0;

    for (int i = dilate_factor; i < width - dilate_factor; ++i)
    {
        for (int j = dilate_factor; j < height - dilate_factor; ++j)
        {
            for (int k = -dilate_factor; k <= dilate_factor; ++k)
            {
                for (int l = -dilate_factor; l <= dilate_factor; ++l)
                {
                    pixel_value += filtered.at<cv::Vec3b>(i+k, j+l)[0];
                }
            }

            if (pixel_value > 0)
            {
                for (int k = -dilate_factor; k <= dilate_factor; ++k)
                {
                    for (int l = -dilate_factor; l <= dilate_factor; ++l)
                    {
                        dilated.at<cv::Vec3b>(i + k, j + l)
                            = cv::Vec3b(255, 255, 255);
                    }
                }
            }
            pixel_value = 0;
        }
    }
    filtered = dilated;
}

void filtre::dilate2() {

    int dilate_factor = 1;
    cv::Mat dilated = cv::Mat::zeros(width, height, filtered.type());

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            dilated.at<cv::Vec3b>(i, j)
                = filtered.at<cv::Vec3b>(i, j);
        }
    }

    for (int i = dilate_factor; i < width - dilate_factor; ++i)
    {
        for (int j = dilate_factor; j < height - dilate_factor; ++j)
        {
            bool black_pix_found = false;
            for (int k = -dilate_factor; k <= dilate_factor; ++k)
            {
                for (int l = -dilate_factor; l <= dilate_factor; ++l)
                {
                    if (is_black(filtered.at<cv::Vec3b>(i+k, j+l))) {
                        black_pix_found = true;
                    }
                }
            }
            if (black_pix_found)
            {
                for (int k = -dilate_factor; k <= 0; ++k)
                {
                    for (int l = -dilate_factor; l <= 0; ++l)
                    {
                        dilated.at<cv::Vec3b>(i + k, j + l)
                            = cv::Vec3b(0, 0, 0);
                    }
                }
            }
        }
    }
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            filtered.at<cv::Vec3b>(i, j)
                = dilated.at<cv::Vec3b>(i, j);
        }
    }
}

void filtre::connex_graph() {

    int connexity = 0;
    for (int i = 1; i < width - 1; ++i)
    {
        for (int j = 1; j < height - 1; ++j)
        {
            cv::Vec3b pix = filtered.at<cv::Vec3b>(i, j);
            if (pix[0] == 255) {
                for (int k = -1; k <= 1; ++k)
                {
                    for (int l = -1; l <= 1; ++l)
                    {
                        if (!((k == 0) && (l == 0)))
                            connexity +=
                                (filtered.at<cv::Vec3b>(i+k, j+l)[0]
                                 == 255) ? 1 : 0;
                    }
                }
                if (connexity <= 7) {
                    graph[i][j] = 1;
                } else {
                    graph[i][j] = 0;
                }
                connexity = 0;
            } else {
                graph[i][j] = 0;
            }
        }
    }
}


void filtre::graph_color() {
    //int sum = 0;
    filtered = image;
    for (int i = 1; i < width - 1; ++i)
    {
        for (int j = 1; j < height - 1; ++j)
        {
            if (graph[i][j] == 1) {
                filtered.at<cv::Vec3b>(i, j)
                    = cv::Vec3b(0, 255, 0);
            }
        }
    }
}

void filtre::draw_zones_green(std::vector<zone> zones) {
    filtered = image;

    for (std::vector<zone>::iterator it = zones.begin();
         it != zones.end(); it++)
    {
        for (int i = (*it).min_i; i < (*it).max_i; ++i)
        {
            filtered.at<cv::Vec3b>(i, (*it).min_j)
                = cv::Vec3b(0, 255, 0);
            filtered.at<cv::Vec3b>(i, (*it).max_j)
                = cv::Vec3b(0, 255, 0);
        }
        for (int j = (*it).min_j; j < (*it).max_j; ++j)
        {
            filtered.at<cv::Vec3b>((*it).min_i, j)
                = cv::Vec3b(0, 255, 0);
            filtered.at<cv::Vec3b>((*it).max_i, j)
                = cv::Vec3b(0, 255, 0);
        }
    }
}

void filtre::draw_zones_blue(std::vector<zone> zones) {
    filtered = image;

    for (std::vector<zone>::iterator it = zones.begin();
         it != zones.end(); it++)
    {
        for (int i = (*it).min_i; i < (*it).max_i; ++i)
        {
            filtered.at<cv::Vec3b>(i, (*it).min_j)
                = cv::Vec3b(255, 0, 0);
            filtered.at<cv::Vec3b>(i, (*it).max_j)
                = cv::Vec3b(255, 0, 0);
        }
        for (int j = (*it).min_j; j < (*it).max_j; ++j)
        {
            filtered.at<cv::Vec3b>((*it).min_i, j)
                = cv::Vec3b(255, 0, 0);
            filtered.at<cv::Vec3b>((*it).max_i, j)
                = cv::Vec3b(255, 0, 0);
        }
    }
}


std::vector<zone> filtre::find_red_zones() {
    std::vector<zone> red_zones;
    zone current_zone;

    for (int i = 1; i < width - 1; ++i)
    {
        for (int j = 1; j < height - 1; ++j)
        {
            if ((graph[i][j] == 1) &&
                (filtered.at<cv::Vec3b>(i, j)[0] != 0))
            {
                current_zone = follow_path(i, j);
                if ((current_zone.min_i != current_zone.max_i) &&
                    (current_zone.min_j != current_zone.max_j) &&
                    (current_zone.max_j - current_zone.min_j > 80) &&
                    (current_zone.max_i - current_zone.min_i > 80))
                red_zones.push_back(current_zone);
            }
        }
    }
    return red_zones;
}

zone filtre::follow_path(int i, int j) {
    bool cont = true;
    zone current_zone;
    current_zone.min_i = i;
    current_zone.max_i = i;
    current_zone.min_j = j;
    current_zone.max_j = j;


    while (cont) {
        cont = false;

        for (int k = -1; k <= 1; ++k)
        {
            for (int l = -1; l <= 1; ++l)
            {
                if ((!((k == 0) && (l == 0))) &&
                    (i > 0) && (j > 0) && (i < width - 1) &&
                    (j < height - 1) && (graph[i+k][j+l] == 1)) {
                    cont = true;
                    if (j+l < current_zone.min_j)
                        current_zone.min_j = j+l;
                    if (j+l > current_zone.max_j)
                        current_zone.max_j = j+l;
                    if (i+k < current_zone.min_i)
                        current_zone.min_i = i+k;
                    if (i+k > current_zone.max_i)
                        current_zone.max_i = i+k;

                    graph[i][j] = -1;
                    i = i + k;
                    j = j + l;
                }
                if (cont)
                    break;
            }
            if (cont)
                break;
        }
   }
    return current_zone;
}

void filtre::clean_red_zone(zone red) {
    for (int j = red.min_j; j < red.max_j; ++j)
    {
        cv::Vec3b pix = image.at<cv::Vec3b>(red.min_i, j);
        int k = 0;
        while (is_white(pix) && (red.min_i + k < red.max_i))
        {
            filtered.at<cv::Vec3b>(red.min_i + k, j)
                = cv::Vec3b(0, 0, 0);
            k++;
            pix = image.at<cv::Vec3b>(red.min_i + k, j);
        }

        pix = image.at<cv::Vec3b>(red.max_i, j);
        k = 0;
        while (is_white(pix) && (red.max_i - k > red.min_i))
        {
            filtered.at<cv::Vec3b>(red.max_i - k, j)
                = cv::Vec3b(0, 0, 0);
            k++;
            pix = image.at<cv::Vec3b>(red.max_i - k, j);
        }
    }
}

bool filtre::dismiss_red_zone(zone red) {
    for (int i = red.min_i; i < red.max_i; ++i)
    {
        for (int j = red.min_j; j < red.max_j; ++j)
        {
            cv::Vec3b pix = filtered.at<cv::Vec3b>(i, j);

            if ((is_green(pix)) || (is_blue(pix)))
                return false;
        }
    }
    return true;
}

std::vector<zone> filtre::find_white_zones(std::vector<zone> zones,
                                           int enlarger) {
    std::vector<zone> white_zones;
    std::vector<zone> pre_white_zones;

    zone *current_zone = new zone;

    filtered = image;
    for (std::vector<zone>::iterator it = zones.begin();
         it != zones.end(); it++)
    {
        clean_red_zone(*it);

        if (dismiss_red_zone(*it)) {
            (*current_zone).max_j = 0;
            (*current_zone).min_j = 0;
            (*current_zone).max_i = 0;
            (*current_zone).min_i = 0;
            for (int i = (*it).min_i; i < (*it).max_i; ++i)
            {
                for (int j = (*it).min_j; j < (*it).max_j; ++j)
                {
                    cv::Vec3b pix = image.at<cv::Vec3b>(i, j);
                    if (is_white(pix))  {
                        (*current_zone).max_j = j;
                        (*current_zone).min_j = j;
                        (*current_zone).max_i = i;
                        (*current_zone).min_i = i;

                        explore_white_zone(i, j, current_zone, (*it));

                        zone save = *current_zone;
                        if ((save.max_j - save.min_j > 4) &&
                            (save.max_i - save.min_i > 4) &&
                            (save.min_i - enlarger > 0) &&
                            (save.min_j - enlarger > 0) &&
                            (save.max_i + enlarger < width) &&
                            (save.max_j + enlarger < height)) {
                            save.max_j += enlarger;
                            save.max_i += enlarger;
                            save.min_j -= enlarger;
                            save.min_i -= enlarger;

                            pre_white_zones.push_back(save);
                        }
                    }
                }
            }
        }
        pre_white_zones = clean_white_zone(pre_white_zones);

        for (unsigned int i = 0; i < pre_white_zones.size(); ++i)
            white_zones.push_back(pre_white_zones.at(i));
        pre_white_zones.clear();
    }

    return join_white_zones(white_zones);
}


std::vector<zone> filtre::clean_white_zone(std::vector<zone> zones) {

    int size = zones.size();

    for (int i = 0; i < size; ++i)
    {
        int j = 0;
        bool cont  = true;
        while ((j < size) && (cont))
        {
            if (i != j) {
                if (inter_zone(zones.at(i), zones.at(j)))
                    cont = false;
            }
            j++;
        }
        if (cont) {
            zones.erase(zones.begin() + i);
            i--;
        }
        size = zones.size();
    }
    return zones;
}


std::vector<zone> filtre::join_white_zones(std::vector<zone> zones) {

    int size = zones.size();

    for (int i = 0; i < size; ++i)
    {
        int j = 0;
        while (j < size)
        {
            if (i != j) {
                if (inter_zone(zones.at(i), zones.at(j))) {
                    zones.at(i) = join_zones(zones.at(i), zones.at(j));
                    zones.erase(zones.begin() + j);
                    j--;

                    if (i > j)
                        i--;
                    size = zones.size();
                }
            }
            j++;
        }
    }

    return zones;
}

void filtre::evaluate_white_zone(std::vector<zone>& zones) {

//    image = cv::imread(file);
//    filtered = image;
    int proj_val;
    std::vector<int> projection;

    for (std::vector<zone>::iterator it = zones.begin();
         it != zones.end();)
    {
        int size_i = (*it).max_i - (*it).min_i;
        int size_j = (*it).max_j - (*it).min_j;

        int sum = 0;


        if ((size_j > size_i) && (size_i > 20)) {
            int step = std::ceil(size_i / 20);

            for (int i = 0; i < 21; ++i) {

                proj_val = calc_proj_hor((*it).min_i + i * step,
                                         (*it).min_j,
                                         (*it).max_i - i * step,
                                         (*it).max_j);

                    //std::cout << proj_val << ";";
                    sum += proj_val;
            }
        } else if ((size_i > size_j) && (size_j > 20)) {
            int step = std::ceil(size_j / 20);
            //std::cout << "in the if for vert" << std::endl;

            for (int i = 0; i < 21; ++i) {
                    proj_val = calc_proj_vert((*it).min_i,
                                              (*it).min_j + i * step,
                                              (*it).max_i,
                                              (*it).max_j - i * step);

                    sum += proj_val;
            }
        }
        if ((sum >= 200) && (sum <= 300)) {
            std::cout << "coca found with sum  = " << sum << std::endl
                      << std::endl;
            it++;
        } else {
            it = zones.erase(it);
        }
        projection.clear();
    }
}

int filtre::calc_proj_hor(float x1, float y1, float x2, float y2) {

    int nb_white_zone_crossed = 0;
    bool in_white = false;
    cv::Vec3b pix = image.at<cv::Vec3b>(x1, y1);
    cv::Vec3b next_pix;

    if (y2 - y1 != 0) {
        float a = (y2 - y1) / (x2 - x1);
        float b = y1 - a * x1;

        for (int j = y1; j < y2; ++j) {
            if (a != 0) {
                pix = image.at<cv::Vec3b>(round(float((j - b)) / a),
                                             j);
                next_pix =
                    image.at<cv::Vec3b>(round(float((j + 1 - b)) / a),
                                        j+1);

                if (in_white) {
                    if (!is_black(pix) && !is_black(next_pix)) {
                        in_white = false;

                    }

                } else {
                    if (is_black(pix)) {
                        nb_white_zone_crossed++;
                        in_white = true;
                    }
                }
            } else {
                pix = filtered.at<cv::Vec3b>(x1, j);
                next_pix = filtered.at<cv::Vec3b>(x1, j + 1);

                if (in_white) {
                    if(!is_black(pix) && !is_black(next_pix)) {
                        in_white = false;
                    }
                } else {
                    if (is_black(pix)) {
                        nb_white_zone_crossed++;
                        in_white = true;
                    }
                }
            }
        }
    }
    return nb_white_zone_crossed;
}

int filtre::calc_proj_vert(float x1, float y1, float x2, float y2) {

    //filtered = image;

    int nb_white_zone_crossed = 0;
    bool in_white = false;
    cv::Vec3b pix = filtered.at<cv::Vec3b>(x1, y1);
    cv::Vec3b next_pix;

    if (x2 - x1 != 0) {
        float a = (y2 - y1) / (x2 - x1);
        float b = y1 - a * x1;

        for (int i = x1; i < x2; ++i) {
            if (a != 0) {
                pix = image.at<cv::Vec3b>(i, a * i + b);
                next_pix = image.at<cv::Vec3b>(i + 1, a * (i + 1) + b);

                if (in_white) {
                    if (!is_black(pix) && !is_black(next_pix)) {
                        in_white = false;
                    }
                } else {
                    if (is_black(pix)) {
                        nb_white_zone_crossed++;
                        in_white = true;
                    }

                }
            } else {
                pix = image.at<cv::Vec3b>(i, y1);
                next_pix = image.at<cv::Vec3b>(i + 1, y1);

                if (in_white) {
                    if (!is_black(pix) && !is_black(next_pix)) {
                        in_white = false;
                    }

                } else {
                    if (is_black(pix)) {
                        nb_white_zone_crossed++;
                        in_white = true;
                    }
                }
            }
        }
    }
    return nb_white_zone_crossed;
}


zone filtre::join_zones(zone z1, zone z2) {
    if (z2.min_i < z1.min_i)
        z1.min_i = z2.min_i;

    if (z2.max_i > z1.max_i)
        z1.max_i = z2.max_i;

    if (z2.min_j < z1.min_j)
        z1.min_j = z2.min_j;

    if (z2.max_j > z1.max_j)
        z1.max_j = z2.max_j;

    return z1;
}

void filtre::explore_white_zone(int i, int j, zone *current, zone red) {

    filtered.at<cv::Vec3b>(i, j)
        = cv::Vec3b(0, 0, 0);

    for (int k = -1; k <= 1; ++k)
    {
        for (int l = -1; l <= 1; ++l)
        {
            if ((!((k == 0) && (l == 0))) &&
                (i + k < red.max_i) && (j + l < red.max_j) &&
                (i + k > red.min_i) && (j + l > red.min_j)) {

                cv::Vec3b pix = image.at<cv::Vec3b>(i + k, j + l);
                if (is_white(pix))  {

                    if (j < (*current).min_j)
                        (*current).min_j = j;
                    if (j > (*current).max_j)
                        (*current).max_j = j;
                    if (i < (*current).min_i)
                        (*current).min_i = i;
                    if (i > (*current).max_i)
                        (*current).max_i = i;

                    explore_white_zone(i + k, j + l, current, red);
                }
            }
        }
    }
}

bool filtre::is_white(cv::Vec3b pix) {
    return ((pix[2] > 150) && (pix[1] > 150) &&
            (pix[0] > 150));
}

bool filtre::is_red(cv::Vec3b pix) {
    return ((pix[2] > 195 - red_coef) && (pix[1] < 40 + red_coef) &&
            (pix[0] < 40 + red_coef));
}

bool filtre::is_black(cv::Vec3b pix) {
    return ((pix[2] == 0) && (pix[1] == 0) &&
            (pix[0] == 0));
}

bool filtre::is_blue(cv::Vec3b pix) {
    return ((pix[0] > 195) && (pix[1] > 150) && (pix[2] < 40));
}

bool filtre::is_green(cv::Vec3b pix) {
        return ((pix[1] > 195) && (pix[0] < 40) && (pix[2] < 40));
}


bool filtre::inter_zone(zone z1, zone z2) {

    if ((z1.max_i >= z2.min_i) && (z1.max_i <= z2.max_i) &&
        (z1.max_j >= z2.min_j) && (z1.max_j <= z2.max_j))
        return true;

    if ((z1.max_i >= z2.min_i) && (z1.max_i <= z2.max_i) &&
        (z1.min_j >= z2.min_j) && (z1.min_j <= z2.max_j))
        return true;

    if ((z1.min_i >= z2.min_i) && (z1.min_i <= z2.max_i) &&
        (z1.max_j >= z2.min_j) && (z1.max_j <= z2.max_j))
        return true;

    if ((z1.min_i >= z2.min_i) && (z1.min_i <= z2.max_i) &&
        (z1.min_j >= z2.min_j) && (z1.min_j <= z2.max_j))
        return true;


    if ((z2.max_i >= z1.min_i) && (z2.max_i <= z1.max_i) &&
        (z2.max_j >= z1.min_j) && (z2.max_j <= z1.max_j))
        return true;

    if ((z2.max_i >= z1.min_i) && (z2.max_i <= z1.max_i) &&
        (z2.min_j >= z1.min_j) && (z2.min_j <= z1.max_j))
        return true;

    if ((z2.min_i >= z1.min_i) && (z2.min_i <= z1.max_i) &&
        (z2.max_j >= z1.min_j) && (z2.max_j <= z1.max_j))
        return true;

    if ((z2.min_i >= z1.min_i) && (z2.min_i <= z1.max_i) &&
        (z2.min_j >= z1.min_j) && (z2.min_j <= z1.max_j))
        return true;


    return false;
}


void filtre::inc_red_coef() {
    red_coef += 25;
}
