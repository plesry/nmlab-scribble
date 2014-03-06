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
    createActionGroup();
    createToolsInDock();

    connectActs();
    setActShortcuts();

    updateActs();

    setWindowTitle(tr("My Scribble"));
    resize(800, 600);
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
    scribbleArea->setPenWidth(ui->penWidthSlider->value());
    updateActs();
}

void MainWindow::shape(QAction *action)
{
    if (action) {
        scribbleArea->setShape(Shape(action->data().toInt()));
    }
}


void MainWindow::pen()
{
    Qt::PenStyle style = Qt::PenStyle(ui->penStyleComboBox->itemData(
                                      ui->penStyleComboBox->currentIndex()).toInt());
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

void MainWindow::createActionGroup()
{
    drawActionGroup = new QActionGroup(this);
    drawActionGroup->addAction(ui->drawPencilAct);
    drawActionGroup->addAction(ui->drawLineAct);
    drawActionGroup->addAction(ui->drawRectAct);
    drawActionGroup->addAction(ui->drawRoundRectAct);
    drawActionGroup->addAction(ui->drawElliAct);
    drawActionGroup->addAction(ui->drawPolyAct);
    drawActionGroup->addAction(ui->drawPieAct);
    drawActionGroup->addAction(ui->drawCurveAct);
    drawActionGroup->addAction(ui->drawTextAct);
    drawActionGroup->addAction(ui->eraseAct);
    drawActionGroup->addAction(ui->selectAct);

    ui->drawPencilAct->setData(QVariant(PENCIL));
    ui->drawLineAct->setData(QVariant(LINE));
    ui->drawRectAct->setData(QVariant(RECT));
    ui->drawRoundRectAct->setData(QVariant(ROUNDRECT));
    ui->drawElliAct->setData(QVariant(ELLIPSE));
    ui->drawPolyAct->setData(QVariant(POLYGON));
    ui->drawTextAct->setData(QVariant(TEXT));
    ui->drawCurveAct->setData(QVariant(CURVE));
    ui->drawPieAct->setData(QVariant(PIE));
    ui->eraseAct->setData(QVariant(ERASER));
    ui->selectAct->setData(QVariant(SELECT));

    ui->drawLineAct->setChecked(true);

}

void MainWindow::createToolsInDock()
{
    ui->penWidthSlider->setRange(1, 50);
    ui->penWidthSlider->setValue(scribbleArea->penWidth());
    QString textvalue = QString().setNum(scribbleArea->penWidth());
    ui->penWidthNumber->setText(textvalue);

    ui->penStyleComboBox->addItem("Solid"       , Qt::SolidLine);
    ui->penStyleComboBox->addItem("Dash"        , Qt::DashLine);
    ui->penStyleComboBox->addItem("Dot"         , Qt::DotLine);
    ui->penStyleComboBox->addItem("Dash Dot"    , Qt::DashDotLine);
    ui->penStyleComboBox->addItem("Dash Dot Dot", Qt::DashDotDotLine);
    ui->penStyleComboBox->addItem("None"        , Qt::NoPen);
    ui->penStyleComboBox->setCurrentIndex(
                ui->penStyleComboBox->findData(QVariant(scribbleArea->penStyle())));

    ui->brushStyleComboBox->addItem("Solid"      , Qt::SolidPattern);
    ui->brushStyleComboBox->addItem("None"       , Qt::NoBrush);
    ui->brushStyleComboBox->setCurrentIndex(
                ui->brushStyleComboBox->findData(QVariant(scribbleArea->brushStyle())));
}

void MainWindow::connectActs()
{
    connect(ui->openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->exitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui->redoAct, SIGNAL(triggered()), this, SLOT(redo()));
    connect(ui->penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
    connect(ui->brushColorAct, SIGNAL(triggered()), this, SLOT(brushColor()));

    connect(ui->printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));
    connect(ui->clearScreenAct, SIGNAL(triggered()), scribbleArea, SLOT(clearImage()));

    connect(ui->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(drawActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(shape(QAction*)));
    connect(ui->cutAct, SIGNAL(triggered()), this, SLOT(cut()));
    connect(ui->copyAct, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    connect(ui->penStyleComboBox, SIGNAL(activated(int)), this, SLOT(pen()));
    connect(ui->brushStyleComboBox, SIGNAL(activated(int)), this, SLOT(brush()));

    connect(ui->penWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(penWidth()));
}

void MainWindow::setActShortcuts()
{
    ui->openAct->setShortcuts(QKeySequence::Open);
    ui->exitAct->setShortcuts(QKeySequence::Quit);
    ui->undoAct->setShortcuts(QKeySequence::Undo);
    ui->redoAct->setShortcuts(QKeySequence::Redo);
    ui->clearScreenAct->setShortcut(tr("Ctrl+L"));

    ui->cutAct->setShortcut(QKeySequence::Cut);
    ui->copyAct->setShortcut(QKeySequence::Copy);
    ui->pasteAct->setShortcut(QKeySequence::Paste);
}

void MainWindow::updateActs()
{
    ui->penWidthNumber->setText(QString().setNum(
                                     ui->penWidthSlider->value()));
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

void MainWindow::cut()
{
    if (scribbleArea->shape() == SELECT) {
        scribbleArea->copySelectedImage();
        scribbleArea->clearSelected(true);
    }
}

void MainWindow::copy()
{
    if (scribbleArea->shape() == SELECT) {
        scribbleArea->copySelectedImage();
        scribbleArea->clearSelected(false);
    }
}

void MainWindow::paste()
{
    if (scribbleArea->shape() == SELECT) scribbleArea->togglePasting();
}
