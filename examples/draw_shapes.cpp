/* draw_shapes.cpp
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#include <iostream>
#include <cve/draw/shape_drawer.h>
#include <cve/draw/colors.h>
#include <cve/imageio/ppm.h>

using namespace cve;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;

int main()
{
    Image8 img(300, 300, 3);
    img.setConstant(255);

    std::cout << "Apply harris detector" << std::endl;
    ShapeDrawer<float> shapeDrawer;
    shapeDrawer.drawCircle(Eigen::Vector2f{150, 150}, 10, Color8::Red(), img);

    shapeDrawer.drawRect(Eigen::Vector2f{200, 30}, {10, 15}, Color8::Green(), img);

    cve::ppm::save("shapes.ppm", img);

    return 0;
}
