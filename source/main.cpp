#include "filtre.hh"

int main(/*int argc, char** argv*/)
{
    std::vector<std::string> filenames;
    filenames.push_back("images/moins_simple/HPIM4422.JPG");
    filenames.push_back("images/moins_simple/HPIM4423.JPG");
    filenames.push_back("images/moins_simple/HPIM4425.JPG");
    filenames.push_back("images/moins_simple/HPIM4426.JPG");
    filenames.push_back("images/moins_simple/HPIM4438.JPG");
    filenames.push_back("images/moins_simple/HPIM4440.JPG");
    filenames.push_back("images/moins_simple/HPIM4442.JPG");
    filenames.push_back("images/moins_simple/HPIM4443.JPG");
    filenames.push_back("images/moins_simple/HPIM4446.JPG");

    filenames.push_back("images/simple/HPIM4427.JPG");
    filenames.push_back("images/simple/HPIM4430.JPG");
    filenames.push_back("images/simple/HPIM4431.JPG");
    filenames.push_back("images/simple/HPIM4432.JPG");
    filenames.push_back("images/simple/HPIM4433.JPG");
    filenames.push_back("images/simple/HPIM4444.JPG");
    filenames.push_back("images/simple/HPIM4445.JPG");

    std::stringstream oss;
    for (int i = 0; i < 16; ++i)
    {
        filtre *fil = new filtre(filenames.at(i));

        fil->contrast(200);
        fil->binarize_red();
        oss << "res/" << i << "result_binary_red.jpg";
        //fil->save_filtered(oss.str());

        std::cout << oss.str() << std::endl;
        oss.str(std::string());
        oss.clear();

        fil->erode();
        fil->dilate();
        //fil->gradient_graph();
        fil->connex_graph();
        //fil->graph_color();
        //oss << "res/" << i << "result_red_contour.jpg";
        //fil->save_filtered(oss.str());

        //oss.str(std::string());
        //oss.clear();

        //fil->erode();
        //fil->dilate();
        //fil->binarize_red();
        //fil->connex_graph();
        fil->find_red_zones();

        oss << "res/" << i << "result_red_zones.jpg";
        fil->save_filtered(oss.str());
        oss.str(std::string());
        oss.clear();

        oss.flush();
    }

    return 0;
}
