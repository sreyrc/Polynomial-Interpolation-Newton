/*----------------------------
 * MAT 500 PROJECT 1
 * Program by Srey Raychaudhuri
 * ID: 60000721
 * ---------------------------
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct Pair {
    double x, y;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Function to store divided differences for Newton's method
    void CalcDividedDiffTable();

    // Function to plot the graphs
    void MakePlot();

    // Function fired when left mouse button is clicked and dragged
    void onPointDrag(QMouseEvent* event);

    // Function fired when the mouse button is double clicked
    void AddPoint(QMouseEvent* event);

private:

    // A 2D Matrix (conceptually) to store values of a Pascal's Triangle
    QVector<QVector<double>> pascalsTriangle;

    Ui::MainWindow *ui;

    // No. of control points
    int pointCount;

    // The actual curve to be drawn
    QCPCurve* polynomialCurve;
    QCPCurve* interpolationPoints;

    // A vector that stores the curve to be plotted
    QVector<QCPCurveData> polynomialCurveData;
    QVector<QCPCurveData> interpolationPointsData;

    // Table to store divided differences
    QVector<QVector<Pair>> dividedDiffTable;

};
#endif // MAINWINDOW_H
