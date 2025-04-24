#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QString>
#include <QRegularExpression>
#include <QClipboard>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct SeqData
{
    int idx;
    double startNum;
    double Interval;
    bool ExpOption;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void onAddConsumpotionCellClicked(int row, int column);
    void resizeEvent(QResizeEvent *e = nullptr);
    void on_tableWidget_SeqNum_customContextMenuRequested(const QPoint &pos);
    void on_toolButton_Copy_clicked();

private:
    Ui::MainWindow *ui;
    void initTable();

    QVector<SeqData> getSeqNumList();
    QString trimDoubletoString(double val);
    QString CreateString();

    void SaveData();
    void LoadData();
        
};
#endif // MAINWINDOW_H
