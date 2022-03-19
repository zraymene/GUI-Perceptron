#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>

#include "neurone.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_genereButton_clicked();
    void on_apprendreButton_clicked();
    void on_validationButton_clicked();

    void on_genereFichierButton_clicked();
    void on_apprendreFichierButton_clicked();
    void on_validationFichierButton_clicked();

    void on_apprendreAlpha_textEdited(const QString &text);

private:
    Ui::MainWindow *ui;

    QChart* chart = new QChart();
    QLineSeries* chart_series = new QLineSeries();
    QValueAxis* chart_axisX = new QValueAxis();
    QValueAxis* chart_axisY = new QValueAxis();

    Neurone perceptron;

    void DessinerErreursGraph(uint32_t* erreurs, int nombre_iteration);
};

#endif // MAINWINDOW_H
