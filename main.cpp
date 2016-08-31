#include <iostream>
#include "lodepng.h"

using namespace std;
using namespace lodepng;

int main()
{
    string filename = "apple.png";

    vector<unsigned char> image; //the raw pixels in a list e.g. {R, G, B, A, R, G, B, A, R, ...}
    unsigned width, height;

    //decode initializes image, width, and height
    unsigned error = decode(image, width, height, filename);

    //if there's an error, display it
    if(error)
        cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;

    int radius=1;
    //if radius is zero encode 1-D image array straight to new file and end program
    if (!radius)
    {
        encode("blurred.png", image, width, height);
        return (0);
    }
    // make a 3-D array from the 1-D image array
    int i = 0;
    vector<vector <vector <unsigned char> > > original;
    original.resize(height);
    for (int row = 0; row < height; row++)
    {
        original[row].resize(width);
        for (int col = 0; col < width; col++)
        {
            original[row][col].resize(4); //image in RGBA format, matrix is thus 4 layers deep
            for (int colour = 0; colour < 4; colour++)
            {
                original[row][col][colour] = image[i];
                i++;
            }
        }
    }
    vector<vector <vector <unsigned char> > > blurred;

    //faster using vertical and horizontal blur
    blurred.resize(height);
    for (int c=0;c<4;c++)
    {
        for (int x=0;x<height;x++)
        {
            blurred[x].resize(width);
            for (int y=0;y<width;y++)
            {
                int starty=y-blur;
                int endy=y+blur;
                if (starty<0) starty=0;
                if (endy>width-1) endy=height-1;
                int accumulation=0;
                int weights=0;
                for (int i=starty;i<=endy;i++)
                {
                    accumulation+=original[x][i][c];
                    weights++;
                }
                blurred[x][y][c]=accumulation/weights;
            }
        }

        for (int y=0;y<width;y++)
        {
            for (int x=0;x<height;x++)
            {
                int startx=x-blur;
                int endx=x+blur;
                if (startx<0) startx=0;
                if (endx>height-1) endx=height-1;
                int accumulation=0;
                int weights =0;
                for (int i=startx;i<=endx;i++)
                {
                    accumulation+=original[i][y][c];
                    weights++;
                }
                blurred[x][y][c]=accumulation/weights;
             }
        }
    }

    // convert 3-D array to 1-D so that it can be encoded back as a PNG file
    vector<unsigned char> flat;
    flat.resize(height*width*4);

    int x = 0;
    for (int i = 0; i < blurred.size(); i++)
    {
        for (int j = 0; j < blurred[i].size(); j++)
        {
            for (int c = 0; c < blurred[i][j].size(); c++)
            {
                flat[x] = blurred[i][j][c];
                x++;
            }
        }
    }

    //
    encode("blurred.png", flat, width, height);

    return 0;
}
