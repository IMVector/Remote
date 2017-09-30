#ifndef SEALINE_H
#define SEALINE_H

#include <QWidget>

namespace Ui {
class sealine;
}

class sealine : public QWidget
{
    Q_OBJECT

public:
    explicit sealine(QWidget *parent = 0);
    ~sealine();

    int* getSeaColor();
    int* getLandColor();
private slots:
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_comboBox_3_currentIndexChanged(int index);
    void on_comboBox_7_currentIndexChanged(int index);
    void on_comboBox_6_currentIndexChanged(int index);
    void on_comboBox_5_currentIndexChanged(int index);
    void on_comboBox_4_currentIndexChanged(int index);
    void on_comboBox_9_currentIndexChanged(int index);
    void on_comboBox_8_currentIndexChanged(int index);
    void on_comboBox_10_currentIndexChanged(int index);

private:
    Ui::sealine *ui;
    void initStyle();
signals:
    void getSealine(int *seaColor,int*landColor);
};

#endif // SEALINE_H
