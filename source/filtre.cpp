#include "filtre.hh"

filtre::filtre(std::string filename) {
    image = cv::imread(filename);

    width = image.rows;
    height = image.cols;
    filtered = cv::Mat::zeros(width, height, image.type());
    graph = new int*[width];
    for (int i = 0; i < width; ++i)
    {
        graph[i] = new int[height];
    }
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
         if ((pix[2] > 195) && (pix[1] < 40) && (pix[0] < 40))
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
         if ((pix[2] > 230) && (pix[1] > 230) && (pix[0] > 230))
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

void filtre::gradient_graph() {
    int gradient_hor = 0;
    int gradient_vert = 0;
    int c = 2;

    for (int i = 1; i < width - 1; ++i)
    {
        for (int j = 1; j < height - 1; ++j)
        {
            gradient_hor = -filtered.at<cv::Vec3b>(i-1, j-1)[0] -
                (c * filtered.at<cv::Vec3b>(i-1, j)[0]) -
                filtered.at<cv::Vec3b>(i-1, j+1)[0] +
                filtered.at<cv::Vec3b>(i+1, j-1)[0] +
                (c * filtered.at<cv::Vec3b>(i+1, j)[0]) +
                filtered.at<cv::Vec3b>(i+1, j+1)[0];

            gradient_vert = -filtered.at<cv::Vec3b>(i-1, j-1)[0] -
                (c * filtered.at<cv::Vec3b>(i, j-1)[0]) -
                filtered.at<cv::Vec3b>(i+1, j-1)[0] +
                filtered.at<cv::Vec3b>(i-1, j+1)[0] +
                (c * filtered.at<cv::Vec3b>(i, j+1)[0]) +
                filtered.at<cv::Vec3b>(i+1, j+1)[0];

            if ((gradient_hor == 0 ) && (gradient_vert == 0))
                graph[i][j] = 0;

            if (gradient_hor != 0) {
                cv::Vec3b before_pix = image.at<cv::Vec3b>(i-1, j);
                cv::Vec3b after_pix = image.at<cv::Vec3b>(i+1, j);

                if ((((before_pix[2] > 195) && (before_pix[1] < 50) &&
                      (before_pix[0] < 50)) &&
                     ((after_pix[2] < 50) && (after_pix[1] < 50) &&
                      (after_pix[0] < 50))) ||
                    (((after_pix[2] > 195) && (after_pix[1] < 50) &&
                      (after_pix[0] < 50)) &&
                     ((before_pix[2] < 50) && (before_pix[1] < 50) &&
                      (before_pix[0] < 50))))//red to white
                {
                    graph[i][j] = 2;
                    std::cout << "red to white or white " <<
                        "to red gradient_hor = " <<
                        gradient_hor << std::endl;

                } else {
                    if (graph[i-1][j] != 1)
                        graph[i][j] = 1;
                    else
                        graph[i][j] = 0;
                    std::cout << "something to red gradient_hor = " <<
                        gradient_hor << std::endl;
                }
            }
            if (gradient_vert != 0) {
                cv::Vec3b before_pix = image.at<cv::Vec3b>(i, j - 1);
                cv::Vec3b after_pix = image.at<cv::Vec3b>(i, j + 1);

                if ((((before_pix[2] > 195) && (before_pix[1] < 50) &&
                      (before_pix[0] < 50)) &&
                     ((after_pix[2] < 50) && (after_pix[1] < 50) &&
                      (after_pix[0] < 50))) ||
                    (((after_pix[2] > 195) && (after_pix[1] < 50) &&
                      (after_pix[0] < 50)) &&
                     ((before_pix[2] < 50) && (before_pix[1] < 50) &&
                      (before_pix[0] < 50))))//red to white
                {
                    graph[i][j] = 2;
                    std::cout << "red to white or white " <<
                        "to red gradient_vert = " <<
                        gradient_vert << std::endl;

                } else {
                    if (graph[i][j-1] != 1)
                        graph[i][j] = 1;
                    else
                        graph[i][j] = 0;
                    std::cout << "something to red gradient_vert = " <<
                        gradient_vert << std::endl;
                }
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

                /*if ((i < width - 2) && (j < height - 2)) {
                    for (int k = 0; i < 2; ++k)
                    {
                        for (int l = 0; i < 2; ++i)
                        {
                            sum += graph[i + k][ j + l];
                        }
                    }
                    if (sum > 4) {
                        for (int k = 1; i < 2; ++k)
                        {
                            for (int l = 1; i < 2; ++i)
                            {
                                graph[i + k][j + l] = 0;
                            }
                        }
                    }
                }*/
            }
            //if (graph[i][j] == 2)
            //    filtered.at<cv::Vec3b>(i, j)
            //        = cv::Vec3b(255, 0, 0);

        }
    }
}

void filtre::draw_zones(std::vector<zone> zones) {
    filtered = image;

    //std::cout << "in red zones" << std::endl;


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

    for (int i = 1; i < width - 1; ++i)
    {
        for (int j = 1; j < height - 1; ++j)
        {
            if (graph[i][j] == -1)
                filtered.at<cv::Vec3b>(i, j)
                    = cv::Vec3b(255, 0, 0);
        }
    }
}

void filtre::find_red_zones() {
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

    std::cout << "size of vector zones = " << red_zones.size()
              << std::endl;


    draw_zones(red_zones);
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

        /* for (int k = -2; k <= 2; ++k)
        {
            for (int l = -2; l <= 2; ++l)
            {
                if ((!((k == 0) && (l == 0))) &&
                    ((k == -2) || (k == 2) || (l == 2) || (l == -2)) &&
                    (i > 1) && (j > 1) && (i < width - 2) &&
                    (j < height - 2) && (graph[i+k][j+l] == 1)) {
                    cont = true;
                    if (j+l < current_zone.min_j)
                        current_zone.min_j = j+l;
                    if (j+l > current_zone.max_j)
                        current_zone.max_j = j+l;
                    if (i+k < current_zone.min_i)
                        current_zone.min_i = i+k;
                    if (i+k > current_zone.max_i)
                        current_zone.max_i = i+k;

                    graph[i][j] = 0;
                    i = i + k;
                    j = j + l;
                    break;
                }
                if (cont)
                    break;
            }
        }*/


    }
    return current_zone;
}














        /*if ((i > 1) && (j > 1) && (i < width - 2) &&
            (j < height - 2)) {
            if (graph[i-2][j] == 1) {
                cont = true;
                if (j < current_zone.min_j)
                    current_zone.min_j = j;
                if (j > current_zone.max_j)
                    current_zone.max_j = j;
                if (i-2 < current_zone.min_i)
                    current_zone.min_i = i -2;
                if (i - 2 > current_zone.max_i)
                    current_zone.max_i = i - 2;

                graph[i][j] = 0;
                i = i - 2;
            }
            if (graph[i+2][j] == 1) {
                cont = true;
                if (j < current_zone.min_j)
                    current_zone.min_j = j;
                if (j > current_zone.max_j)
                    current_zone.max_j = j;
                if (i + 2 < current_zone.min_i)
                    current_zone.min_i = i + 2;
                if (i + 2 > current_zone.max_i)
                    current_zone.max_i = i + 2;

                graph[i][j] = 0;
                i = i + 2;
            }
            if (graph[i][j-2] == 1) {
                cont = true;
                if (j - 2< current_zone.min_j)
                    current_zone.min_j = j - 2;
                if (j - 2 > current_zone.max_j)
                    current_zone.max_j = j - 2;
                if (i < current_zone.min_i)
                    current_zone.min_i = i;
                if (i > current_zone.max_i)
                    current_zone.max_i = i;

                graph[i][j] = 0;
                j = j - 2;
            }
            if (graph[i][j+2] == 1) {
                cont = true;
                if (j + 2 < current_zone.min_j)
                    current_zone.min_j = j + 2;
                if (j + 2> current_zone.max_j)
                    current_zone.max_j = j + 2;
                if (i < current_zone.min_i)
                    current_zone.min_i = i;
                if (i > current_zone.max_i)
                    current_zone.max_i = i;

                graph[i][j] = 0;
                j = j + 2;
            }
            }*/
