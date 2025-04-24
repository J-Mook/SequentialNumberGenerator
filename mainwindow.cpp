#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Sequential Number Generator");
    this->show();
    initTable();
    resizeEvent();
    LoadData();

    ui->toolButton_Copy->hide();
}

MainWindow::~MainWindow()
{
    SaveData();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(CreateString());

    // QFile file(QCoreApplication::applicationDirPath() + "\\Output.txt");
    // if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
    //     QTextStream out(&file);
    //     out << CreateString();
    //     file.close();
    // }
}

QString MainWindow::CreateString(){

    QString PostText = ui->textEdit_post_text->toPlainText();
    QString RepeatText = ui->textEdit_repeat_text->toPlainText();
    int RepeatNum = ui->lineEdit_repeat->text().toInt();
    bool AutoEnter = (ui->checkBox_AutoEnter->checkState() == Qt::Checked);

    if(RepeatNum <= 0) { QMessageBox::warning(this, "Warning", "Repeat Number is wrong"); return QString(""); }

    QVector<SeqData> vecSeqList = getSeqNumList();

    QString str;
    str += PostText;
    for(int nnn = 0; nnn < RepeatNum; nnn++){
        QString tempRepeatText = RepeatText;
        for(auto& seq : vecSeqList){

            QString SeqStr;
            if(seq.ExpOption)
                SeqStr = QString::number(seq.startNum, 'e', 6);
            else
                SeqStr = trimDoubletoString(seq.startNum);

            tempRepeatText.replace(QString("%%1").arg(seq.idx), SeqStr);
            
            seq.startNum += seq.Interval;
        }
        str += tempRepeatText;
        str += (AutoEnter ? QString("\n") : QString(""));
    }

    return str;
}

QVector<SeqData> MainWindow::getSeqNumList(){

    QVector<SeqData> vecSeqList;
    QTableWidget *table = ui->tableWidget_SeqNum;

    for(int rrr = 0; rrr < table->rowCount() - 1; ++rrr){
        SeqData sss;
        sss.idx = rrr + 1;
        sss.startNum = table->item(rrr, 0)->text().toDouble();
        sss.Interval = table->item(rrr, 1)->text().toDouble();
        sss.ExpOption = (table->item(rrr, 2)->checkState() == Qt::Checked);

        vecSeqList.append(sss);
    }

    return vecSeqList;
}

QString MainWindow::trimDoubletoString(double val){
    QString res = QString::number(val, 'f', 10);
    res = res.remove(QRegExp("0+$"));
    res = res.remove(QRegExp("\\.$"));
    return res;
}

void MainWindow::initTable()
{
    QTableWidget *table = ui->tableWidget_SeqNum;
    int rowCount = table->rowCount();

    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList() << "Start\nNumber" << "Interval" << "Exponenetial\nOption");


    table->insertRow(rowCount);
    QTableWidgetItem *plusItem = new QTableWidgetItem("+");
    plusItem->setTextAlignment(Qt::AlignCenter);
    table->setItem(rowCount, 0, plusItem);
    table->setSpan(rowCount, 0, 1, 3);

    connect(table, &QTableWidget::cellClicked, this, &MainWindow::onAddConsumpotionCellClicked);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    int www = ui->tableWidget_SeqNum->viewport()->width();
    ui->tableWidget_SeqNum->setColumnWidth(0, www * 0.25);
    ui->tableWidget_SeqNum->setColumnWidth(1, www * 0.25);
    ui->tableWidget_SeqNum->setColumnWidth(2, www * 0.5);
}

void MainWindow::onAddConsumpotionCellClicked(int row, int column) {
    QTableWidget *table = qobject_cast<QTableWidget *>(sender());
    if (table && row == table->rowCount() - 1) {

        QTableWidgetItem *chkItem = new QTableWidgetItem();
        chkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        chkItem->setCheckState(Qt::Unchecked);
        chkItem->setTextAlignment(Qt::AlignCenter);

        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(""));
        table->setItem(row, 1, new QTableWidgetItem(""));
        table->setItem(row, 2, chkItem);
    }
}

void MainWindow::SaveData() {
    QFile file(QCoreApplication::applicationDirPath() + "\\Data.dat");
    QTableWidget *table = ui->tableWidget_SeqNum;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "파일 열기 실패:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    // 문자열 저장
    out << ui->textEdit_post_text->toPlainText().replace("\n", "\\n") << "\n";
    out << ui->textEdit_repeat_text->toPlainText().replace("\n", "\\n") << "\n";
    out << ui->lineEdit_repeat->text() << "\n";
    out << ((ui->checkBox_AutoEnter->checkState() == Qt::Checked) ? QString("t") : QString("f")) << "\n";

    // 테이블 데이터 저장
    for (int row = 0; row < table->rowCount() - 1; ++row) {
        QStringList rowData;
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);

            if (item) {
                if (item->data(Qt::CheckStateRole).isValid())
                    rowData << (item->checkState() == Qt::Checked ? "t" : "f");
                else
                    rowData << item->text();
            }
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
}

void MainWindow::LoadData() {
    QFile file(QCoreApplication::applicationDirPath() + "\\Data.dat");
    QTableWidget *table = ui->tableWidget_SeqNum;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "파일 열기 실패:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    QString post = in.readLine();
    QString text = in.readLine();
    QString rep = in.readLine();
    QString AutoEnter = in.readLine();

    post.replace("\\n", "\n");
    text.replace("\\n", "\n");

    ui->textEdit_post_text->setText(post);
    ui->textEdit_repeat_text->setText(text);
    ui->lineEdit_repeat->setText(rep);
    ui->checkBox_AutoEnter->setCheckState(AutoEnter == QString("t") ? Qt::Checked : Qt::Unchecked);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty())
            continue;

        QStringList cols = line.split(",");

        int row = table->rowCount() - 1;
        QTableWidgetItem *chkItem = new QTableWidgetItem();
        chkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        chkItem->setCheckState(cols[2] == "t" ? Qt::Checked : Qt::Unchecked);
        ui->tableWidget_SeqNum->insertRow(row);
        ui->tableWidget_SeqNum->setItem(row, 0, new QTableWidgetItem(cols[0]));
        ui->tableWidget_SeqNum->setItem(row, 1, new QTableWidgetItem(cols[1]));
        ui->tableWidget_SeqNum->setItem(row, 2, chkItem);
    }

    file.close();
}

void MainWindow::on_tableWidget_SeqNum_customContextMenuRequested(const QPoint &pos)
{
    QTableWidget *table = ui->tableWidget_SeqNum;
    QMenu contextMenu(tr("Context menu"), this);
    QAction actionRemove("Remove", this);
    connect(&actionRemove, &QAction::triggered, [table, pos, this]() {
        int row = table->rowAt(pos.y());
        if (row >= 0) {
            table->removeRow(row);
        }
    });
    contextMenu.addAction(&actionRemove);
    contextMenu.exec(table->viewport()->mapToGlobal(pos));
}

void MainWindow::on_toolButton_Copy_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(CreateString());
}

