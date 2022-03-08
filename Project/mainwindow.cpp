/*-----------------------------
 * MAT 500 PROJECT 3
 * Program by Srey Raychaudhuri
 * ID: 60000721
 * ----------------------------
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setting up and initializing UI
    ui->setupUi(this);

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    // Adding graphs
    ui->customPlot->addGraph();
    ui->customPlot->addGraph();

    // Initializing curve objects
    polynomialCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    interpolationPoints = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);

    // Points will appear as circles
    interpolationPoints->setScatterStyle(QCPScatterStyle::ssCircle);

    // Setting colors for two sets of data
    polynomialCurve->setPen(QPen(Qt::red));
    interpolationPoints->setPen(QPen(Qt::blue));


    // White screen - no axes visible
    ui->customPlot->xAxis->setVisible(false);
    ui->customPlot->yAxis->setVisible(false);

    //polynomialCurveData.resize(101);

    qDebug() << interpolationPointsData.size();
    pointCount = 0;

    // Setting mouse input signals and slots
    connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(onPointDrag(QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(AddPoint(QMouseEvent*)));
}


// Destroy all curves
MainWindow::~MainWindow() {
    delete ui;
}

//------ THE COMMENTED FUNCTION CALCULATES DIVIDED DIFFERENCES RECURSIVELY--------
//-----------IT'S SUPER SLOW, SO NOT USING IT, BUT LEFT IT HERE ANYWAY------------


// Find the divided difference coefficient in the Newtonian equation
//float MainWindow::DividedDiffCoeff(QVector<QCPCurveData> points, int mode) {

//    if (points.size() == 1) {
//        if(mode == 1) {
//            return points[0].key;
//        }
//        else {
//            return points[0].value;
//        }
//    }
//    else {
//        QVector<QCPCurveData> first {points.begin() + 1,  points.end()};
//        QVector<QCPCurveData> second {points.begin(),  points.end() - 1};

//        return (DividedDiffCoeff(first, mode) - DividedDiffCoeff(second, mode))/
//                (points[points.size() - 1].t - points[0].t);
//    }
//}

//-------------------------------------------------------------------------------------

/* Function to calculate divided differences for Newton's Method
 * and store them in a table. Like this
 *
 * [t0]g        [t1]g       [t2]g
 * [t0,t1]g     [t1,t2]g
 * [t0,t1,t2]g
 *
 * Each element is a pair which has calculations for x and y
 */
void MainWindow::CalcDividedDiffTable() {

    // Resize table according to number of points
    dividedDiffTable.resize(interpolationPointsData.size());

    for(int i = 0; i < dividedDiffTable.size(); i++) {
        dividedDiffTable[i].resize(interpolationPointsData.size());
    }

    // copy the x and y values of the interpolating points
    // to the first row of this vector
    for(int i = 0; i < interpolationPointsData.size(); i++) {
            dividedDiffTable[0][i] =
            {interpolationPointsData[i].key, interpolationPointsData[i].value};
    }

    if (interpolationPointsData.size() > 1) {

        int k = interpolationPointsData.size() - 1;

        // Each new row stores intermediate divided differences for that stage
        for (int i = 1; i < interpolationPointsData.size(); i++) {
            for (int j = 0; j < k; j++) {

                Pair newElement;
                newElement.x =
                       (dividedDiffTable[i-1][j+1].x - dividedDiffTable[i-1][j].x)/
                       static_cast<double>(i);

                newElement.y =
                       (dividedDiffTable[i-1][j+1].y - dividedDiffTable[i-1][j].y)/
                       static_cast<double>(i);

                dividedDiffTable[i][j] = newElement;
            }
            k--;
        }
    }
}


// Function to plot the graphs
void MainWindow::MakePlot() {

    // Removing all data for now
    polynomialCurveData.clear();

    CalcDividedDiffTable();

    // Calculating points
    for (int i = 0; i <= (pointCount - 1) * 100; i++) {

        double t = i / 100.0;

        QVector<QCPCurveData> temp;

        QCPCurveData newData = {0, 0, 0};
        for (int j = 0; j < interpolationPointsData.size(); j++) {

            temp.push_back(interpolationPointsData[j]);

            // Finding product portion
            double productTerm = 1;
            if (j > 0) {
                for (int k = 0; k < j; k++) {
                    productTerm *= (t - interpolationPointsData[k].t);
                }
            }

            // Updating x(t) and y(t)
            newData.key += dividedDiffTable[j][0].x * productTerm;
            newData.value += dividedDiffTable[j][0].y * productTerm;

        }

        newData.t = i;
        polynomialCurveData.push_back(newData);
    }

    // Setting the data for the curves
    polynomialCurve->data()->set(polynomialCurveData);
    interpolationPoints->data()->set(interpolationPointsData);

    // Replotting and updating the graphs
    ui->customPlot->replot();
    ui->customPlot->update();
}



// Function fired when mouse is clicked and dragged
void MainWindow::onPointDrag(QMouseEvent *event) {

    // Finding the point nearest to the point where the mouse is clicked and dragged
    QPoint point = event->pos();
    for (int i = 0; i < interpolationPointsData.size(); i++) {

        if (fabs(interpolationPointsData[i].key - ui->customPlot->xAxis->pixelToCoord(point.x())) <= 0.2
         && fabs(interpolationPointsData[i].value - ui->customPlot->yAxis->pixelToCoord(point.y())) <= 0.2) {

            interpolationPointsData[i].key = ui->customPlot->xAxis->pixelToCoord(point.x());
            interpolationPointsData[i].value = ui->customPlot->yAxis->pixelToCoord(point.y());
        }
    }

    // Redraw graphs
    MakePlot();
}



// Funtion to add a point on the screen
void MainWindow::AddPoint(QMouseEvent *event)
{
    QPoint point = event->pos();

    float interpolationPointX = ui->customPlot->xAxis->pixelToCoord(point.x());
    float interpolationPointY = ui->customPlot->yAxis->pixelToCoord(point.y());

    interpolationPointsData.push_back(QCPCurveData(pointCount++, interpolationPointX, interpolationPointY));

    MakePlot();
}

