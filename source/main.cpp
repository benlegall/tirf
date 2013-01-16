#include "filtre.hh"

int main(/*int argc, char** argv*/)
{
    std::vector<std::string> filenames;

    filenames.push_back("images/simple/HPIM4427.JPG");
    filenames.push_back("images/simple/HPIM4430.JPG");
    filenames.push_back("images/simple/HPIM4431.JPG");
    filenames.push_back("images/simple/HPIM4432.JPG");
    filenames.push_back("images/simple/HPIM4433.JPG");
    filenames.push_back("images/simple/HPIM4444.JPG");
    filenames.push_back("images/simple/HPIM4445.JPG");

    filenames.push_back("images/moins_simple/HPIM4422.JPG");
    filenames.push_back("images/moins_simple/HPIM4423.JPG");
    filenames.push_back("images/moins_simple/HPIM4425.JPG");
    filenames.push_back("images/moins_simple/HPIM4426.JPG");
    filenames.push_back("images/moins_simple/HPIM4438.JPG");
    filenames.push_back("images/moins_simple/HPIM4440.JPG");
    filenames.push_back("images/moins_simple/HPIM4442.JPG");
    filenames.push_back("images/moins_simple/HPIM4443.JPG");
    filenames.push_back("images/moins_simple/HPIM4446.JPG");


    std::stringstream oss;
    for (unsigned i = 0; i < filenames.size(); ++i)
    {

        //if ((i == 9) || (i == 14)) {


            filtre *fil = new filtre(filenames.at(i));
            std::cout << "IMAGE : " << i << std::endl;
            oss << "res/" << i << "result_red_zones.jpg";
            int cont = 0;
            int size = 0;
            int zone_enlarger = 2;

            while ((size == 0) && (cont <= 2)) {
                std::vector<zone> zones;

                if (cont > 0) {
                    zone_enlarger = 5;
                    std::cout << "in new while" << std::endl;
                    fil->reread(filenames.at(i));
                }

                fil->contrast(350);
                fil->binarize_red();
                fil->erode();
                fil->dilate();
                fil->connex_graph();

                zones = fil->find_red_zones();
                //fil->draw_zones(zones);
                zones = fil->find_white_zones(zones, zone_enlarger);
                fil->draw_zones_green(zones);

                fil->dilate2();
                //fil->dilate2();
                fil->evaluate_white_zone(zones);
                fil->draw_zones_blue(zones);

                fil->inc_red_coef();
                cont++;
                size = zones.size();
            }
            fil->save_filtered(oss.str());

            oss.str(std::string());
            oss.clear();
            //}
    }

    return 0;
}
