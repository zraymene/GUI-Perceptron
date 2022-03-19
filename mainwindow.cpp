#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QIntValidator>
#include <QDoubleValidator>

#include <QFileDialog>
#include <QElapsedTimer>

QElapsedTimer timer;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIntValidator* int_validator = new QIntValidator(0, 1410065407);
    QDoubleValidator* double_validator = new QDoubleValidator();
    QDoubleValidator* double_validator_range = new QDoubleValidator();
    double_validator_range->setRange(0, 1, 6);

    ui->genere_nombre_exemple->setValidator(int_validator);
    ui->genere_a->setValidator(double_validator);
    ui->genere_b->setValidator(double_validator);
    ui->genere_c->setValidator(double_validator);

    ui->apprendre_nombre_exemple->setValidator(int_validator);
    ui->apprendre_nombre_iteration->setValidator(int_validator);

    ui->apprendreAlpha->setValidator(double_validator_range);
    ui->apprendre_biais->setValidator(double_validator);
    ui->apprendre_w1->setValidator(double_validator);
    ui->apprendre_w2->setValidator(double_validator);

    ui->validation_nombre_exemple->setValidator(int_validator);

    // Graph (chart)
    chart->legend()->hide();
    chart->addSeries( chart_series );
    chart->setTitle("La courbe d’évolution du nombre d’erreur");

    chart_axisX->setLabelFormat("%d");
    chart_axisY->setLabelFormat("%d");
    chart->addAxis( chart_axisX, Qt::AlignBottom );
    chart->addAxis( chart_axisY, Qt::AlignLeft);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint( QPainter::Antialiasing );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_genereButton_clicked()
{
    timer.start();

    GenereEnsemblePoint( ui->genere_fichier->text(),
                         ui->genere_nombre_exemple->text().toInt(),
                         ui->genere_a->text().toFloat(),
                         ui->genere_b->text().toFloat(),
                         ui->genere_c->text().toFloat() );

    ui->resultats_temps->setText(
                QString::number( timer.elapsed() / 1000.0f ) );
    ui->resultats_nombre_exemple->setText(
                ui->genere_nombre_exemple->text() );
}

void MainWindow::on_apprendreButton_clicked()
{
    int nombre_exemple = ui->apprendre_nombre_exemple->text().toInt();
    int nombre_iteration = ui->apprendre_nombre_iteration->text().toInt();
    float alpha = ui->apprendreAlpha->text().toFloat();
    float biais = ui->apprendre_biais->text().toFloat();
    float w1 = ui->apprendre_w1->text().toFloat();
    float w2 = ui->apprendre_w2->text().toFloat();

    Exemple* base = (Exemple*)malloc( sizeof(Exemple) * nombre_exemple );
    if(!LireEnsemblePoint( ui->apprendre_fichier->text(),
                       base, &nombre_exemple ))
    {
        free(base);
        return;
    }
    ui->apprendre_nombre_exemple->setText(
                QString::number( nombre_exemple ) );

    uint32_t* erreurs = (uint32_t*)malloc( sizeof(uint32_t) * nombre_iteration);

    perceptron.biais = biais;
    perceptron.poids[0] = w1;
    perceptron.poids[1] = w2;

    timer.start();

    Apprendre(&perceptron,
              base, nombre_exemple,
              nombre_iteration,
              alpha,
              erreurs);

     uint32_t dernier_erreur = 0;
    if(nombre_iteration != 0)
        dernier_erreur = erreurs[ nombre_iteration - 1];

    uint32_t succes = nombre_exemple - dernier_erreur;
    float succ_per = (succes*100)/ nombre_exemple;

    ui->resultats_temps->setText(
                QString::number( timer.elapsed() / 1000.0f ) );
    ui->perceptron_alpha->setText( ui->apprendreAlpha->text() );
    ui->perceptron_biais->setText(
                QString::number( perceptron.biais ) );
    ui->perceptron_w1->setText(
                QString::number( perceptron.poids[0] ) );
    ui->perceptron_w2->setText(
                QString::number( perceptron.poids[1] ) );
    ui->perceptron_erreurs->setText(
                QString::number( dernier_erreur ) );
    ui->perceptron_erreurs_percentage->setText( "(" +
                QString::number( 100 - succ_per ) + "%)" );
    ui->perceptron_succes->setText(
                 QString::number( succes ) );
    ui->perceptron_succes_percentage->setText( "(" +
                QString::number( succ_per ) + "%)" );

    this->DessinerErreursGraph(erreurs, nombre_iteration );

    free(erreurs);
    free(base);
}

void MainWindow::on_validationButton_clicked()
{
    int nombre_exemple = ui->validation_nombre_exemple->text().toInt();

    Exemple* base = (Exemple*)malloc( sizeof(Exemple) * nombre_exemple );
    if(!LireEnsemblePoint( ui->validation_fichier->text(),
                       base, &nombre_exemple ))
    {
        free(base);
        return;
    }

    ui->validation_nombre_exemple->setText(
                QString::number( nombre_exemple ) );


    uint32_t erreurs = 0;

    timer.start();
    for( int i = 0; i < nombre_exemple ; i++)
    {
        int8_t label = LabelPoint( perceptron.poids[0],
                                   perceptron.poids[1],
                                   perceptron.biais,
                                   base[i].x[0], base[i].x[1] );

        if(label != base[i].label)
            erreurs++;
    }

    ui->resultats_temps->setText(
                QString::number( timer.elapsed() / 1000.0f ) );

    uint32_t succ = nombre_exemple - erreurs;
    float succ_per = (succ  * 100) / nombre_exemple;

    ui->resultats_nombre_exemple->setText(
                QString::number( nombre_exemple ) );
    ui->resultats_erreurs->setText(
                QString::number( erreurs ) );
    ui->resultats_erreurs_percentage->setText( "(" +
                QString::number( 100 - succ_per ) + "%)" );
    ui->resultats_succes->setText(
                QString::number( succ ) );
    ui->resultats_succes_percentage->setText( "(" +
                QString::number( succ_per ) + "%)" );

    free(base);
}

void MainWindow::on_genereFichierButton_clicked()
{
    QString nom_fichier = QFileDialog::getOpenFileName(this);
    ui->genere_fichier->setText(nom_fichier);
}

void MainWindow::on_apprendreFichierButton_clicked()
{
    QString nom_fichier = QFileDialog::getOpenFileName(this);
    ui->apprendre_fichier->setText(nom_fichier);
}

void MainWindow::on_validationFichierButton_clicked()
{
    QString nom_fichier = QFileDialog::getOpenFileName(this);
    ui->validation_fichier->setText(nom_fichier);
}

void MainWindow::on_apprendreAlpha_textEdited(const QString &text)
{
    if(text.toFloat() > 1)
        ui->apprendreAlpha->setText("1");
}

void MainWindow::DessinerErreursGraph(uint32_t *erreurs, int nombre_iteration)
{
    chart_series->clear();

    uint32_t erreur_max = 0;
    for( int i = 0; i < nombre_iteration ; i++)
    {
        chart_series->append( i + 1, erreurs[i] );

        if(erreurs[i] > erreur_max)
            erreur_max = erreurs[i];
    }

    // Mise a jour le graph
    chart->removeSeries( chart_series );
    chart->addSeries( chart_series );

    // Mise a jour les axes du graph
    chart_axisX->setRange( 0, nombre_iteration );
    chart_axisY->setRange( 0, erreur_max);

    chart->setTitle("La courbe d’évolution du nombre d’erreur ("
                    + ui->apprendre_nombre_exemple->text() + " exemples),a: "
                    + ui->apprendreAlpha->text());
}

