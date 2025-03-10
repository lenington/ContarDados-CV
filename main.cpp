
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

/*
*
*
*/

int main() {
    Mat imagem, frame_threshold;
    string entrada, saida, arquivo;

    int somaTotal = 0; //soma total de todos os pontos encontrados

    // Lendo a imagem, especificando o caminho da saida...
    arquivo = "dices2.png"; //nome da imagem
    entrada = "images/"+arquivo;
    saida = "images/output/"+arquivo;
    imagem = imread(entrada);

    namedWindow("Imagem Original", WINDOW_NORMAL); // define a janela
    //namedWindow('image',WINDOW_NORMAL);
    imshow("Imagem Original", imagem);
    resizeWindow("Imagem Original", 600,600);

    // Para o threshold
    //adaptado de: https://docs.opencv.org/3.4/da/d97/tutorial_threshold_inRange.html
    inRange(imagem, Scalar(200, 200, 200), Scalar(255, 255, 255), frame_threshold);

    namedWindow("Threshold da Imagem", WINDOW_NORMAL);
    imshow("Threshold da Imagem", frame_threshold);
    resizeWindow("Threshold da Imagem", 600,600);

    // Salva na pasta de threshold
    imwrite("images/threshold/"+arquivo, frame_threshold);

    // Removendo pequenas regioes (retangulos)
    //adaptado de: https://docs.opencv.org/3.4/d4/d86/group__imgproc__filter.html
    Mat rect = getStructuringElement(MORPH_RECT, Size(6, 6), Point(1, 1));
    //Eliminando as particulas indesejadas o retangulo
    //(Abertura = edorir e depois dilatar o resultado da erosao)
    erode(frame_threshold, frame_threshold, rect);
    dilate(frame_threshold, frame_threshold, rect);

    // Pegando os contornos
    //Adaptado de: https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
    vector<vector<Point> > contornos; //contorno dos dados
    vector<Vec4i> hierarchy; //
    findContours(frame_threshold, contornos, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    int countPontos = 0; //contador de pontos para cada dado
    int ultimoPonto = 0; //
    double tamanhoDado = 0;
    int debugDado = 0; //variavel apenas para debug de ordem

    for(int i = 0; i < contornos.size(); i++){
        double areaContornoDado = contourArea(contornos[i]); //pega a area de contorno

        if(areaContornoDado > 10000 && areaContornoDado < 20000){ //limita a area do contorno do dado
            tamanhoDado = areaContornoDado;
            //reinicia a contagem
            countPontos = 0;
            ultimoPonto = i;

            //cout << tamanhoDado;
            //cout << "\n";
        }

        //se o contorno estiver dentro da area do dado, ele possuira uma porcentagem
        //tirado por regra de tres, essa porcentagem geralmente esta entre:
        //0,029 e 0,045
        double x = areaContornoDado/tamanhoDado;

        if((x >= 0.019 && x <= 0.075) && (tamanhoDado > 10000 && tamanhoDado < 20000)){

            if (hierarchy[i][3] == -1){ //significa que o contorno do dado acabou
                // Reinicia quando um novo contorno eh encontrado
                countPontos = 0;
                ultimoPonto = i;
            } else {

                countPontos += 1;

                // Verifica dentro do contorno se eh o ultimo ponto (hierarchy[i + 1][3] == -1)
                if ((i == contornos.size() - 1) || (hierarchy[i + 1][3] == -1)){
                    somaTotal += countPontos;
                    cout << "Dado de ";
                    cout << countPontos;
                    cout << "\n";

                    debugDado = debugDado + 1;

                    stringstream stream;
                    stream << countPontos;
                    Moments m = moments(contornos[ultimoPonto]); //centro do contorno

                    //adaptado de: https://docs.opencv.org/3.4.7/dc/dcf/tutorial_js_contour_features.html
                    Point p = Point(m.m10 / m.m00 + 10, m.m01 / m.m00);
                    putText(imagem, stream.str(), p, FONT_HERSHEY_PLAIN, 2, Scalar(255,0,0),3);

                    countPontos = 0;
                    ultimoPonto = i;

                    /*stringstream stream2;
                    stream2 << debugDado;
                    p = Point(m.m10 / m.m00 - 10, m.m01 / m.m00);
                    putText(img, stream2.str(), p, FONT_HERSHEY_PLAIN, 2, Scalar(128,0,128),3);


                    cout << tamanhoDado;
                    cout << "\n\n";*/
                }
            }
            drawContours(imagem, contornos, i, Scalar(0,128,0), 2, 8, hierarchy, 0, Point());

            //cout << countPontos;
            //cout << "\n";
        }

    }

    // Escrevendo o resultado na imagem
    stringstream stream;
    stream << "Total: " << somaTotal;
    putText(imagem, stream.str(), Point(30, 50), FONT_HERSHEY_PLAIN, 2, Scalar(0,0,0), 3);

    // Salva na pasta de output o resultado final
    imwrite(saida, imagem);

    namedWindow("Resultado da Imagem", WINDOW_NORMAL);
    imshow("Resultado da Imagem", imagem);
    resizeWindow("Resultado da Imagem", 600,600);


    waitKey(0);
    return 0;
}
