#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scribblearea.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);

    createSaveAsMenu();
    createActionGroups();
    setData();

    connectAct();
    setActShortcuts();

    setWindowTitle(tr("My Scribble"));
    resize(500, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()) {
            scribbleArea->openImage(fileName);
        }
    }
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::undo()
{
    scribbleArea->moveHistory(-1);
}

void MainWindow::redo()
{
    scribbleArea->moveHistory(1);
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}

void MainWindow::brushColor()
{
    QColor newColor = QColorDialog::getColor(scribbleArea->brushColor());
    if (newColor.isValid())
        scribbleArea->setBrushColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInteger(this, tr("Scribble"),
                                            tr("Select pen width:"),
                                            scribbleArea->penWidth(),
                                            1, 50, 1, &ok);
    if (ok)
        scribbleArea->setPenWidth(newWidth);
}

void MainWindow::lineWidth()
{
    int value = ui->lineWidthSlider->value();
    QString textvalue = QString().setNum(value);
    scribbleArea->setPenWidth(value);
    ui->lineWidthNumber->setText(textvalue);
}

void MainWindow::shape(QAction *action)
{
    if (action) {
        scribbleArea->setShape(Shape(action->data().toInt()));
    }
    //#include <QDebug>
    //qDebug() << "Triggered: " << action->data().toInt();
}

void MainWindow::line()
{
    Qt::PenStyle style = Qt::PenStyle(ui->lineStyleComboBox->itemData(
                                      ui->lineStyleComboBox->currentIndex()).toInt());
    scribbleArea->setPenStyle(style);
}

void MainWindow::brush()
{
    Qt::BrushStyle style = Qt::BrushStyle(ui->brushStyleComboBox->itemData(
                                          ui->brushStyleComboBox->currentIndex()).toInt());
    scribbleArea->setBrushStyle(style);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About My Scribble"),
            tr("<p>The Scribble program written by Pin-hsien Li (b00901113) "
               "for Network & Multimedia Lab, 2014 Spring.</p>"
               "<p>The basis code is <b>Scribble</b>, an widget example included "
               "in QT4 widget example.</p>"));
}

void MainWindow::createSaveAsMenu()
{
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }
    foreach (QAction *action, saveAsActs)
        ui->saveAsMenu->addAction(action);
}

void MainWindow::createActionGroups()
{
    drawActionGroup = new QActionGroup(this);
    drawActionGroup->addAction(ui->drawPencilAct);
    drawActionGroup->addAction(ui->drawLineAct);
    drawActionGroup->addAction(ui->drawRectAct);
    drawActionGroup->addAction(ui->drawRoundRectAct);
    drawActionGroup->addAction(ui->drawElliAct);
    drawActionGroup->addAction(ui->eraseAct);
}

void MainWindow::setData()
{
    ui->drawPencilAct->setData(QVariant(PENCIL));
    ui->drawLineAct->setData(QVariant(LINE));
    ui->drawRectAct->setData(QVariant(RECT));
    ui->drawRoundRectAct->setData(QVariant(ROUNDRECT));
    ui->drawElliAct->setData(QVariant(ELLIPSE));
    ui->eraseAct->setData(QVariant(ERASER));

    ui->lineStyleComboBox->addItem("Solid"       , Qt::SolidLine);
    ui->lineStyleComboBox->addItem("Dash"        , Qt::DashLine);
    ui->lineStyleComboBox->addItem("Dot"         , Qt::DotLine);
    ui->lineStyleComboBox->addItem("Dash Dot"    , Qt::DashDotLine);
    ui->lineStyleComboBox->addItem("Dash Dot Dot", Qt::DashDotDotLine);
    ui->lineStyleComboBox->addItem("None"        , Qt::NoPen);

    ui->brushStyleComboBox->addItem("Solid"      , Qt::SolidPattern);
    ui->brushStyleComboBox->addItem("None"       , Qt::NoBrush);

    ui->lineWidthSlider->setRange(1, 50);
    ui->lineWidthSlider->setValue(scribbleArea->penWidth());
    QString textvalue = QString().setNum(scribbleArea->penWidth());
    ui->lineWidthNumber->setText(textvalue);
}

void MainWindow::connectAct()
{
    connect(ui->openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->exitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui->redoAct, SIGNAL(triggered()), this, SLOT(redo()));
    connect(ui->penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
    connect(ui->brushColorAct, SIGNAL(triggered()), this, SLOT(brushColor()));
    connect(ui->penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    connect(ui->printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));
    connect(ui->clearScreenAct, SIGNAL(triggered()), scribbleArea, SLOT(clearImage()));

    connect(ui->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(drawActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(shape(QAction*)));

    connect(ui->lineStyleComboBox, SIGNAL(activated(int)), this, SLOT(line()));
    connect(ui->brushStyleComboBox, SIGNAL(activated(int)), this, SLOT(brush()));

    connect(ui->lineWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(lineWidth()));
}

void MainWindow::setActShortcuts()
{
    ui->openAct->setShortcuts(QKeySequence::Open);
    ui->exitAct->setShortcuts(QKeySequence::Quit);
    ui->undoAct->setShortcuts(QKeySequence::Undo);
    ui->redoAct->setShortcuts(QKeySequence::Redo);
    ui->clearScreenAct->setShortcut(tr("Ctrl+L"));
}

bool MainWindow::maybeSave()
{
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("Scribble"),
                          tr("The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
			  | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveFile("png");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString(fileFormat.toUpper()))
                               .arg(QString(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return scribbleArea->saveImage(fileName, fileFormat);
    }
}

