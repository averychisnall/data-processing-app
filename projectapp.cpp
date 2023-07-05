#include "projectapp.h"
#include "ui_projectapp.h"
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QtCore>
#include <QColorDialog>

//Setting up global variables to count the number of graphs that have been drawn
int graphCount = 0;
int mathCount = 1;

//Creating a class to store information about data sets
class DataSet
{
private:
    QVector<double> firstColumn;
    QVector<double> secondColumn;

public:
    //Function to open a .csv file given a file name
    QVector<QVector<double>> GetFile (QString fileName)
    {        
        QFile file(fileName);
        //Error handling - checks to see if the file is in the correct format
        if (file.fileName().contains(".csv"))
        {
            //Error handling - Checks to see if a file has been opened (Will run if the menu is closed without selecting anything)
            if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << file.errorString();
            }

            QVector<double> firstColumn;
            QVector<double> secondColumn;
            QVector<QVector<double>> columns;


            //Iterates through the .csv file's data and adds each column to a vector
            while (!file.atEnd()) {
                QByteArray line = file.readLine();

                firstColumn.append(line.split(',').first().toDouble());


                QString s = line.split(',').last();
                s.chop(2);
                secondColumn.append(s.toDouble());
            }

            //Adding both columns to a 2D vector so that it can be returned as a single value
            columns.append(firstColumn);
            columns.append(secondColumn);
            return columns;
        }
        else
        {
            //Protects against the program crashing by forcing it to quit instead of returning a null value
            qDebug() << "You attempted to open a non-csv file. Datasets must be in the .csv format.";
            QApplication::quit();
        }
    };
};

//Function to construct graphs, add them to the QCustomPlot and update the two list widgets
void ProjectApp::ConstructGraph(QString graphName, QVector<double> firstColumn, QVector<double> secondColumn)
{
    //Setting plot data
    ui->customPlot->addGraph();
    ui->customPlot->graph(graphCount)->setData(firstColumn,secondColumn);
    ui->customPlot->graph(graphCount)->setName(graphName);

    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    //Rescales the axes to fit every plot currently visible on the graph
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    graphCount++;

    //Adding new items to the two list widgets
    QListWidgetItem* item = new QListWidgetItem(QString::number(graphCount), ui->listWidget);
    QListWidgetItem* item2 = new QListWidgetItem(QString::number(graphCount), ui->colourList);
}

ProjectApp::ProjectApp(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProjectApp)
{
    ui->setupUi(this);    
    //Setting the legend to be viewable
    ui->customPlot->legend->setVisible(true);

    //Base axes - will auto update as soon as the ConstructGraph function is called
    ui->customPlot->xAxis->setRange(-20,20);
    ui->customPlot->yAxis->setRange(-20,20);
}

ProjectApp::~ProjectApp()
{
    delete ui;
}

//When the button is clicked, a file explorer will open to allow the user to search for a .csv file
void ProjectApp::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "C://");
    ui->lineEdit->setText(fileName);

    QVector<QVector<double>> columns;

    //Creating a new DataSet object
    DataSet ds = *new DataSet;

    //Opening the file specified earlier
    columns = ds.GetFile(fileName);

    QVector<double> firstColumn;
    QVector<double> secondColumn;

    firstColumn = columns[0];
    secondColumn = columns[1];

    //Building the graph from the .csv file's data
    ConstructGraph(fileName, firstColumn, secondColumn);
}


//When an item in the list widget is clicked, it will show or hide that graph from the plot
void ProjectApp::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int listNum = 0;
    listNum = item->text().toInt();
    if(ui->customPlot->graph(listNum-1)->visible())
    {
        ui->customPlot->graph(listNum-1)->setVisible(false);
        ui->customPlot->replot();
    }
    else
    {
        ui->customPlot->graph(listNum-1)->setVisible(true);
        ui->customPlot->replot();
    }
}

//When the pushbutton is clicked, a file explorer will open to allow the user to select a .csv file
void ProjectApp::on_mathSetButton1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "C://");
    ui->lineEdit_2->setText(fileName);
    //Adds the file path of the selected .csv file to a list widget
    QListWidgetItem* item = new QListWidgetItem(ui->lineEdit_2->text(), ui->mathList1);
}

//Button to commence the selected calculation
void ProjectApp::on_calculateButton_clicked()
{
    bool valid = true;
    QVector<double> resultantFirstColumn, resultantSecondColumn;
    QString function = ui->comboBox->currentText();
    QString name;

    //Iterates through the list of every .csv file that the user selected by using the math set button
    for(int i = 0; i < ui->mathList1->count(); i++)
    {
        QVector<double> mathSetColumn1, mathSetColumn2;
        QVector<QVector<double>> mathSetColumns;
        DataSet mathDataSet = *new DataSet;
        //Opens each item one by one
        mathSetColumns = mathDataSet.GetFile(ui->mathList1->item(i)->text());

        mathSetColumn1 = mathSetColumns[0];
        mathSetColumn2 = mathSetColumns[1];


        //In the first iteration, the value of the result vector must be set to the initial values held in the first data set
        if (i == 0)
        {
            resultantFirstColumn.append(mathSetColumn1);
            resultantSecondColumn.append(mathSetColumn2);
        }
        else
        {
            //Error handling - Makes sure that the file being added has =< the number of columns of the first data set:
            //Avoids index_out_of_range errors
            if(mathSetColumn1.size() > resultantFirstColumn.size())
            {
                qDebug() << "The number of rows in the added data set exceeds the number in the first. Make sure the first data set you enter has the highest number of rows.";
                //Clears the list of data sets so that the user can input new ones
                ui->mathList1->clear();
                valid = false;
            }
            //If a valid data set is chosen, the rest of the function will continue
            if(valid)
            {
                //Checks to see which mathematical function the user wants to perform
                if (function == "+")
                {
                    //Iterating through each column of the data set that is being added
                    for(int i=0; i < mathSetColumn1.size() ; i++)
                    {
                        int result = resultantFirstColumn[i] + mathSetColumn1[i];
                        int result2 = resultantSecondColumn[i] + mathSetColumn2[i];

                        //Sets the resultant vector to the new value - adds each .csv file iteratively
                        resultantFirstColumn[i] = result;
                        resultantSecondColumn[i] = result2;
                    }
                    name = "Combined addition of several data sets #" + QString::number(mathCount);
                }
                else if (function == "*")
                {
                    //Iterating through each column of the data set that is being multiplied
                    for(int i=0; i < mathSetColumn1.size() ; i++)
                    {
                        int result = resultantFirstColumn[i] * mathSetColumn1[i];
                        int result2 = resultantSecondColumn[i] * mathSetColumn2[i];

                        resultantFirstColumn[i] = result;
                        resultantSecondColumn[i] = result2;


                    }
                    name = "Combined multiplication of several data sets #" + QString::number(mathCount);
                }
                else if (function == "log")
                {
                    for(int i=0; i < mathSetColumn1.size() ; i++)
                    {
                        int result = mathSetColumn1[i] + log(resultantFirstColumn[i]);
                        int result2 = mathSetColumn2[i] + log(resultantSecondColumn[i]);

                        resultantFirstColumn.append(result);
                        resultantSecondColumn.append(result2);
                    }
                 QString name =  "Logarithm of several data sets #" + QString::number(mathCount);
                }
            }
        }
    }
    //If the mathematical function has been properly carried out, a graph will be added
    if (valid)
    {
        ConstructGraph(name, resultantFirstColumn, resultantSecondColumn);
        mathCount++;
    }

}

//Allows the user to change the colour of a plot by clicking on said plot in another list widget
void ProjectApp::on_colourList_itemClicked(QListWidgetItem *item)
{
    int listNum = 0;
    listNum = item->text().toInt();
    if(ui->customPlot->graph(listNum-1)->visible())
    {
        //Opens a colour picker dialog box which allows the user to change the selected graph to any colour they choose
        QColor colour = QColorDialog::getColor(Qt::blue, this);

        ui->customPlot->graph(listNum-1)->setPen(colour);
        ui->customPlot->replot();
    }
}

